// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package send

import (
	"github.com/spf13/cobra"
	icpb "visionai/proto/ingester_config_go_proto"
)

var (
	ingesterConfig = &icpb.IngesterConfig{
		FilterConfig: &icpb.FilterConfig{
			Name: "NoopFilter",
		},
	}

	// SendCmd is the root for the send command.
	SendCmd = newSendCmd()

	sendStdinCmd   = newStdinCmd()
	sendStdinToCmd = newGenericToCmd()

	sendBytesCmd         = newBytesCmd()
	sendBytesToCmd       = newGenericToCmd()
	sendBytesApplyingCmd = newGenericApplyingCmd()

	sendVideoFileCmd         = newSendVideoFileCmd()
	sendVideoFileToCmd       = newEncodedVideoToCmd()
	sendVideoFileApplyingCmd = newVideoApplyingCmd()

	sendRtspCmd         = newSendRtspCmd()
	sendRtspToCmd       = newEncodedVideoToCmd()
	sendRtspApplyingCmd = newVideoApplyingCmd()
)

func init() {
	SendCmd.AddCommand(sendStdinCmd)
	SendCmd.AddCommand(sendBytesCmd)
	SendCmd.AddCommand(sendVideoFileCmd)
	SendCmd.AddCommand(sendRtspCmd)

	sendStdinCmd.AddCommand(sendStdinToCmd)

	sendBytesCmd.AddCommand(sendBytesToCmd)
	sendBytesCmd.AddCommand(sendBytesApplyingCmd)

	sendVideoFileCmd.AddCommand(sendVideoFileToCmd)

	sendVideoFileCmd.AddCommand(sendVideoFileApplyingCmd)

	sendRtspCmd.AddCommand(sendRtspToCmd)

	sendRtspCmd.AddCommand(sendRtspApplyingCmd)
}

func newGenericApplyingCmd() *cobra.Command {
	applyingCmd := newApplyingCmd()
	simpleSegmenterCmd := newSimpleSegmenterCmd()
	simpleSegmenterToCmd := newGenericToCmd()

	applyingCmd.AddCommand(simpleSegmenterCmd)
	simpleSegmenterCmd.AddCommand(simpleSegmenterToCmd)

	return applyingCmd
}

func newGenericToCmd() *cobra.Command {
	toCmd := newToCmd()
	stdoutCmd := newStdoutCmd()
	streamsDestCmd := newStreamsDestCmd()
	localFileCmd := newLocalFileCmd()
	toCmd.AddCommand(stdoutCmd)
	toCmd.AddCommand(streamsDestCmd)
	toCmd.AddCommand(localFileCmd)

	return toCmd
}

func newVideoApplyingCmd() *cobra.Command {
	applyingCmd := newApplyingCmd()
	motionFilterCmd := newMotionFilterCmd()
	motionFilterToCmd := newDecodedVideoToCmd()

	applyingCmd.AddCommand(motionFilterCmd)
	motionFilterCmd.AddCommand(motionFilterToCmd)

	return applyingCmd
}

func newEncodedVideoToCmd() *cobra.Command {
	toCmd := newToCmd()
	stdoutCmd := newStdoutCmd()
	streamsDestCmd := newStreamsDestCmd()
	localFileCmd := newLocalFileCmd()
	toCmd.AddCommand(stdoutCmd)
	toCmd.AddCommand(streamsDestCmd)
	toCmd.AddCommand(localFileCmd)

	// TODO(b/237348441): Restore this after a distributable ffmpeg is built.
	// warehouseDestCmd := newWarehouseDestCmd()
	// toCmd.AddCommand(warehouseDestCmd)

	return toCmd
}

func newDecodedVideoToCmd() *cobra.Command {
	toCmd := newToCmd()
	stdoutCmd := newStdoutCmd()
	encodedstdoutCmd := newEncodedStdoutCmd()
	streamsDestCmd := newStreamsDestCmd()
	localFileCmd := newLocalFileCmd()
	toCmd.AddCommand(stdoutCmd)
	toCmd.AddCommand(encodedstdoutCmd)
	toCmd.AddCommand(streamsDestCmd)
	toCmd.AddCommand(localFileCmd)

	return toCmd
}
