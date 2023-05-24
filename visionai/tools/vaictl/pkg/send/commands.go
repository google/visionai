// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package send

import (
	"fmt"
	"strconv"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"visionai/golang/pkg/streams/exec"
	cspb "visionai/proto/cluster_selection_go_proto"
	icpb "visionai/proto/ingester_config_go_proto"
	"visionai/tools/vaictl/pkg/common"
)

func newSendCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "send",
		Short: "Send data to Vision AI.",
		Long: `The send command offers the user ways to ingest data from
different media sources, process them in predefined ways, and finally send them
into Vision AI services.
`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			ingesterConfig.IngesterName = viper.GetString("send.senderName")
			ingesterConfig.IngestPolicy = &icpb.IngesterConfig_IngestPolicy{
				ContinuousMode: viper.GetBool("send.continuousMode"),
				Event:          viper.GetString("send.event"),
			}
			return nil
		},
	}
	command.PersistentFlags().StringP(
		"sender-name", "n", "",
		"A name to identify the sender. Leave empty for the system to autogenerate.",
	)
	viper.BindPFlag("send.senderName", command.PersistentFlags().Lookup("sender-name"))
	viper.SetDefault("send.senderName", "")

	command.PersistentFlags().BoolP(
		"continuous-mode", "", true,
		"Whether to send in continuous mode.",
	)
	viper.BindPFlag("send.continuousMode",
		command.PersistentFlags().Lookup("continuous-mode"))
	viper.SetDefault("send.continuousMode", true)

	command.PersistentFlags().StringP(
		"event", "e", "",
		"A specific event to ingest into for continuous mode, or a specific prefix to use for sequential mode. Leave empty for the system to auto-generate.",
	)
	viper.BindPFlag("send.event", command.PersistentFlags().Lookup("event"))
	viper.SetDefault("send.event", "")

	return command
}

func newStdinCmd() *cobra.Command {
	return &cobra.Command{
		Use:           "stdin",
		Short:         "Send stdin.",
		Long:          `Send stdin.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			captureConfig := &icpb.CaptureConfig{
				Name: "GetlineCapture",
			}
			ingesterConfig.CaptureConfig = captureConfig
			return nil
		},
	}
}

func newBytesCmd() *cobra.Command {
	bytesPerMessageKey := common.UniquifyViperKey("bytes", "bytesPerMessage")
	sendPeriodKey := common.UniquifyViperKey("bytes", "sendPeriodKey")

	command := &cobra.Command{
		Use:   "bytes",
		Short: "Send bytes.",
		Long: `Send bytes to a stream at regular intervals.

Setting the bytes per message to 4166 and send period ms to 33 will
be ~1Mpbs.
`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			captureConfig := &icpb.CaptureConfig{
				Name: "BytesSenderCapture",
				Attr: map[string]string{
					"bytes_per_message": strconv.Itoa(viper.GetInt(bytesPerMessageKey)),
					"send_period_ms":    strconv.Itoa(viper.GetInt(sendPeriodKey)),
				},
			}
			ingesterConfig.CaptureConfig = captureConfig
			return nil
		},
	}

	command.PersistentFlags().IntP(
		"bytes-per-message", "", 416,
		"Number of bytes to send per message. Must be positive.",
	)
	viper.BindPFlag(bytesPerMessageKey,
		command.PersistentFlags().Lookup("bytes-per-message"))
	viper.SetDefault(bytesPerMessageKey, 416)

	command.PersistentFlags().IntP(
		"send-period-ms", "", 33,
		"The send period. Must be positive.",
	)
	viper.BindPFlag(sendPeriodKey,
		command.PersistentFlags().Lookup("send-period-ms"))
	viper.SetDefault(sendPeriodKey, 33)

	return command
}

func newSendVideoFileCmd() *cobra.Command {
	frameRateKey := common.UniquifyViperKey("sendVideoFile", "frameRate")
	filePathKey := common.UniquifyViperKey("sendVideoFile", "filePath")
	loopKey := common.UniquifyViperKey("sendVideoFile", "loop")
	command := &cobra.Command{
		Use:           "video-file",
		Short:         "Send a video file.",
		Long:          `Send a video file.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			captureConfig := &icpb.CaptureConfig{
				SourceUrls: []string{
					viper.GetString(filePathKey),
				},
			}
			if !common.NeedCaptureDecode {
				captureConfig.Name = "FileSourceCapture"
				captureConfig.Attr = map[string]string{
					"loop": strconv.FormatBool(viper.GetBool(loopKey)),
				}
			} else {
				captureConfig.Name = "FileSourceImageCapture"
				captureConfig.Attr = map[string]string{
					"frame_rate": viper.GetString(frameRateKey),
					"loop":       strconv.FormatBool(viper.GetBool(loopKey)),
				}
			}
			ingesterConfig.CaptureConfig = captureConfig
			return nil
		},
	}

	command.PersistentFlags().StringP(
		"file-path", "", "",
		"The path to the video file to be sent.",
	)
	viper.BindPFlag(filePathKey,
		command.PersistentFlags().Lookup("file-path"))
	viper.SetDefault(filePathKey, "")

	command.PersistentFlags().BoolP(
		"loop", "", false,
		"Play the video on loop.",
	)
	viper.BindPFlag(loopKey,
		command.PersistentFlags().Lookup("loop"))
	viper.SetDefault(loopKey, false)

	command.PersistentFlags().StringP(
		"frame-rate", "", "",
		"An optional output frame-rate of the capture in the format \"SECONDS/FRAME\"",
	)
	viper.BindPFlag(frameRateKey,
		command.PersistentFlags().Lookup("frame-rate"))
	viper.SetDefault(frameRateKey, "")

	return command
}

