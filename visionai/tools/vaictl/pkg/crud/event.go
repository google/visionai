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

func newListEventsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "events",
		Short: "List events.",
		Long:  `List events.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			listOptions := &exec.ListEventsOptions{
				RootOptions: newRootOptions(),
			}
			return exec.ListEvents(listOptions)
		},
	}
	return command
}
