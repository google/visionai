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

func newCreateCorpusCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "corpus DISPLAY_NAME",
		Short: "Create a corpus.",
		Long:  `Create a corpus.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			projectLocationName, err := util.MakeProjectLocationName(
				common.ProjectID, common.LocationID)
			if err != nil {
				return err
			}

			corpus := wpb.Corpus_builder{
				DisplayName: args[0],
			}.Build()

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			err = managerClient.CreateCorpus(projectLocationName, corpus)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}

func newListCorporaCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "corpora",
		Short: "List corpora.",
		Long:  `List corpora.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			projectLocationName, err := util.MakeProjectLocationName(
				common.ProjectID, common.LocationID)
			if err != nil {
				return err
			}

			managerClient, err := client.NewManager(common.ServiceEndpoint)
			if err != nil {
				return err
			}

			corpora, err := managerClient.ListCorpora(projectLocationName)
			if err != nil {
				return err
			}

			// TODO: pretty-print.
			for _, corpus := range corpora {
				fmt.Printf("%v\n", corpus)
			}

			return nil
		},
	}
	return command
}

func newDeleteCorpusCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "corpus CORPUS_ID",
		Short: "Delete a corpus.",
		Long:  `Delete a corpus.`,
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

			err = managerClient.DeleteCorpus(corpusName)
			if err != nil {
				return err
			}

			return nil
		},
	}
	return command
}
