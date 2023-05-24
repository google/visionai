// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package exec

import (
	"os"
	"strconv"
	"syscall"
)

// VisualizationApp runs the oc_visualization_app.
func VisualizationApp(options *VisualizationAppOptions) error {
	binary, err := GetVisualizationAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.ReceiveOptions.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	if options.ReceiveOptions.EventID != "" {
		args = append(args, "--event_id", options.ReceiveOptions.EventID)
	}
	if options.ReceiveOptions.ReceiverName != "" {
		args = append(args, "--receiver_id", options.ReceiveOptions.ReceiverName)
	}
	args = append(args, "--v_stream_id", options.VideoStreamID)
	args = append(args, "--a_stream_id", options.AnnotationStreamID)

	// Boolean flags are required to use "--key=value" format, where "value" takes either "true" or "false".
	// If we use "--key value", the actual value of the flag would always be considered true.
	args = append(args, "--summary_only="+strconv.FormatBool(options.SummaryOnly))
	args = append(args, "--try_decode_protobuf="+strconv.FormatBool(options.TryDecodeProtobuf))
	args = append(args, "--same_fps="+strconv.FormatBool(options.SameFps))
	args = append(args, "--no_display="+strconv.FormatBool(options.NoDisplay))
	if options.OutputVideoFilePath != "" {
		args = append(args, "--output_video_filepath", options.OutputVideoFilePath)
	}
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}
