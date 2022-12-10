// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"fmt"
	"regexp"
)

var (
	resourceIDRegex = "^[a-z]([a-z0-9-]{0,61}[a-z0-9])?$"
	validResourceID = regexp.MustCompile(resourceIDRegex)
)

// IsValidResourceID checks whether the given resource id conforms to those expected of AIP.
func IsValidResourceID(resourceID string) error {
	if !validResourceID.MatchString(resourceID) {
		return fmt.Errorf("given a resource id (%q) in the wrong format; it must match %q", resourceID, resourceIDRegex)
	}
	return nil
}

// MakeProjectName assembles the resource name from the given project-id.
func MakeProjectName(projectID string) (string, error) {
	if err := IsValidResourceID(projectID); err != nil {
		return "", fmt.Errorf("the given project-id had errors: %v", err)
	}
	return fmt.Sprintf("projects/%s", projectID), nil
}

// MakeProjectLocationName assembles the resource name from the given project-id and location-id.
func MakeProjectLocationName(projectID, locationID string) (string, error) {
	if err := IsValidResourceID(locationID); err != nil {
		return "", fmt.Errorf("the given location-id had errors: %v", err)
	}
	projectName, err := MakeProjectName(projectID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/locations/%v", projectName, locationID), nil
}

// MakeClusterName assembles the resource name from the given project-id, location-id, and cluster-id.
func MakeClusterName(projectID, locationID, clusterID string) (string, error) {
	if err := IsValidResourceID(clusterID); err != nil {
		return "", fmt.Errorf("the given cluster-id had errors: %v", err)
	}
	projectLocationName, err := MakeProjectLocationName(projectID, locationID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/clusters/%v", projectLocationName, clusterID), nil
}
