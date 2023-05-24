// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package compile

import (
	"fmt"
	"io/ioutil"
	"os"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/driver/driver"
	"visionai/tools/vaictl/pkg/common"
)

func newCompilerCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "compile ANALYSIS_PROGRAM_FNAME",
		Short: "Compile analysis graphs",
		Long:  `Compile analysis graphs into a runtime deployable representation.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			// Setup the compiler context.
			dctx, err := driver.NewDefaultContext()
			if err != nil {
				return err
			}
			dctx.Verbose = common.Verbose
			dctx.CodegenOptions.BackendName = viper.GetString("compile.march")
			dctx.CodegenOptions.RunMode = viper.GetString("compile.runMode")
			content, err := ioutil.ReadFile(args[0])
			if err != nil {
				return fmt.Errorf("failed to read the file %q: %v", args[0], err)
			}
			dctx.AttributeOverrides = viper.GetStringSlice("compile.attr")
			dctx.InputProgramText = string(content)

			// Run the compiler.
			err = driver.Compile(dctx)
			if err != nil {
				return err
			}

			// Write the compiled output.
			outf, err := os.Create(viper.GetString("compile.output"))
			if err != nil {
				return err
			}
			defer outf.Close()
			_, err = outf.WriteString(dctx.OutputProgramText)
			if err != nil {
				return err
			}
			return nil
		},
	}

	command.Flags().StringP(
		"output", "o", "lva.out",
		"Path to save the compiled output.",
	)
	viper.BindPFlag("compile.output", command.Flags().Lookup("output"))
	viper.SetDefault("compile.output", "lva.out")

	command.Flags().StringP(
		"march", "", "k8s_one",
		"Name of the backend target.",
	)
	viper.BindPFlag("compile.march", command.Flags().Lookup("march"))
	viper.SetDefault("compile.march", "k8s_one")

	command.Flags().StringP(
		"run-mode", "", "live",
		"The mode under which to run the analysis.",
	)
	viper.BindPFlag("compile.runMode", command.Flags().Lookup("run-mode"))
	viper.SetDefault("compile.runMode", "live")

	command.Flags().StringSliceP(
		"attr", "", []string{},
		"A list of attributes and their values used for overriding. The format is \"<analyzer_name>:<attribute_name>=<override_value>\"",
	)
	viper.BindPFlag("compile.attr", command.Flags().Lookup("attr"))
	viper.SetDefault("compile.attr", []string{})

	return command
}
