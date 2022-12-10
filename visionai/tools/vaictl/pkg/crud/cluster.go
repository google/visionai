// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"github.com/spf13/cobra"
	"visionai/golang/pkg/streams/exec"
	"visionai/tools/vaictl/pkg/common"
)

func newCreateClusterCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "cluster CLUSTER_NAME",
		Short: "Create a cluster.",
		Long:  `Create a cluster.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			createOptions := &exec.CreateClusterOptions{
				ClusterID:   args[0],
				RootOptions: newRootOptions(),
			}
			return exec.CreateCluster(createOptions)
		},
	}
	return command
}

func newListClustersCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "clusters",
		Short: "List clusters.",
		Long:  `List clusters.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			listOptions := &exec.ListClustersOptions{
				RootOptions: newRootOptions(),
			}
			return exec.ListClusters(listOptions)
		},
	}
	return command
}

func newDeleteClusterCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "cluster CLUSTER_NAME",
		Short: "Delete a cluster.",
		Long:  `Delete a cluster.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			deleteOptions := &exec.DeleteClusterOptions{
				ClusterID:   args[0],
				RootOptions: newRootOptions(),
			}
			return exec.DeleteCluster(deleteOptions)
		},
	}
	return command
}
