// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"fmt"
	"io/ioutil"
	"strings"
	"time"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	"github.com/spf13/cobra"
	"google3/third_party/golang/go_pretty/table/table"
	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"
	"github.com/spf13/viper"
	"google3/third_party/visionai/golang/pkg/lva/client/client"
	lutil "google3/third_party/visionai/golang/pkg/lva/util/util"
	sutil "google3/third_party/visionai/golang/pkg/streams/util/util"
	"visionai/golang/pkg/util"
	"visionai/tools/vaictl/pkg/common"
)

func newCreateAnalysisCmd() *cobra.Command {
	command := &cobra.Command{
		Use: `analysis ANALYSIS_ID ANALYSIS_PROGRAM_FNAME
    [-i PLACEHOLDER_ID:STREAM_ID]... [-o PLACEHOLDER_ID:STREAM_ID]...`,
		Short: "Create an analysis.",
		Long: `This command will create an analysis that read inputs from the specified input streams and produce outputs on the specified output streams.

If the event watch is not disabled, then the LVA will listen for new events containing the specified input streams and automatically create processes from them.
`,
		Args: cobra.ExactArgs(2),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			analysis, err := makeAnalysis(args[0], args[1])
			if err != nil {
				return err
			}
			if common.Verbose {
				fmt.Printf("%v\n", analysis)
			}

			clusterName, err := util.MakeClusterName(
				common.ProjectID, common.LocationID, common.ClusterID)
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.CreateAnalysis(clusterName, args[0], analysis)
			if err != nil {
				return err
			}
			return nil
		},
	}

	command.Flags().StringSliceP(
		"inputs", "i",
		[]string{},
		"The input mappings, supplied as <placeholder-id>:<stream-id>.",
	)
	viper.BindPFlag("analysis.inputIDMappings",
		command.Flags().Lookup("inputs"))

	command.Flags().StringSliceP(
		"outputs", "o",
		[]string{},
		"The output mappings, supplied as <placeholder-id>:<stream-id>.",
	)
	viper.BindPFlag("analysis.outputIDMappings",
		command.Flags().Lookup("outputs"))

	command.Flags().BoolP(
		"disable-event-watch", "", false,
		"If true, then no event watch will be registered for this analysis.",
	)
	viper.BindPFlag("analysis.disableEventWatch",
		command.Flags().Lookup("disable-event-watch"))

	return command
}

func newListAnalysesCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "analyses",
		Short: "List analyses.",
		Long:  `List analyses.`,
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

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			analyses, err := managerClient.ListAnalyses(clusterName)
			if err != nil {
				return err
			}

			fmt.Printf("Total %d analyses\n", len(analyses))
			printAnalysesTable(analyses)

			return nil
		},
	}
	return command
}

// printAnalysesTable uses the go library go_pretty:table prety the print of the analyses.
func printAnalysesTable(analyses []*lvapb.Analysis) {
	tw := table.NewWriter()
	// Defines the maximum character length of the columns
	tw.SetColumnConfigs([]table.ColumnConfig{
		{
			Name:     "Definition",
			WidthMax: 100,
		},
	})
	tw.AppendHeader(table.Row{"ID", "Definition", "Create Time"})

	for _, analysis := range analyses {
		analysisID := util.ResourceNameToID(analysis.GetName())
		definition := analysis.GetAnalysisDefinition()
		createTime := time.Unix(analysis.GetCreateTime().GetSeconds(), 0).Format(time.RFC3339)
		tw.AppendRow(table.Row{analysisID, definition, createTime})
	}
	// Custom sort the analyses by the ascending order of the "Create Time"
	tw.SortBy([]table.SortBy{{Name: "Create Time", Mode: table.Asc}})
	tw.SetStyle(table.StyleColoredBright)
	tw.SetAutoIndex(true)
	fmt.Printf("%s\n", tw.Render())
}

func newDeleteAnalysisCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "analysis ANALYSIS_ID",
		Short: "Delete an analysis.",
		Long:  `Delete an analysis.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			analysisName, err := lutil.MakeAnalysisName(
				common.ProjectID, common.LocationID, common.ClusterID, args[0])
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.DeleteAnalysis(analysisName)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}

func makeAnalysis(analysisID string, analysisFname string) (*lvapb.Analysis, error) {
	analysisName, err := lutil.MakeAnalysisName(
		common.ProjectID, common.LocationID, common.ClusterID, analysisID)
	if err != nil {
		return nil, err
	}

	analysisDefinition, err := parseAnalysisDefinition(analysisFname)
	if err != nil {
		return nil, err
	}

	inputMappings, err := parsePlaceholderMappings(
		viper.GetStringSlice("analysis.inputIDMappings"))
	if err != nil {
		return nil, err
	}

	outputMappings, err := parsePlaceholderMappings(
		viper.GetStringSlice("analysis.outputIDMappings"))
	if err != nil {
		return nil, err
	}

	disableEventWatch := viper.GetBool("analysis.DisableEventWatch")

	analysis := lvapb.Analysis_builder{
		Name:                 analysisName,
		AnalysisDefinition:   analysisDefinition,
		DisableEventWatch:    disableEventWatch,
		InputStreamsMapping:  inputMappings,
		OutputStreamsMapping: outputMappings,
	}.Build()

	return analysis, nil
}

func parseAnalysisDefinition(fileName string) (*lvapb.AnalysisDefinition, error) {
	content, err := ioutil.ReadFile(fileName)
	if err != nil {
		return nil, fmt.Errorf("failed to read the file %q: %v", fileName, err)
	}

	analysisDefinition := &lvapb.AnalysisDefinition{}
	if err := prototext.Unmarshal(content, analysisDefinition); err != nil {
		return nil, fmt.Errorf("failed to unmarshal the contents of the file %q as an AnalysisDefinition textproto: %v", fileName, err)
	}

	return analysisDefinition, nil
}

func parsePlaceholderMappings(idMappingStrings []string) (map[string]string, error) {
	placeholderMapping := map[string]string{}
	for _, s := range idMappingStrings {
		kv := strings.Split(s, ":")
		if len(kv) != 2 {
			return nil, fmt.Errorf("given unrecognizable placeholder mapping %q (must be <placeholder-id>:<stream-id>)", s)
		}
		k := strings.TrimSpace(kv[0])
		v := strings.TrimSpace(kv[1])
		if v == "" {
			return nil, fmt.Errorf("given an empty value for placeholder %q", s)
		}
		v, err := sutil.MakeStreamName(common.ProjectID, common.LocationID, common.ClusterID, v)
		if err != nil {
			return nil, err
		}
		placeholderMapping[k] = v
	}
	return placeholderMapping, nil
}
