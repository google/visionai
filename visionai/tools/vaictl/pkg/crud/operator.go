// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"fmt"

	"github.com/spf13/cobra"
	"google3/third_party/golang/go_pretty/table/table"
	"github.com/spf13/viper"
	"google3/third_party/visionai/golang/pkg/lva/client/client"
	"visionai/golang/pkg/util"
	"visionai/tools/vaictl/pkg/common"

	lvapb "google3/google/cloud/visionai/v1_lva_go_proto"
	lutil "google3/third_party/visionai/golang/pkg/lva/util/util"
)

func newListOperatorsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "operators --operator-name OPERATOR_NAME",
		Short: "List operators.",
		Long:  `List operators.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			opName := viper.GetString("operator.name")
			projectName, err := util.MakeProjectLocationName(common.ProjectID, common.LocationID)
			if err != nil {
				return err
			}
			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			privateOps := []*lvapb.Operator{}
			publicOps := []*lvapb.Operator{}

			privateResp, err := managerClient.ListOperators(projectName)
			if err != nil {
				return err
			}
			if opName == "" {
				privateOps = privateResp
			} else {
				for _, op := range privateResp {
					if op.GetOperatorDefinition().GetOperator() == opName {
						privateOps = append(privateOps, op)
					}
				}
			}

			publicResp, err := managerClient.ListPublicOperators(projectName)
			if err != nil {
				return err
			}
			if opName == "" {
				publicOps = publicResp
			} else {
				for _, op := range publicResp {
					if op.GetOperatorDefinition().GetOperator() == opName {
						publicOps = append(publicOps, op)
					}
				}
			}
			fmt.Printf("=== Total %d operators in Public Registry === \n", len(publicOps))
			printOperatorsTable(publicOps)

			fmt.Printf("=== Total %d operators in Private Registry === \n", len(privateOps))
			printOperatorsTable(privateOps)
			return nil
		},
	}

	command.Flags().StringP(
		"operator-name", "", "",
		"The name of the operator to list. e.g. OccupancyCount",
	)
	viper.BindPFlag("operator.name", command.Flags().Lookup("operator-name"))
	return command
}

func newDeleteOperatorCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "operator OPERATOR_ID",
		Short: "Delete an operator.",
		Long:  `Delete an operator.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			operatorName, err := lutil.MakeOperatorName(common.ProjectID, common.LocationID, args[0])
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.DeleteOperator(operatorName)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}

func printOperatorsTable(operators []*lvapb.Operator) {
	tw := table.NewWriter()
	// Defines the maximum character length of the columns
	tw.SetColumnConfigs([]table.ColumnConfig{
		{
			Name:     "Operator ID",
			WidthMax: 35,
		},
		{
			Name:     "Operator Definition",
			WidthMax: 35,
		},
		{
			Name:     "Docker Image",
			WidthMax: 35,
		},
	})
	tw.AppendHeader(table.Row{"Operator ID", "Operator Definition", "Docker Image"})
	for _, operator := range operators {
		operatorID := util.ResourceNameToID(operator.GetName())
		def := operator.GetOperatorDefinition()
		image := operator.GetDockerImage()
		tw.AppendRow(table.Row{operatorID, def, image})
	}
	// Custom sort the analyses by the ascending order of the "Create Time"
	tw.SortBy([]table.SortBy{{Name: "Operator ID", Mode: table.Asc}})
	tw.SetStyle(table.StyleColoredBright)
	tw.SetAutoIndex(true)
	fmt.Printf("%s\n", tw.Render())
}
