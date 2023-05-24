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

// MakeAnalysisName assembles the resource name from the given project-id, location-id, cluster-id, and analysis-id.
func MakeAnalysisName(projectID, locationID, clusterID, analysisID string) (string, error) {
	if err := util.IsValidResourceID(analysisID); err != nil {
		return "", fmt.Errorf("the given analysis-id had errors: %v", err)
	}
	clusterName, err := util.MakeClusterName(projectID, locationID, clusterID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/analyses/%v", clusterName, analysisID), nil
}

// MakeProcessName assembles the resource name from the given project-id, location-id, cluster-id, and process-id.
func MakeProcessName(projectID, locationID, clusterID, processID string) (string, error) {
	if err := util.IsValidResourceID(processID); err != nil {
		return "", fmt.Errorf("the given process-id had errors: %v", err)
	}
	clusterName, err := util.MakeClusterName(projectID, locationID, clusterID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/processes/%v", clusterName, processID), nil
}