func newSendRtspCmd() *cobra.Command {
	frameRateKey := common.UniquifyViperKey("sendRtsp", "frameRate")
	rtspURIKey := common.UniquifyViperKey("sendRtsp", "rtspURI")
	command := &cobra.Command{
		Use:           "rtsp",
		Short:         "Send an rtsp endpoint.",
		Long:          `Send an rtsp endpoint.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			captureConfig := &icpb.CaptureConfig{
				SourceUrls: []string{
					viper.GetString(rtspURIKey),
				},
			}
			if !common.NeedCaptureDecode {
				captureConfig.Name = "RTSPCapture"
			} else {
				captureConfig.Name = "RTSPImageCapture"
				captureConfig.Attr = map[string]string{
					"frame_rate": viper.GetString(frameRateKey),
				}
			}
			ingesterConfig.CaptureConfig = captureConfig
			return nil
		},
	}

	command.PersistentFlags().StringP(
		"rtsp-uri", "", "",
		"The uri of the RTSP endpoint.",
	)
	viper.BindPFlag(rtspURIKey,
		command.PersistentFlags().Lookup("rtsp-uri"))
	viper.SetDefault(rtspURIKey, "")

	command.PersistentFlags().StringP(
		"frame-rate", "", "",
		"An optional output frame-rate of the capture in the format \"SECONDS/FRAME\"",
	)
	viper.BindPFlag(frameRateKey,
		command.PersistentFlags().Lookup("frame-rate"))
	viper.SetDefault(frameRateKey, "")

	return command
}

func newApplyingCmd() *cobra.Command {
	return &cobra.Command{
		Use:           "applying",
		Short:         "Select from among possible transformations.",
		Long:          `Select from among possible transformations.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			return common.CallParentPersistentPreRunE(cmd, args)
		},
	}
}

