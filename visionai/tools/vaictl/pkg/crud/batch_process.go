// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"fmt"
	"regexp"
	"strconv"
	"strings"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"google3/third_party/visionai/golang/pkg/gcs/client/gcsclient"
	gcsutil "google3/third_party/visionai/golang/pkg/gcs/util/util"
	"google3/third_party/visionai/golang/pkg/lva/client/client"
	lutil "google3/third_party/visionai/golang/pkg/lva/util/util"
	"visionai/golang/pkg/util"
	"visionai/tools/vaictl/pkg/common"
)

func newBatchRunProcess() *cobra.Command {
	command := &cobra.Command{
		Use: `batch-process ANALYSIS_ID [—-gcs-folder=GCS_FOLDER_PATH]
		[--file-pattern=REGEX_OF_THE_FILES ]... [OTHER_OPTIONS]`,
		Short: "Create batch process, under the submission mode.",
		Long:  `This command will create a batch process running the analysis ANALYSIS_ID against all matched files in the gcs folder. Will not trigger the running for files inside the subfolder.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			clusterName, err := util.MakeClusterName(
				common.ProjectID, common.LocationID, common.ClusterID)
			if err != nil {
				return err
			}

			analysisID := args[0]
			gcsFolder := viper.GetString("batch-process.gcsFolder")
			filePattern := viper.GetString("batch-process.filePattern")

			managerLVAClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			analysisName, err := lutil.MakeAnalysisName(
				common.ProjectID, common.LocationID, common.ClusterID, analysisID)
			if err != nil {
				return err
			}

			analysis, err := managerLVAClient.GetAnalysis(analysisName)
			if err != nil {
				return err
			}

			managerGcsClient, err := gcsclient.NewManager()
			if err != nil {
				return err
			}

			bucketName, folderPath, err := gcsutil.GcsBucketAndFolder(gcsFolder)
			if err != nil {
				return err
			}

			files, err := managerGcsClient.ListObjects(bucketName, folderPath)
			if err != nil {
				return err
			}

			createProcessRequests, err := composeCreateProcessRequests(files, filePattern, clusterName, analysisID, analysis)
			if err != nil {
				return err
			}

			if len(createProcessRequests) == 0 {
				fmt.Printf("Couldn't find any files in the gcs folder: %s can match the given file pattern: %s. Skipping batch process.\n", gcsFolder, filePattern)
				return nil
			}

			lro, err := managerLVAClient.BatchRunProcess(clusterName, createProcessRequests)
			if err != nil {
				return err
			}

			// TODO(b/276948744): [vaictl] Better print, print table for "create batch-process".
			// It should contains ProcessID, AnalysisID, GcsInputFilePath, GcsOutputFilePath for all processes
			fmt.Printf("Long-running operation: %s\n", lro.GetName())

			return nil
		},
	}

	command.Flags().StringP(
		"gcs-folder", "", "",
		"The gcs folder path of the input files. Wants gs://bucket/ or gs://bucket/folder/ format.",
	)
	viper.BindPFlag("batch-process.gcsFolder",
		command.Flags().Lookup("gcs-folder"))

	command.Flags().StringP(
		"file-pattern", "", "[-_a-zA-Z0-9]+(.mp4)",
		"The regex pattern of the files for which want to be processed.",
	)
	viper.BindPFlag("batch-process.filePattern",
		command.Flags().Lookup("file-pattern"))

	return command
}

func sourceAndSinkAnalyzers(analysis *lvapb.Analysis) (sourceAnalyzer string, sourceOperator string, sinkAnalyzer string, sinkOperator string, err error) {
	var sourceAnalyzerDefinition *lvapb.AnalyzerDefinition
	var sinkAnalyzerDefinition *lvapb.AnalyzerDefinition

	for _, analyzerDefinition := range analysis.GetAnalysisDefinition().GetAnalyzers() {
		if analyzerDefinition.GetOperator() == "GcsVideoSource" {
			sourceAnalyzerDefinition = analyzerDefinition
		}
		if analyzerDefinition.GetOperator() == "GcsVideoSink" || analyzerDefinition.GetOperator() == "GcsProtoSink" {
			sinkAnalyzerDefinition = analyzerDefinition
		}
	}

	if sourceAnalyzerDefinition == nil {
		return "", "", "", "", fmt.Errorf("The source operator is not supported in batch-process. Wants GcsVideoSource")
	}
	if sinkAnalyzerDefinition == nil {
		return "", "", "", "", fmt.Errorf("The sink operator is not supported in batch-process. Wants GcsVideoSink or GcsProtoSink")
	}

	return sourceAnalyzerDefinition.GetAnalyzer(), sourceAnalyzerDefinition.GetOperator(), sinkAnalyzerDefinition.GetAnalyzer(), sinkAnalyzerDefinition.GetOperator(), nil
}

func composeCreateProcessRequests(files []string, filePattern string, clusterName string, analysisID string, analysis *lvapb.Analysis) ([]*lvapb.CreateProcessRequest, error) {
	sourceAnalyzer, sourceOperator, sinkAnalyzer, sinkOperator, err := sourceAndSinkAnalyzers(analysis)
	if err != nil {
		return nil, err
	}

	validFilePattern := regexp.MustCompile(filePattern)

	createProcessRequests := []*lvapb.CreateProcessRequest{}
	processIDPrefix := randomResourceID(defaultResourceIDLength)

	index := 0

	for _, gcsInputFilePath := range files {
		// TODO(chenyangwei): Refractory if involves more gcs source operator in the feature, currently only allows mp4 extension.
		if validFilePattern.MatchString(gcsInputFilePath) && strings.HasSuffix(gcsInputFilePath, gcsutil.MP4Ext) {
			processID := processIDPrefix + "-" + strconv.Itoa(index)
			gcsOutputFilePath, err := gcsutil.GcsOutputFilePath(gcsInputFilePath, processID, sourceOperator, sinkOperator)
			if err != nil {
				return nil, err
			}

			attributeOverrides := makeAttributeOverrides(sourceAnalyzer, sourceOperator, sinkAnalyzer, sinkOperator, gcsInputFilePath, gcsOutputFilePath)
			process, err := makeProcess(analysisID, attributeOverrides, "submission", 0)
			if err != nil {
				return nil, err
			}

			createProcessRequests = append(createProcessRequests, lvapb.CreateProcessRequest_builder{
				Parent:    clusterName,
				ProcessId: processID,
				Process:   process,
			}.Build())

			index++
		}
	}

	return createProcessRequests, nil
}

func makeAttributeOverrides(sourceAnalyzer string, sourceOperator string, sinkAnalyzer string, sinkOperator string, gcsInputFilePath string, gcsOutputFilePath string) []string {
	var sourceAnalyzerAttributeOverride string
	var sinkAnalyzerAttributeOverride string

	switch sourceOperator {
	case "GcsVideoSource":
		sourceAnalyzerAttributeOverride = sourceAnalyzer + ":" + "input_video_gcs_path" + "=" + gcsInputFilePath
	}

	switch sinkOperator {
	case "GcsVideoSink":
		sinkAnalyzerAttributeOverride = sinkAnalyzer + ":" + "output_video_gcs_path" + "=" + gcsOutputFilePath
	case "GcsProtoSink":
		sinkAnalyzerAttributeOverride = sinkAnalyzer + ":" + "output_file_gcs_path" + "=" + gcsOutputFilePath
	}

	return []string{
		sourceAnalyzerAttributeOverride,
		sinkAnalyzerAttributeOverride,
	}
}
