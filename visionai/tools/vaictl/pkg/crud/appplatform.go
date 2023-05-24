// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package crud

import (
	"fmt"
	"time"

	"github.com/spf13/cobra"
	"google3/third_party/golang/go_pretty/table/table"
	"google3/third_party/visionai/golang/pkg/appplatform/client/client"
	"visionai/golang/pkg/util"
	"visionai/tools/vaictl/pkg/common"

	pb "google3/google/cloud/visionai/v1alpha1_platform_go_proto"
)

func newListApplicationsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "applications",
		Short: "List Applications.",
		Long:  "List Applications.",
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			parentName, err := util.MakeProjectLocationName(
				common.ProjectID, common.LocationID)
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			applications, err := managerClient.ListApplications(parentName)
			if err != nil {
				return err
			}
			printApplications(applications)
			return nil
		},
	}
	return command
}

func newListInstancesCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "instances APPLICATION_ID",
		Short: "List Instances.",
		Long:  "List Instances.",
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			parentName, err := util.MakeApplicationName(
				common.ProjectID, common.LocationID, args[0])
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			instances, err := managerClient.ListInstances(parentName)
			if err != nil {
				return err
			}
			printInstances(instances)
			return nil
		},
	}
	return command
}

func printApplications(applications []*pb.Application) {
	tw := table.NewWriter()
	// Defines the maximum character length of the columns
	tw.SetColumnConfigs([]table.ColumnConfig{
		{
			Name:     "ID",
			WidthMax: 35,
		},
		{
			Name:     "Display Name",
			WidthMax: 35,
		},
		{
			Name:     "Description",
			WidthMax: 35,
		},
		{
			Name:     "Application Configs",
			WidthMax: 50,
		},
		{
			Name:     "Create Time",
			WidthMax: 35,
		},
	})

	tw.AppendHeader(table.Row{"ID", "Display Name", "Description", "Application Configs", "Create Time"})
	for _, application := range applications {
		id := util.ResourceNameToID(application.GetName())
		displayName := application.GetDisplayName()
		description := application.GetDescription()
		config := application.GetApplicationConfigs()
		ct := time.Unix(application.GetCreateTime().GetSeconds(), 0).Format(time.RFC3339)
		tw.AppendRow(table.Row{id, displayName, description, config, ct})
	}

	tw.SortBy([]table.SortBy{{Name: "Create Time", Mode: table.Asc}})
	tw.SetStyle(table.StyleColoredBright)
	tw.SetAutoIndex(true)
	fmt.Printf("%s\n", tw.Render())
}

func printInstances(instances []*pb.Instance) {
	tw := table.NewWriter()
	// Defines the maximum character length of the columns
	tw.SetColumnConfigs([]table.ColumnConfig{
		{
			Name:     "ID",
			WidthMax: 35,
		},
		{
			Name:     "Display Name",
			WidthMax: 35,
		},
		{
			Name:     "Description",
			WidthMax: 35,
		},
		{
			Name:     "Instance Type",
			WidthMax: 35,
		},
		{
			Name:     "Input Resources",
			WidthMax: 50,
		},
		{
			Name:     "Output Resources",
			WidthMax: 50,
		},
		{
			Name:     "Create Time",
			WidthMax: 35,
		},
	})

	tw.AppendHeader(table.Row{"ID", "Display Name", "Description", "Instance Type", "Input Resources", "Output Resources", "Create Time"})
	for _, instance := range instances {
		id := util.ResourceNameToID(instance.GetName())
		displayName := instance.GetDisplayName()
		description := instance.GetDescription()
		instanceType := instance.GetInstanceType()
		inputResources := instance.GetInputResources()
		outputResources := instance.GetOutputResources()
		ct := time.Unix(instance.GetCreateTime().GetSeconds(), 0).Format(time.RFC3339)
		tw.AppendRow(table.Row{id, displayName, description, instanceType, inputResources, outputResources, ct})
	}

	tw.SortBy([]table.SortBy{{Name: "Create Time", Mode: table.Asc}})
	tw.SetStyle(table.StyleColoredBright)
	tw.SetAutoIndex(true)
	fmt.Printf("%s\n", tw.Render())
}