func newSimpleSegmenterCmd() *cobra.Command {
	sendSegmentLengthKey := common.UniquifyViperKey("simpleSegmenter", "sendSegmentLength")

	command := &cobra.Command{
		Use:           "simple-segmenter",
		Short:         "Partition the input stream into segments.",
		Long:          `Partition the input stream into segments.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			filterConfig := &icpb.FilterConfig{
				Name: "SimpleSegmentFilter",
				Attr: map[string]string{
					"messages_per_segment": strconv.Itoa(viper.GetInt(sendSegmentLengthKey)),
				},
			}
			ingesterConfig.FilterConfig = filterConfig

			return nil
		},
	}

	command.PersistentFlags().IntP(
		"send-segment-length", "", 0,
		"The number of messages per segment. Each segment will be sent to a different event. Non-positive values indicate no limit.",
	)
	viper.BindPFlag(sendSegmentLengthKey,
		command.PersistentFlags().Lookup("send-segment-length"))
	viper.SetDefault(sendSegmentLengthKey, 0)

	return command
}

func newMotionFilterCmd() *cobra.Command {
	eventDurationKey := common.UniquifyViperKey("encodedMotionFilter", "eventDuration")
	lookbackDurationKey := common.UniquifyViperKey("encodedMotionFilter", "lookbackDuration")
	cooldownDurationKey := common.UniquifyViperKey("encodedMotionFilter", "cooldownDuration")
	motionSensitivityKey := common.UniquifyViperKey("encodedMotionFilter", "motionSensitivity")
	zoneAnnotationKey := common.UniquifyViperKey("encodedMotionFilter", "zoneAnnotation")
	excludeAnnotatedZoneKey := common.UniquifyViperKey("encodedMotionFilter", "excludeAnnotatedZone")
	command := &cobra.Command{
		Use:           "motion-filter",
		Short:         "Perform motion filtering on the encoded input frames.",
		Long:          `Perform motion filtering on the encoded input frames.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			common.NeedCaptureDecode = false
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			filterConfig := &icpb.FilterConfig{
				Name: "EncodedMotionFilter",
				Attr: map[string]string{
					"min_event_length_in_seconds":  strconv.Itoa(viper.GetInt(eventDurationKey)),
					"lookback_window_in_seconds":   strconv.Itoa(viper.GetInt(lookbackDurationKey)),
					"cool_down_period_in_seconds":  strconv.Itoa(viper.GetInt(cooldownDurationKey)),
					"motion_detection_sensitivity": viper.GetString(motionSensitivityKey),
					"zone_annotation":              viper.GetString(zoneAnnotationKey),
					"exclude_annotated_zone":       strconv.FormatBool(viper.GetBool(excludeAnnotatedZoneKey)),
				},
			}
			ingesterConfig.FilterConfig = filterConfig
			ingesterConfig.IngestPolicy = &icpb.IngesterConfig_IngestPolicy{
				ContinuousMode: false,
			}
			return nil
		},
	}

	command.PersistentFlags().IntP(
		"min-event-length", "", 10,
		"The minimum duration of a motion event in seconds. The default value is 10 seconds.",
	)
	viper.BindPFlag(eventDurationKey,
		command.PersistentFlags().Lookup("min-event-length"))
	viper.SetDefault(eventDurationKey, 10)

	command.PersistentFlags().IntP(
		"lookback-length", "", 3,
		"The duration of the lookback window before the motion event starts in seconds. The default value is 3 seconds.",
	)
	viper.BindPFlag(lookbackDurationKey,
		command.PersistentFlags().Lookup("lookback-length"))
	viper.SetDefault(lookbackDurationKey, 3)

	command.PersistentFlags().IntP(
		"cooldown-length", "", 300,
		"The cooldown period after a motion event in seconds. The default value is 5 minutes.",
	)
	viper.BindPFlag(cooldownDurationKey,
		command.PersistentFlags().Lookup("cooldown-length"))
	viper.SetDefault(cooldownDurationKey, 300)

	command.PersistentFlags().StringP(
		"motion-sensitivity", "", "medium",
		"The sensitivity of the motion event filtering. Options are high, medium, low. High is more sensitive to motion and more aggressive motion filtering.",
	)
	viper.BindPFlag(motionSensitivityKey,
		command.PersistentFlags().Lookup("motion-sensitivity"))
	viper.SetDefault(motionSensitivityKey, "medium")

	command.PersistentFlags().StringP(
		"zone-annotation", "", "",
		"Zones to be include or exclude from motion detection. A point is an image coordinates. A zone must have three or more points. "+
			"Use : to connect x and y coordinates for each point. Use ; to connect point within same zone. Use - to connect multiple zones. "+
			"For example, input of 1:1;1:2;1:3-2:1;2:2;2:3;2:4 contains two zones. One with (1,1), (1,2), (1,3), and one with (2,1), (2,2), (2,3), (2,4).",
	)
	viper.BindPFlag(zoneAnnotationKey,
		command.PersistentFlags().Lookup("zone-annotation"))
	viper.SetDefault(zoneAnnotationKey, "")

	command.PersistentFlags().BoolP(
		"exclude_annotated_zone", "", false,
		"Set to true to detect motion outside of the annotated zone. Set to false to detect motion inside of the annotated zone. The default is false.",
	)
	viper.BindPFlag(excludeAnnotatedZoneKey,
		command.PersistentFlags().Lookup("exclude_annotated_zone"))
	viper.SetDefault(excludeAnnotatedZoneKey, false)

	return command
}

func newToCmd() *cobra.Command {
	return &cobra.Command{
		Use:           "to",
		Short:         "Select from among possible destinations.",
		Long:          `Select from among possible destinations.`,
		SilenceUsage:  true,
		SilenceErrors: true,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			return common.CallParentPersistentPreRunE(cmd, args)
		},
	}
}

func newStdoutCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "stdout",
		Short: "Send results to stdout.",
		Long:  `Send results to stdout.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			eventWriterConfig := &icpb.EventWriterConfig{
				Name: "LogEventWriter",
			}
			ingesterConfig.EventWriterConfig = eventWriterConfig

			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			if common.Verbose {
				fmt.Printf("%v\n", ingesterConfig)
			}
			return exec.RunIngester(ingesterConfig)
		},
	}
	return command
}

func newEncodedStdoutCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "encoded-stdout",
		Short: "Encoded to H264 and send results to stdout.",
		Long:  `Encoded RawImage packets to H264 stream and send results to stdout.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			eventWriterConfig := &icpb.EventWriterConfig{
				Name: "EncodedStreamLogEventWriter",
			}
			ingesterConfig.EventWriterConfig = eventWriterConfig

			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			if common.Verbose {
				fmt.Printf("%v\n", ingesterConfig)
			}
			return exec.RunIngester(ingesterConfig)
		},
	}
	return command
}

