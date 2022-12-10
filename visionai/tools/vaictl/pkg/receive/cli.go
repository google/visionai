// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package receive

var (
	// ReceiveCmd is the root for the receive command.
	ReceiveCmd = newReceiveCmd()

	streamsCmd          = newStreamsCmd()
	streamsPacketsCmd   = newStreamsPacketsCmd()
	streamsVideoFileCmd = newStreamsVideoFileCmd()
)

func init() {
	ReceiveCmd.AddCommand(streamsCmd)
	streamsCmd.AddCommand(streamsPacketsCmd)
	streamsCmd.AddCommand(streamsVideoFileCmd)
}
