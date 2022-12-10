// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package exec

// RootOptions contain user options passed from vaictl at the root level.
type RootOptions struct {
	// The address/domain name of the AIS service endpoint.
	ServiceEndpoint string

	// The GCP project ID.
	ProjectID string

	// The GCP location ID.
	LocationID string

	// The cluster name.
	//
	// This will be used for all operations under a specific cluster.
	// Ignored for cluster management.
	ClusterID string

	// The timeout (in seconds) used for requests.
	Timeout int

	// The number of retries for requests.
	Retries int

	// Verbose output.
	Verbose bool
}

// CreateClusterOptions contain configurations for cluster creation.
type CreateClusterOptions struct {
	// The cluster id.
	ClusterID string

	// Root options.
	RootOptions *RootOptions
}

// ListClustersOptions contain configurations for listing clusters.
type ListClustersOptions struct {
	// Root options.
	RootOptions *RootOptions
}

// DeleteClusterOptions contain configurations for cluster deletion.
type DeleteClusterOptions struct {
	// The cluster id.
	ClusterID string

	// Root options.
	RootOptions *RootOptions
}

// CreateStreamOptions contain configurations for stream creation.
type CreateStreamOptions struct {
	// The stream id.
	StreamID string

	// Root options.
	RootOptions *RootOptions
}

// ListStreamsOptions contain configurations for listing streams.
type ListStreamsOptions struct {
	// Root options.
	RootOptions *RootOptions
}

// DeleteStreamOptions contain configurations for deleting streams.
type DeleteStreamOptions struct {
	// The stream id.
	StreamID string

	// Root options.
	RootOptions *RootOptions
}

// ListEventsOptions contain configurations for listing events.
type ListEventsOptions struct {
	// Root options.
	RootOptions *RootOptions
}

// ReceiveOptions contains options available to all receive subcommands.
type ReceiveOptions struct {
	// Name to identify the receiver.
	ReceiverName string

	// The specific envet id to receive from.
	//
	// If empty, then the latest event will be read. Furthermore,
	// when a newer event arrives, then the receiver will also
	// automatically transition to that latest event.
	EventID string

	// Root options.
	RootOptions *RootOptions
}

// ReceiveCatOptions contains options for the receive cat app.
type ReceiveCatOptions struct {
	// The stream id to receive from.
	StreamID string

	// Receive options.
	ReceiveOptions *ReceiveOptions
}

// VisualizationAppOptions contains options for the visualization app.
type VisualizationAppOptions struct {
	// ID of the video stream.
	VideoStreamID string

	// ID of the annotation stream.
	AnnotationStreamID string

	// Print just a short summary about the received packets.
	SummaryOnly bool

	// If the package is a protobuf, try decode it against known types and print.
	TryDecodeProtobuf bool

	// Should use same fps for both video and annotation.
	SameFps bool

	// The full path and filename of the annotated video, supports only .avi format. If it is empty, no video file will be generated.
	OutputVideoFilePath string

	// Receive options.
	ReceiveOptions *ReceiveOptions
}

// EnableMwhExporterOptions contains options for enabling stream mwh export.
type EnableMwhExporterOptions struct {
	// The full media warehouse asset resource name into which to export.
	AssetName string

	// The stream id to enable exports for.
	StreamID string

	// Root options.
	RootOptions *RootOptions
}

// DisableMwhExporterOptions contains options for disabling stream mwh export.
type DisableMwhExporterOptions struct {
	// The stream id to disable exports for.
	StreamID string

	// Root options.
	RootOptions *RootOptions
}

// EnableHlsPlaybackOptions contains options for enabling stream hls playback.
type EnableHlsPlaybackOptions struct {
	// The stream id to enable hls playback for.
	StreamID string

	// Root options.
	RootOptions *RootOptions
}

// DisableHlsPlaybackOptions contains options for disabling stream hls playback.
type DisableHlsPlaybackOptions struct {
	// The stream id to disabling hls playback for.
	StreamID string

	// Root options.
	RootOptions *RootOptions
}