func newStreamsDestCmd() *cobra.Command {
	encodedKey := common.UniquifyViperKey("streamdest", "encoded")
	command := &cobra.Command{
		Use:   "streams",
		Short: "Send to a Vision AI Stream.",
		Long:  `Send to a Vision AI Stream.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			eventWriterConfig := &icpb.EventWriterConfig{
				Name: "StreamsEventWriter",
				Attr: map[string]string{
					"sender_name": viper.GetString("send.senderName"),
					"stream_id":   args[0],
					"encoded":     viper.GetString(encodedKey),
				},
				ClusterSelection: &cspb.ClusterSelection{
					ProjectId:       common.ProjectID,
					LocationId:      common.LocationID,
					ClusterId:       common.ClusterID,
					ServiceEndpoint: common.ServiceEndpoint,
				},
			}
			ingesterConfig.EventWriterConfig = eventWriterConfig

			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			if common.Verbose {
				fmt.Printf("%v\n", ingesterConfig)
			}
			return exec.RunIngester(ingesterConfig)
		},
	}

	command.PersistentFlags().BoolP(
		"encoded", "", false,
		"Whether to encode the rawimages into H264 stream before sending out to stream.",
	)
	viper.BindPFlag(encodedKey,
		command.PersistentFlags().Lookup("encoded"))
	viper.SetDefault(encodedKey, false)

	return command
}

func newLocalFileCmd() *cobra.Command {
	tempVideoPathKey := common.UniquifyViperKey("localfile", "tempVideoPath")

	command := &cobra.Command{
		Use:   "mp4file",
		Short: "Write to a local mp4 file.",
		Long:  `Write to a local mp4 file.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			eventWriterConfig := &icpb.EventWriterConfig{
				Name: "LocalVideoEventWriter",
				Attr: map[string]string{
					"output": viper.GetString(tempVideoPathKey),
				},
			}
			ingesterConfig.EventWriterConfig = eventWriterConfig

			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			if common.Verbose {
				fmt.Printf("%v\n", ingesterConfig)
			}
			return exec.RunIngester(ingesterConfig)
		},
	}

	command.PersistentFlags().StringP(
		"mp4-file-path", "", "/tmp",
		"The output directory for mp4 files",
	)
	viper.BindPFlag(tempVideoPathKey,
		command.PersistentFlags().Lookup("mp4-file-path"))
	viper.SetDefault(tempVideoPathKey, "/tmp")

	return command
}

func newWarehouseDestCmd() *cobra.Command {
	warehouseServerAddress := common.UniquifyViperKey("warehouse", "warehouseServerAddress")
	assetName := common.UniquifyViperKey("warehouse", "assetName")
	tempVideoDir := common.UniquifyViperKey("warehouse", "tempVideoDir")

	command := &cobra.Command{
		Use:   "warehouse",
		Short: "Send to a Vision AI Warehouse.",
		Long:  `Send to a Vision AI Warehouse.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}

			eventWriterConfig := &icpb.EventWriterConfig{
				Name: "WarehouseEventWriter",
				Attr: map[string]string{
					"warehouse_server_address": viper.GetString(warehouseServerAddress),
					"asset_name":               viper.GetString(assetName),
					"temp_video_dir":           viper.GetString(tempVideoDir),
					"h264_mux_only":            "True",
					"h264_only":                "True",
				},
			}
			ingesterConfig.EventWriterConfig = eventWriterConfig

			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			if common.Verbose {
				fmt.Printf("%v\n", ingesterConfig)
			}
			return exec.RunIngester(ingesterConfig)
		},
	}

	command.PersistentFlags().StringP(
		"warehouse-server-address", "", "warehouse-visionai.googleapis.com",
		"The warehouse endpoint to send video data to.",
	)
	viper.BindPFlag(warehouseServerAddress,
		command.PersistentFlags().Lookup("warehouse-server-address"))
	viper.SetDefault(warehouseServerAddress, "warehouse-visionai.googleapis.com")

	command.PersistentFlags().StringP(
		"asset-name", "", "",
		"The existing warehouse asset to be associated with the video data.",
	)
	viper.BindPFlag(assetName,
		command.PersistentFlags().Lookup("asset-name"))
	viper.SetDefault(assetName, "")

	command.PersistentFlags().StringP(
		"temp-video-dir", "", "",
		"The directory to start temp video files which will be sent to warehouse.",
	)
	viper.BindPFlag(tempVideoDir,
		command.PersistentFlags().Lookup("temp-video-dir"))
	viper.SetDefault(tempVideoDir, "")

	return command
}
