// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package exec

import (
	"os"
	"syscall"
)

// ReceiveCat runs the receive_cat_app.
func ReceiveCat(options *ReceiveCatOptions) error {
	binary, err := GetReceiveCatAppPath()
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
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}
