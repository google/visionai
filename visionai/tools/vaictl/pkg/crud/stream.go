// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"visionai/golang/pkg/streams/exec"
	"visionai/tools/vaictl/pkg/common"
)

func newCreateStreamCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "stream STREAM_NAME",
		Short: "Create a stream.",
		Long:  `Create a stream.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			createOptions := &exec.CreateStreamOptions{
				StreamID:    args[0],
				RootOptions: newRootOptions(),
			}
			return exec.CreateStream(createOptions)
		},
	}
	return command
}

func newListStreamsCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "streams",
		Short: "List streams.",
		Long:  `List streams.`,
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			listOptions := &exec.ListStreamsOptions{
				RootOptions: newRootOptions(),
			}
			return exec.ListStreams(listOptions)
		},
	}
	return command
}

func newDeleteStreamCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "stream STREAM_NAME",
		Short: "Delete a stream.",
		Long:  `Delete a stream.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			deleteOptions := &exec.DeleteStreamOptions{
				StreamID:    args[0],
				RootOptions: newRootOptions(),
			}
			return exec.DeleteStream(deleteOptions)
		},
	}
	return command
}

func newEnableStreamMwhExportCmd() *cobra.Command {
	destAssetKey := common.UniquifyViperKey("streamMwhExport", "destAsset")
	command := &cobra.Command{
		Use:   "stream-mwh-export --dest-asset=ASSET_NAME STREAM_ID",
		Short: "Enable a stream to automatically export to media warehouse.",
		Long: `Enable a stream to automatically export to media warehouse.

Streams with this feature enabled will automatically export its data
into the media warehouse. Specifically, it will take data from the
given stream-id, and save it into the given asset.`,
		Args: cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			enableOptions := &exec.EnableMwhExporterOptions{
				AssetName:   viper.GetString(destAssetKey),
				StreamID:    args[0],
				RootOptions: newRootOptions(),
			}
			return exec.EnableMwhExporter(enableOptions)
		},
	}

	command.Flags().StringP(
		"dest-asset", "", "",
		"The full API resource name of the media warehouse asset to export into.",
	)
	viper.BindPFlag(destAssetKey,
		command.Flags().Lookup("dest-asset"))
	viper.SetDefault(destAssetKey, "")

	return command
}

func newDisableStreamMwhExportCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "stream-mwh-export STREAM_ID",
		Short: "Disable a stream from automatically exporting to media warehouse.",
		Long:  `Disable a stream from automatically exporting to media warehouse.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			disableOptions := &exec.DisableMwhExporterOptions{
				StreamID:    args[0],
				RootOptions: newRootOptions(),
			}
			return exec.DisableMwhExporter(disableOptions)
		},
	}
	return command
}

func newEnableStreamHlsPlaybackCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "stream-hls-playback STREAM_ID",
		Short: "Commission an HLS endpoint to publish the (video) stream contents.",
		Long:  `Commission an HLS endpoint to publish the (video) stream contents.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			enableOptions := &exec.EnableHlsPlaybackOptions{
				StreamID:    args[0],
				RootOptions: newRootOptions(),
			}
			return exec.EnableHlsPlayback(enableOptions)
		},
	}
	return command
}

func newDisableStreamHlsPlaybackCmd() *cobra.Command {
	command := &cobra.Command{
		Use:   "stream-hls-playback STREAM_ID",
		Short: "Decommission the HLS endpoint for a video stream.",
		Long:  `Decommission the HLS endpoint for a video stream.`,
		Args:  cobra.ExactArgs(1),
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := common.CallParentPersistentPreRunE(cmd, args); err != nil {
				return err
			}
			return nil
		},
		RunE: func(cmd *cobra.Command, args []string) error {
			disableOptions := &exec.DisableHlsPlaybackOptions{
				StreamID:    args[0],
				RootOptions: newRootOptions(),
			}
			return exec.DisableHlsPlayback(disableOptions)
		},
	}
	return command
}
