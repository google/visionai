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

func isValidResourceID(resourceID string) error {
	if resourceID == "" {
		return fmt.Errorf("given an empty resource-id")
	}
	return nil
}

// MakeProjectName assembles the project name from the given project-id.
func MakeProjectName(projectID string) (string, error) {
	if err := isValidResourceID(projectID); err != nil {
		return "", fmt.Errorf("the given project-id had errors: %v", err)
	}
	return fmt.Sprintf("projects/%s", projectID), nil
}

// MakeProjectLocationName assembles the project-location name from the given project-id and location-id.
func MakeProjectLocationName(projectID, locationID string) (string, error) {
	if err := util.IsValidResourceID(locationID); err != nil {
		return "", fmt.Errorf("the given location-id had errors: %v", err)
	}
	projectName, err := MakeProjectName(projectID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/locations/%v", projectName, locationID), nil
}

// MakeCorpusName assembles a corpus name from the given project-id, location-id, and corpus-id.
func MakeCorpusName(projectID, locationID, corpusID string) (string, error) {
	if err := isValidResourceID(corpusID); err != nil {
		return "", fmt.Errorf("the given corpus-id had errors: %v", err)
	}
	projectLocationName, err := MakeProjectLocationName(projectID, locationID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/corpora/%v", projectLocationName, corpusID), nil
}

// MakeAssetName assembles an asset name from the given project-id, location-id, corpus-id, and asset-id.
func MakeAssetName(projectID, locationID, corpusID, assetID string) (string, error) {
	if err := isValidResourceID(assetID); err != nil {
		return "", fmt.Errorf("the given asset-id had errors: %v", err)
	}
	corpusName, err := MakeCorpusName(projectID, locationID, corpusID)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%v/assets/%v", corpusName, assetID), nil
}
