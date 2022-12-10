// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"os"
	"path/filepath"
	"regexp"
	"strings"
)

func changelogPath() (string, error) {
	binPath, err := os.Executable()
	if err != nil {
		return "", err
	}
	binary := filepath.Join(
		filepath.Dir(binPath),
		"vaictl.runfiles",
		"visionai",
		"debian",
		"changelog",
	)
	return binary, nil
}

// Version returns the vaictl verion.
func Version() string {
	changelogPath, err := changelogPath()
	if err != nil {
		return ""
	}
	content, err := os.ReadFile(changelogPath)
	if err != nil {
		return ""
	}

	re := regexp.MustCompile(`\((.*?)\)`)
	version := re.FindAllString(string(content), -1)[0]
	version = strings.Trim(version, "(")
	version = strings.Trim(version, ")")
	return version
}
