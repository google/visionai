// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package visualize

import (
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"visionai/golang/pkg/streams/exec"
	"visionai/tools/vaictl/pkg/common"
)

func newVisualizeCmd() *cobra.Command {

	videoStreamIDKey := common.UniquifyViperKey("streamsVisualize", "videoStreamID")
	annotationStreamIDKey := common.UniquifyViperKey("streamsVisualize", "annotationStreamID")
	summaryOnlyKey := common.UniquifyViperKey("streamsVisualize", "summaryOnly")
	tryDecodeProtobufKey := common.UniquifyViperKey("streamsVisualize", "tryDecodeProtobuf")
	sameFpsKey := common.UniquifyViperKey("streamsVisualize", "sameFps")
	outputVideoFilePathKey := common.UniquifyViperKey("streamsVisualize", "outputVideoFilePath")

	command := &cobra.Command{
		Use:           "visualize",
		Short:         "Receive and visualize annotated Vision AI streams",
		Long:          `Receive packets from a video stream and its annotation stream, then visualize annotated stream.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			visualizationAppOptions := &exec.VisualizationAppOptions{
				VideoStreamID:       viper.GetString(videoStreamIDKey),
				AnnotationStreamID:  viper.GetString(annotationStreamIDKey),
				SummaryOnly:         viper.GetBool(summaryOnlyKey),
				TryDecodeProtobuf:   viper.GetBool(tryDecodeProtobufKey),
				SameFps:             viper.GetBool(sameFpsKey),
				OutputVideoFilePath: viper.GetString(outputVideoFilePathKey),

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
			return exec.VisualizationApp(visualizationAppOptions)
		},
	}

	command.PersistentFlags().StringP(
		"receiver_id", "", "",
		"A name to identify the receiver.",
	)
	viper.BindPFlag("receive.receiverName", command.PersistentFlags().Lookup("receiver_id"))
	viper.SetDefault("receive.receiverName", "")

	command.PersistentFlags().StringP(
		"event_id", "e", "",
		"The event id to receive from.",
	)
	viper.BindPFlag("receive.eventID", command.PersistentFlags().Lookup("event_id"))
	viper.SetDefault("receive.eventID", "")

	command.PersistentFlags().StringP(
		"v_stream_id", "", "", "ID of the video stream.",
	)
	viper.BindPFlag(videoStreamIDKey, command.PersistentFlags().Lookup("v_stream_id"))
	viper.SetDefault(videoStreamIDKey, "")

	command.PersistentFlags().StringP(
		"a_stream_id", "", "", "ID of the annotation stream.",
	)
	viper.BindPFlag(annotationStreamIDKey, command.PersistentFlags().Lookup("a_stream_id"))
	viper.SetDefault(annotationStreamIDKey, "")

	command.PersistentFlags().BoolP(
		"summary_only", "", false, "Print just a short summary about the received packets.",
	)
	viper.BindPFlag(summaryOnlyKey, command.PersistentFlags().Lookup("summary_only"))
	viper.SetDefault(summaryOnlyKey, false)

	command.PersistentFlags().BoolP(
		"try_decode_protobuf", "", true, "If the package is a protobuf, try decode it against known types and print.",
	)
	viper.BindPFlag(tryDecodeProtobufKey, command.PersistentFlags().Lookup("try_decode_protobuf"))
	viper.SetDefault(tryDecodeProtobufKey, true)

	command.PersistentFlags().BoolP(
		"same_fps", "", false, "Should use same fps for both video and annotation.",
	)
	viper.BindPFlag(sameFpsKey, command.PersistentFlags().Lookup("same_fps"))
	viper.SetDefault(sameFpsKey, false)

	command.PersistentFlags().StringP(
		"output_video_filepath", "", "", "The full path and filename of the annotated video, supports only .avi format. If it is empty, no video file will be generated.",
	)
	viper.BindPFlag(outputVideoFilePathKey, command.PersistentFlags().Lookup("output_video_filepath"))
	viper.SetDefault(outputVideoFilePathKey, "")

	return command
}
