// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Package util provides the gcs utilities.
package util

import (
	"fmt"
	"regexp"
	"strings"

	"google3/util/task/go/status"
)

var (
	gcsFolderReg = regexp.MustCompile(`^gs://(?P<bucket>[^/]+)/(?P<object>.+/)?$`)
)

// GcsBucketAndFolder accepts a gcsFolder input and return the bucket and folder
//
// Example 1:
// gcsFolder: "gs://bucket/folder/"
// bucket:    "bucket"
// folder:    "folder/"
//
// Example 2:
// gcsFolder: "gs://bucket/"
// bucket:    "bucket"
// folder:     ""
func GcsBucketAndFolder(gcsFolder string) (bucket string, folder string, err error) {
	// Check the GCS folder naming format.
	matches := gcsFolderReg.FindStringSubmatch(gcsFolder)
	if len(matches) != 3 {
		return "", "", fmt.Errorf("invalid GCS folder: %w, want gs://bucket/ or gs://bucket/folder/ format", status.ErrInvalidArgument)
	}
	return matches[1], matches[2], nil
}

// GcsOutputFilePath is to compose the gcs file path of the output file from the input file path
// and is based on the source and sink operator
//
// Example 1:
// sourceOperator: "GcsVideoSource"
// sinkOperator: "GcsVideoSink"
// gcsInputFilePath: "gs://bucket/folder/video.mp4"
// processID: "process"
// gcsOutputFilePath: "gs://bucket/folder/video-process-output.mp4"
//
// Example 2:
// sourceOperator: "GcsVideoSource"
// sinkOperator: "GcsProtoSink"
// gcsInputFilePath: "gs://bucket/folder/video.mp4"
// processID: "process"
// gcsOutputFilePath: "gs://bucket/folder/video-process-output.json"
func GcsOutputFilePath(gcsInputFilePath, processID, sourceOperator, sinkOperator string) (string, error) {
	var gcsOutputFilePath string

	switch sourceOperator {
	case "GcsVideoSource":
		gcsOutputFilePath = strings.TrimSuffix(gcsInputFilePath, MP4Ext)
	default:
		return "", fmt.Errorf("the source operator: %s, is not supported in batch-process", sourceOperator)
	}

	gcsOutputFilePath = gcsOutputFilePath + "-" + processID + "-output"

	switch sinkOperator {
	case "GcsVideoSink":
		gcsOutputFilePath = gcsOutputFilePath + MP4Ext
	case "GcsProtoSink":
		gcsOutputFilePath = gcsOutputFilePath + JSONExt
	default:
		return "", fmt.Errorf("the sink operator: %s, is not supported in batch-process", sinkOperator)
	}

	return gcsOutputFilePath, nil
}
