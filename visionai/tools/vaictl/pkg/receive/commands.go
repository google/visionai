// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package receive

import (
	"strconv"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"visionai/golang/pkg/streams/exec"
	icpb "visionai/proto/ingester_config_go_proto"
	"visionai/tools/vaictl/pkg/common"
)

func newReceiveCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "receive",
		Short:         "Receive data from Vision AI.",
		Long:          `The receive command offers the user ways to receive Vision AI data from GCP.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
	}

	command.PersistentFlags().StringP(
		"receiver-name", "", "",
		"A name to identify the receiver.",
	)
	viper.BindPFlag("receive.receiverName", command.PersistentFlags().Lookup("receiver-name"))
	viper.SetDefault("receive.receiverName", "")

	command.PersistentFlags().StringP(
		"event-id", "e", "",
		"The event id to receive from.",
	)
	viper.BindPFlag("receive.eventID", command.PersistentFlags().Lookup("event-id"))
	viper.SetDefault("receive.eventID", "")

	return command
}

func newStreamsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:           "streams",
		Short:         "Receive data from Vision AI Streams.",
		Long:          `Receive data from Vision AI Streams.`,
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

func newStreamsPacketsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "packets",
		Short: "Receive packets from Vision AI Streams.",
		Long: `Receive packets from Vision AI Streams.

This command will receive and print the packets as is. It does not
attempt to interpret the packets in any way, except possibly
pretty-printing protobufs when it is possible.
`,
		Args:          cobra.ExactArgs(1),
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			receiveCatOptions := &exec.ReceiveCatOptions{
				StreamID: args[0],
				ReceiveOptions: &exec.ReceiveOptions{
					ReceiverName: viper.GetString("receive.receiverName"),
					EventID:      viper.GetString("receive.eventID"),
					RootOptions: &exec.RootOptions{
						ServiceEndpoint: common.ServiceEndpoint,
						ProjectID:       common.ProjectID,
						LocationID:      common.LocationID,
						ClusterID:       common.ClusterID,
						Verbose:         common.Verbose,
					},
				},
			}
			return exec.ReceiveCat(receiveCatOptions)
		},
	}
	return command
}

func newStreamsVideoFileCmd() *cobra.Command {
	outputKey := common.UniquifyViperKey("streamsVideoFile", "outputVideo")
	framesPerSegmentKey := common.UniquifyViperKey("streamsVideoFile", "framesPerSegment")
	command := &cobra.Command{
		Use:           "video-file",
		Short:         "Receive video files from Vision AI Streams.",
		Long:          `Receive packets from Vision AI Streams and save to mp4 video files in the specified output directory.`,
		Args:          cobra.ExactArgs(1),
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			ingesterConfig := &icpb.IngesterConfig{
				CaptureConfig: &icpb.CaptureConfig{
					Name: "StreamingServiceCapture",
					Attr: map[string]string{
						"service_endpoint": common.ServiceEndpoint,
						"project_id":       common.ProjectID,
						"location_id":      common.LocationID,
						"cluster_id":       common.ClusterID,
						"stream_id":        args[0],
					},
				},
				FilterConfig: &icpb.FilterConfig{
					Name: "SimpleSegmentFilter",
					Attr: map[string]string{
						"messages_per_segment": strconv.Itoa(viper.GetInt(framesPerSegmentKey)),
					},
				},
				EventWriterConfig: &icpb.EventWriterConfig{
					Name: "LocalVideoEventWriter",
					Attr: map[string]string{
						"output": viper.GetString(outputKey),
					},
				},
			}
			return exec.RunIngester(ingesterConfig)
		},
	}
	command.PersistentFlags().StringP(
		"output", "o", "/tmp",
		"The output video folder.",
	)
	viper.BindPFlag(outputKey, command.PersistentFlags().Lookup("output"))
	viper.SetDefault(outputKey, "/tmp")

	command.PersistentFlags().IntP(
		"frames-per-segment", "", 600,
		"The number of frames in each output mp4 video segment.",
	)
	viper.BindPFlag(framesPerSegmentKey, command.PersistentFlags().Lookup("frames-per-segment"))
	viper.SetDefault(framesPerSegmentKey, 300)

	return command
}
