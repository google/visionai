// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Package gcsclient supplies a programmatic client to the gcs.
package gcsclient

import (
	"context"
	"fmt"
	"path/filepath"

	"google3/third_party/golang/cloud_google_com/go/storage/v/v1/storage"
	"google3/third_party/golang/google_api/iterator/iterator"
	"google3/third_party/visionai/golang/pkg/gcs/util/util"
)

// Interface defines the GCS management interface.
type Interface interface {
	ListObjects(bucketName, prefix string) ([]string, error)
}

// NewManager creates a GCS manager.
func NewManager() (Interface, error) {
	gcsClient, err := storage.NewClient(context.Background())
	if err != nil {
		return nil, err
	}
	return &manager{
		client: gcsClient,
	}, nil
}

// NewTestManager creates a test GCS manager for testing only.
func NewTestManager(gcsClient *storage.Client) (Interface, error) {
	return &manager{
		client: gcsClient,
	}, nil
}

type manager struct {
	client *storage.Client
}

// ListObjects returns a list of object filePaths in format `gs://bucketName/objectName`
// for all the GCS objects in a bucket with object IDs starting with `prefix`
func (m *manager) ListObjects(bucketName, prefix string) ([]string, error) {
	// References:
	// - https://pkg.go.dev/cloud.google.com/go/storage#Query
	// - https://cloud.google.com/storage/docs/listing-objects#storage-list-objects-go
	//
	// Prefixes and delimiters can be used to emulate directory listings.
	// Prefixes can be used to filter objects starting with the prefix.
	// The delimiter argument can be used to restrict the results to only the
	// objects in the given "directory". Without the delimiter, the entire tree
	// under the prefix is returned.
	it := m.client.Bucket(bucketName).Objects(context.Background(), &storage.Query{
		Prefix:    prefix,
		Delimiter: util.GcsDelimiter,
	})
	var filePaths []string
	for {
		object, err := it.Next()
		if err == iterator.Done {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("Could not iterate through objects in bucket %q: %v", bucketName, err)
		}
		filePaths = append(filePaths, util.GcsPrefix+filepath.Join(object.Bucket, object.Name))
	}
	return filePaths, nil
}
