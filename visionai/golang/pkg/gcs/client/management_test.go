// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package gcsclient

import (
	"testing"

	"google3/base/go/log"

	"google3/third_party/golang/fakegcsserver/fakestorage/fakestorage"
	"google3/third_party/visionai/services/operators/pkg/util/util"
)

func mustCreateManager() Interface {
	server := fakestorage.NewServer([]fakestorage.Object{
		{
			ObjectAttrs: fakestorage.ObjectAttrs{
				BucketName: "test-bucket",
				Name:       "test-objects1/test-video1.mp4",
			},
			Content: []byte("Content"),
		},
		{
			ObjectAttrs: fakestorage.ObjectAttrs{
				BucketName: "test-bucket",
				Name:       "test-objects1/test-video2.mp4",
			},
			Content: []byte("Content"),
		},
		{
			ObjectAttrs: fakestorage.ObjectAttrs{
				BucketName: "test-bucket",
				Name:       "test-objects1/test-video3.mp4",
			},
			Content: []byte("Content"),
		},
		{
			ObjectAttrs: fakestorage.ObjectAttrs{
				BucketName: "test-bucket",
				Name:       "test-objects2/test-video1.mp4",
			},
			Content: []byte("Content"),
		},
		{
			ObjectAttrs: fakestorage.ObjectAttrs{
				BucketName: "test-bucket",
				Name:       "test-objects2/test-video2.mp4",
			},
			Content: []byte("Content"),
		},
	})
	client := server.Client()
	manager, err := NewTestManager(client)
	if err != nil {
		log.Fatal(err)
	}
	return manager
}

func TestGcsListObjectsWithNotExistsBucket(t *testing.T) {
	manager := mustCreateManager()
	_, err := manager.ListObjects("test-bucket1", "test-objects1/")
	if err == nil {
		t.Fatal("ListObjects succeeded, want error")
	}
}

func TestGcsListObjects(t *testing.T) {
	manager := mustCreateManager()
	files, err := manager.ListObjects("test-bucket", "test-objects1/")
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}
	expectedFiles := []string{
		"gs://test-bucket/test-objects1/test-video1.mp4",
		"gs://test-bucket/test-objects1/test-video2.mp4",
		"gs://test-bucket/test-objects1/test-video3.mp4",
	}
	unexpectedFiles := []string{
		"gs://test-bucket/test-objects2/test-video1.mp4",
		"gs://test-bucket/test-objects2/test-video2.mp4",
	}
	if len(files) != len(expectedFiles) {
		t.Errorf("Expected %d files, got %d", len(expectedFiles), len(files))
	}
	for _, f := range files {
		if !util.StringSliceContains(expectedFiles, f) {
			t.Errorf("Expected files contains %s, got false", f)
		}
		if util.StringSliceContains(unexpectedFiles, f) {
			t.Errorf("Unexpected files contains %s, got false", f)
		}
	}
}
