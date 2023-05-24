// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"fmt"

	"github.com/spf13/cobra"
	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
	"visionai/tools/vaictl/pkg/common"
)

func newListOperatorsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "operators",
		Short: "List operators.",
		Long:  `List operators.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			opList, err := operators.DefaultOperatorList()
			if err != nil {
				return err
			}
			for _, opDef := range opList.GetOperators() {
				fmt.Printf("%v\n", opDef.GetOperator())
			}
			return nil
		},
	}
	return command
}

func newDescribeOperatorCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "operator OPERATOR_NAME",
		Short: "Describe an operator.",
		Long:  `Describe an operator.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			opList, err := operators.DefaultOperatorList()
			if err != nil {
				return err
			}
			for _, opDef := range opList.GetOperators() {
				if opDef.GetOperator() == args[0] {
					fmt.Printf("%v\n", prototext.Format(opDef))
					return nil
				}
			}
			return fmt.Errorf("could not find operator %q", args[0])
		},
	}
	return command
}
