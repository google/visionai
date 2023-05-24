// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"testing"
)

func TestGcsBucketAndFolder(t *testing.T) {
	testCases := []struct {
		name       string
		gcsFolder  string
		wantBucket string
		wantFolder string
		wantErr    bool
	}{
		{
			name:      "Non gcs prefix",
			gcsFolder: "bucket/folder/",
			wantErr:   true,
		},
		{
			name:      "Non delimiter suffix 1",
			gcsFolder: "gs://bucket",
			wantErr:   true,
		},
		{
			name:      "Non delimiter suffix 2",
			gcsFolder: "gs://bucket/folder",
			wantErr:   true,
		},
		{
			name:       "Empty folder",
			gcsFolder:  "gs://bucket/",
			wantBucket: "bucket",
			wantFolder: "",
		},
		{
			name:       "Full folder",
			gcsFolder:  "gs://bucket/folder/",
			wantBucket: "bucket",
			wantFolder: "folder/",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			bucket, folder, err := GcsBucketAndFolder(tc.gcsFolder)
			if gotErr := err != nil; gotErr != tc.wantErr {
				t.Errorf("GcsBucketAndFolder return gotErr %v, wantErr %v", gotErr, tc.wantErr)
			}
			if bucket != tc.wantBucket {
				t.Errorf("GcsBucketAndFolder() = %+v, want %+v", bucket, tc.wantBucket)
			}
			if folder != tc.wantFolder {
				t.Errorf("GcsBucketAndFolder() = %+v, want %+v", folder, tc.wantFolder)
			}
		})
	}
}

func TestGcsOutputFilePath(t *testing.T) {
	testCases := []struct {
		name                  string
		gcsInputFilePath      string
		processID             string
		sourceOperator        string
		sinkOperator          string
		wantGcsOutputFilePath string
		wantErr               bool
	}{
		{
			name:             "Unsupported source operator",
			gcsInputFilePath: "gs://bucket/folder/video.mp4",
			processID:        "process",
			sourceOperator:   "DeID",
			sinkOperator:     "GcsVideoSink",
			wantErr:          true,
		},
		{
			name:             "Unsupported sink operator",
			gcsInputFilePath: "gs://bucket/folder/video.mp4",
			processID:        "process",
			sourceOperator:   "GcsVideoSource",
			sinkOperator:     "DeID",
			wantErr:          true,
		},
		{
			name:                  "Gcs video sink operator",
			gcsInputFilePath:      "gs://bucket/folder/video.mp4",
			processID:             "process",
			sourceOperator:        "GcsVideoSource",
			sinkOperator:          "GcsVideoSink",
			wantGcsOutputFilePath: "gs://bucket/folder/video-process-output.mp4",
		},
		{
			name:                  "Gcs proto sink operator",
			gcsInputFilePath:      "gs://bucket/folder/video.mp4",
			processID:             "process",
			sourceOperator:        "GcsVideoSource",
			sinkOperator:          "GcsProtoSink",
			wantGcsOutputFilePath: "gs://bucket/folder/video-process-output.json",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			gcsOutputFilePath, err := GcsOutputFilePath(tc.gcsInputFilePath, tc.processID, tc.sourceOperator, tc.sinkOperator)
			if gotErr := err != nil; gotErr != tc.wantErr {
				t.Errorf("GcsOutputFilePath return gotErr %v, wantErr %v", gotErr, tc.wantErr)
			}
			if gcsOutputFilePath != tc.wantGcsOutputFilePath {
				t.Errorf("GcsOutputFilePath() = %+v, want %+v", gcsOutputFilePath, tc.wantGcsOutputFilePath)
			}
		})
	}
}
