// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"fmt"

	"visionai/golang/pkg/util"
)

// MakeStreamName assembles the resource name from the given project-id, location-id, cluster-id, and stream-id.
func MakeStreamName(projectID, locationID, clusterID, streamID string) (string, error) {
	if err := util.IsValidResourceID(streamID); err != nil {
		return "", fmt.Errorf("the given stream-id had errors: %v", err)
	}
	clusterName, err := util.MakeClusterName(projectID, locationID, clusterID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/streams/%v", clusterName, streamID), nil
}
