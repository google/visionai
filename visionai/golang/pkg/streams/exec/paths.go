// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package exec

import (
	"os"
	"path/filepath"
)

// GetVaictlDir returns the directory in which the vaictl binary is located.
//
// The absolute path is returned.
func GetVaictlDir() (string, error) {
	binPath, err := os.Executable()
	if err != nil {
		return "", err
	}
	return filepath.Dir(binPath), nil
}

// GetVaictlStreamsAppDir returns the directory where streams application binaries are located at runtime.
//
// The absolute path is returned.
func GetVaictlStreamsAppDir() (string, error) {
	binDir, err := GetVaictlDir()
	if err != nil {
		return "", err
	}
	binary := filepath.Join(
		binDir,
		"vaictl.runfiles",
		"visionai",
		"visionai",
		"streams",
		"apps",
	)
	return binary, nil
}

// GetStreamsResourceManagerAppPath returns the path to the streams resource manager app binary at runtime.
//
// The absolute path is returned.
func GetStreamsResourceManagerAppPath() (string, error) {
	binDir, err := GetVaictlStreamsAppDir()
	if err != nil {
		return "", err
	}
	binary := filepath.Join(
		binDir,
		"resource_manager_app",
	)
	return binary, nil
}

// GetIngesterAppPath returns the path to the ingester app binary at runtime.
//
// The absolute path is returned.
func GetIngesterAppPath() (string, error) {
	binDir, err := GetVaictlStreamsAppDir()
	if err != nil {
		return "", err
	}
	binary := filepath.Join(
		binDir,
		"ingester_app",
	)
	return binary, nil
}

// GetReceiveCatAppPath returns the path to the receive cat app binary at runtime.
//
// The absolute path is returned.
func GetReceiveCatAppPath() (string, error) {
	binDir, err := GetVaictlStreamsAppDir()
	if err != nil {
		return "", err
	}
	binary := filepath.Join(
		binDir,
		"receive_cat_app",
	)
	return binary, nil
}

// GetVisualizationAppPath returns the path to the oc_visualization_app binary at runtime.
//
// The absolute path is returned.
func GetVisualizationAppPath() (string, error) {
	binDir, err := GetVaictlStreamsAppDir()
	if err != nil {
		return "", err
	}
	binary := filepath.Join(
		binDir,
		"visualization",
		"oc_visualization_app",
	)
	return binary, nil
}
