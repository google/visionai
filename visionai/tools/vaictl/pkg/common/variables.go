// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package common

var (
	// Verbose indicates whether the user requested verbose.
	Verbose bool

	// ServiceEndpoint holds the visionai endpoint to connect to.
	// e.g. visionai.googleapis.com.
	ServiceEndpoint string

	// ProjectID holds the user supplied GCP project id.
	ProjectID string

	// LocationID holds the user supplied GCP location id.
	LocationID string

	// ClusterID holds the user supplied Vision AI cluster id.
	ClusterID string

	// Timeout holds the default timeout used for rpc requests.
	Timeout int
)

// TODO: These are workarounds for lack of a proper typefind/autoplug
// mechanism in vaictl. Build this later and remove these crutches.
var (
	// NeedCaptureDecode indicates that the output of the capture
	// must be decoded frames.
	NeedCaptureDecode bool
)
