// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"github.com/spf13/cobra"
	"visionai/tools/vaictl/pkg/common"
)

var (
	// CreateCmd is the root for the create command.
	CreateCmd = newCreateCmd()

	// ListCmd is the root for the list command.
	ListCmd = newListCmd()

	// DescribeCmd is the root for the describe command.
	DescribeCmd = newDescribeCmd()

	// DeleteCmd is the root for the delete command.
	DeleteCmd = newDeleteCmd()

	// EnableCmd is the root for the enable command.
	EnableCmd = newEnableCmd()

	// DisableCmd is the root for the disable command.
	DisableCmd = newDisableCmd()

	createClusterCmd = newCreateClusterCmd()
	listClustersCmd  = newListClustersCmd()
	deleteClusterCmd = newDeleteClusterCmd()

	createStreamCmd = newCreateStreamCmd()
	listStreamsCmd  = newListStreamsCmd()
	deleteStreamCmd = newDeleteStreamCmd()

	enableStreamMwhExportCmd  = newEnableStreamMwhExportCmd()
	disableStreamMwhExportCmd = newDisableStreamMwhExportCmd()

	enableStreamHlsPlaybackCmd  = newEnableStreamHlsPlaybackCmd()
	disableStreamHlsPlaybackCmd = newDisableStreamHlsPlaybackCmd()
)

func init() {
	CreateCmd.AddCommand(createClusterCmd)
	ListCmd.AddCommand(listClustersCmd)
	DeleteCmd.AddCommand(deleteClusterCmd)

	CreateCmd.AddCommand(createStreamCmd)
	ListCmd.AddCommand(listStreamsCmd)
	DeleteCmd.AddCommand(deleteStreamCmd)

	EnableCmd.AddCommand(enableStreamMwhExportCmd)
	DisableCmd.AddCommand(disableStreamMwhExportCmd)

	EnableCmd.AddCommand(enableStreamHlsPlaybackCmd)
	DisableCmd.AddCommand(disableStreamHlsPlaybackCmd)
}

func newCreateCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "create",
		Short:         "Create API resources in Vision AI.",
		Long:          `Create API resources in Vision AI.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}
	return command
}

func newListCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "list",
		Short:         "List API resources in Vision AI.",
		Long:          `List API resources in Vision AI.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}
	return command
}

func newDeleteCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "delete",
		Short:         "Delete API resources in Vision AI.",
		Long:          `Delete API resources in Vision AI.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}
	return command
}

func newEnableCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "enable",
		Short:         "Enable features for specific resources in Vision AI.",
		Long:          `Enable features for specific resources in Vision AI.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}
	return command
}

func newDisableCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "disable",
		Short:         "Disable features for specific resources in Vision AI.",
		Long:          `Disable features for specific resources in Vision AI.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}
	return command
}

func newDescribeCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "describe",
		Short:         "Describe API resources in Vision AI.",
		Long:          `Describe API resources in Vision AI.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}
	return command
}
