// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"fmt"
	"time"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	"github.com/spf13/cobra"
	"google3/third_party/golang/go_pretty/table/table"
	"github.com/spf13/viper"
	"google3/third_party/visionai/golang/pkg/lva/client/client"
	lutil "google3/third_party/visionai/golang/pkg/lva/util/util"
	"visionai/golang/pkg/util"
	"visionai/tools/vaictl/pkg/common"
)

func newCreateProcessCmd() *cobra.Command {
	command := &cobra.Command{
		Use: `process ANALYSIS_ID [-p PROCESS_ID]
		[-a ANALYZER_NAME:ATTRIBUTE_NAME=VALUE]... [OTHER_OPTIONS]`,
		Short: "Create a process.",
		Long:  `This command will create a process running the analysis ANALYSIS_ID.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			analysisID := args[0]
			processID := viper.GetString("process.processID")
			if processID == "" {
				processID = randomResourceID(defaultResourceIDLength)
			}
			attributeOverrides := viper.GetStringSlice("process.attributeOverrides")
			runModeString := viper.GetString("process.runMode")
			retryCount := viper.GetInt("process.retryCount")
			process, err := makeProcess(analysisID, attributeOverrides, runModeString, retryCount)
			if err != nil {
				return err
			}
			if common.Verbose {
				fmt.Printf("%v\n", process)
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

			err = managerClient.CreateProcess(clusterName, processID, process)
			if err != nil {
				return err
			}
			return nil
		},
	}

	command.Flags().StringSliceP(
		"attribute-overrides", "a",
		[]string{},
		"The attribute overrides. Syntax: <analyzer-name>:<attribute-name>=<value>.",
	)
	viper.BindPFlag("process.attributeOverrides",
		command.Flags().Lookup("attribute-overrides"))

	command.Flags().StringP(
		"process-id", "", "",
		"If non-empty, create the process with the given id. Otherwise, a random id will be generated.",
	)
	viper.BindPFlag("process.processID",
		command.Flags().Lookup("process-id"))

	command.Flags().StringP(
		"run-mode", "", "live",
		"The run mode for the process. It is either \"live\" or \"submission\".",
	)
	viper.BindPFlag("process.runMode",
		command.Flags().Lookup("run-mode"))

	command.Flags().IntP("retry-count", "", 0, "The number of retry if the process failed")
	viper.BindPFlag("process.retryCount",
		command.Flags().Lookup("retry-count"))

	return command
}

func newListProcessesCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "processes",
		Short: "List processes.",
		Long:  `List processes.`,
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

			processes, err := managerClient.ListProcesses(clusterName)
			if err != nil {
				return err
			}

			printProcesses(processes)

			return nil
		},
	}
	return command
}

func printProcesses(processes []*lvapb.Process) {
	fmt.Printf("Total %d processes\n", len(processes))
	printProcessesTable(processes)
}

func newDeleteProcessCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "process PROCESS_ID",
		Short: "Delete a process.",
		Long:  `Delete a process.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			processName, err := lutil.MakeProcessName(
				common.ProjectID, common.LocationID, common.ClusterID, args[0])
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.DeleteProcess(processName)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}

// Introducing the go_pretty/table: https://g3doc.corp.google.com/third_party/golang/go_pretty/table/README.md?cl=head
// To prettyprint the processes.
func printProcessesTable(processes []*lvapb.Process) {
	tw := table.NewWriter()
	// Defines the maximum character length of the columns
	tw.SetColumnConfigs([]table.ColumnConfig{
		{
			Name:     "ID",
			WidthMax: 35,
		},
		{
			Name:     "Analysis ID",
			WidthMax: 35,
		},
		{
			Name:     "Reason",
			WidthMax: 35,
		},
	})
	tw.AppendHeader(table.Row{"ID", "Analysis ID", "Run Mode", "Run Status", "Create Time", "Reason"})
	for _, process := range processes {
		processID := util.ResourceNameToID(process.GetName())
		analysisID := util.ResourceNameToID(process.GetAnalysis())
		runStatus := process.GetRunStatus().GetState()
		runMode := process.GetRunMode()
		createTime := time.Unix(process.GetCreateTime().GetSeconds(), 0).Format(time.RFC3339)
		reason := process.GetRunStatus().GetReason()
		tw.AppendRow(table.Row{processID, analysisID, runMode, runStatus, createTime, reason})
	}
	// Custom sort the analyses by the ascending order of the "Create Time"
	tw.SortBy([]table.SortBy{{Name: "Create Time", Mode: table.Asc}})
	tw.SetStyle(table.StyleColoredBright)
	tw.SetAutoIndex(true)
	fmt.Printf("%s\n", tw.Render())
}

func makeProcess(analysisID string, attributeOverrides []string, runModeString string, retryCount int) (*lvapb.Process, error) {
	analysisName, err := lutil.MakeAnalysisName(
		common.ProjectID, common.LocationID, common.ClusterID, analysisID)
	if err != nil {
		return nil, err
	}

	runMode := lvapb.RunMode_RUN_MODE_UNSPECIFIED
	switch runModeString {
	case "live":
		runMode = lvapb.RunMode_LIVE
	case "submission":
		runMode = lvapb.RunMode_SUBMISSION
	default:
		return nil, fmt.Errorf("given the unknown run mode %q; this must be either \"live\" or \"submission\"", runModeString)
	}

	process := lvapb.Process_builder{
		Analysis:           analysisName,
		RunMode:            runMode,
		AttributeOverrides: attributeOverrides,
		RetryCount:         int32(retryCount),
	}.Build()

	return process, nil
}
