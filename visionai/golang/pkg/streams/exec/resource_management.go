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

// CreateCluster creates a cluster.
func CreateCluster(options *CreateClusterOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, false)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "CreateCluster")
	args = append(args, "--cluster_id", options.ClusterID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// ListClusters lists clusters.
func ListClusters(options *ListClustersOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, false)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "ListClusters")
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// DeleteCluster deletes a cluster.
func DeleteCluster(options *DeleteClusterOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, false)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "DeleteCluster")
	args = append(args, "--cluster_id", options.ClusterID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// CreateStream creates a stream.
func CreateStream(options *CreateStreamOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "CreateStream")
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// ListStreams lists streams.
func ListStreams(options *ListStreamsOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "ListStreams")
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// DeleteStream deletes a stream.
func DeleteStream(options *DeleteStreamOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "DeleteStream")
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// ListEvents lists streams.
func ListEvents(options *ListEventsOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "ListEvents")
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// EnableMwhExporter enables media warehouse exporter.
func EnableMwhExporter(options *EnableMwhExporterOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "EnableMwhExporter")
	args = append(args, "--mwh_asset_name", options.AssetName)
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// DisableMwhExporter disables a media warehouse exporter.
func DisableMwhExporter(options *DisableMwhExporterOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "DisableMwhExporter")
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// EnableHlsPlayback enables hls playback.
func EnableHlsPlayback(options *EnableHlsPlaybackOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "EnableHlsPlayback")
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}

// DisableHlsPlayback disables an hls playback.
func DisableHlsPlayback(options *DisableHlsPlaybackOptions) error {
	binary, err := GetStreamsResourceManagerAppPath()
	if err != nil {
		return err
	}
	connArgs := connArgsFromRootOptions(options.RootOptions, true)

	args := []string{binary}
	args = append(args, connArgs...)
	args = append(args, "--op_code", "DisableHlsPlayback")
	args = append(args, "--stream_id", options.StreamID)
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}
