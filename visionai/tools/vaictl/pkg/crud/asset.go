// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"fmt"

	wpb "google3/google/cloud/visionai/v1alpha1_warehouse_go_proto"
	"github.com/spf13/cobra"
	"google3/third_party/visionai/golang/pkg/warehouse/client/client"
	"google3/third_party/visionai/golang/pkg/warehouse/util/util"
	"visionai/tools/vaictl/pkg/common"
)

func newCreateAssetCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "asset CORPUS_ID",
		Short: "Create an asset.",
		Long:  `Create an asset.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			corpusName, err := util.MakeCorpusName(
				common.ProjectID, common.LocationID, args[0])
			if err != nil {
				return err
			}

			asset := wpb.Asset_builder{}.Build()

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.CreateAsset(corpusName, asset)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}

func newListAssetsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "assets CORPUS_ID",
		Short: "List assets.",
		Long:  `List assets under the given corpus.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			corpusName, err := util.MakeCorpusName(
				common.ProjectID, common.LocationID, args[0])
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			assets, err := managerClient.ListAssets(corpusName)
			if err != nil {
				return err
			}

			// TODO: pretty-print.
			for _, asset := range assets {
				fmt.Printf("%v\n", asset)
			}

			return nil
		},
	}
	return command
}

func newDeleteAssetCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "asset CORPUS_ID ASSET_ID",
		Short: "Delete an asset.",
		Long:  `Delete the asset ASSET_ID that resides under the given corpus CORPUS_D.`,
		Args:  cobra.ExactArgs(2),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			assetName, err := util.MakeAssetName(
				common.ProjectID, common.LocationID, args[0], args[1])
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.DeleteAsset(assetName)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}
