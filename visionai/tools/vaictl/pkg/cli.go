// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package pkg

import (
	"fmt"
	"os"
	"strings"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"visionai/golang/pkg/util"
	"visionai/tools/vaictl/pkg/common"
	"visionai/tools/vaictl/pkg/crud"
	"visionai/tools/vaictl/pkg/receive"
	"visionai/tools/vaictl/pkg/send"
)

var (
	rootCmd = newRootCmd()
)

func init() {
	rootCmd.AddCommand(crud.CreateCmd)
	rootCmd.AddCommand(crud.ListCmd)
	rootCmd.AddCommand(crud.DescribeCmd)
	rootCmd.AddCommand(crud.DeleteCmd)
	rootCmd.AddCommand(crud.EnableCmd)
	rootCmd.AddCommand(crud.DisableCmd)
	rootCmd.AddCommand(send.SendCmd)
	rootCmd.AddCommand(receive.ReceiveCmd)
}

// TODO(b/276759666): Implement GetOperation cmd in vaictl
func newRootCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "vaictl",
		Short: "The CLI for Vision AI.",
		Long: `This command line tool enables the user to
manage and interact with resources in Vision AI.
`,
		SilenceUsage:  true,
		SilenceErrors: true,
		Version:       util.Version(),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			common.Verbose = viper.GetBool("verbose")
			common.ServiceEndpoint = viper.GetString("serviceEndpoint")
			if !strings.Contains(common.ServiceEndpoint, ":") {
				common.ServiceEndpoint = fmt.Sprintf("%v:443", common.ServiceEndpoint)
			}
			common.ProjectID = viper.GetString("projectId")
			common.LocationID = viper.GetString("locationId")
			common.ClusterID = viper.GetString("clusterId")
			common.Timeout = viper.GetInt("timeout")
			if !common.Verbose {
				os.Setenv("GRPC_VERBOSITY", "ERROR")
			}
			return nil
		},
	}

	command.PersistentFlags().StringP(
		"service-endpoint", "",
		"visionai.googleapis.com",
		"The service endpoint for Vision AI.",
	)
	viper.BindPFlag(
		"serviceEndpoint",
		command.PersistentFlags().Lookup("service-endpoint"),
	)
	viper.SetDefault("serviceEndpoint", "visionai.googleapis.com")

	command.PersistentFlags().StringP(
		"project-id", "p",
		"",
		"The GCP project id.",
	)
	viper.BindPFlag("projectId", command.PersistentFlags().Lookup("project-id"))
	viper.SetDefault("projectId", "")

	command.PersistentFlags().StringP(
		"location-id", "l",
		"",
		"The GCP location id.",
	)
	viper.BindPFlag("locationId", command.PersistentFlags().Lookup("location-id"))
	viper.SetDefault("locationId", "")

	command.PersistentFlags().StringP(
		"cluster-id", "c",
		"",
		"The cluster id.",
	)
	viper.BindPFlag("clusterId", command.PersistentFlags().Lookup("cluster-id"))
	viper.SetDefault("clusterId", "")

	command.PersistentFlags().IntP(
		"timeout", "",
		10,
		"The default timeout (in seconds) for operations.",
	)
	viper.BindPFlag("timeout", command.PersistentFlags().Lookup("timeout"))
	viper.SetDefault("timeout", 10)

	command.PersistentFlags().BoolP(
		"verbose", "v",
		false,
		"Verbose output.",
	)
	viper.BindPFlag("verbose",
		command.PersistentFlags().Lookup("verbose"),
	)
	viper.SetDefault("verbose", false)

	return command
}

// Execute runs the vaictl command
func Execute() error {
	return rootCmd.Execute()
}
