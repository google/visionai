// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package exec

import (
	"io/ioutil"
	"os"
	"syscall"

	"github.com/golang/protobuf/proto"
	icpb "visionai/proto/ingester_config_go_proto"
)

// RunIngester runs the ingester.
func RunIngester(config *icpb.IngesterConfig) error {
	// Exits immediately if a source file doesn't exit when ingesting the source files.
	captureConfigName := config.GetCaptureConfig().GetName()
	if captureConfigName == "FileSourceCapture" || captureConfigName == "FileSourceImageCapture" {
		for _, sourceURL := range config.GetCaptureConfig().GetSourceUrls() {
			if _, err := os.Stat(sourceURL); err != nil {
				return err
			}
		}
	}

	binary, err := GetIngesterAppPath()
	if err != nil {
		return err
	}

	// Write the given ingester config into a temporary file.
	tmpfile, err := ioutil.TempFile("", "ingester_config.*.pb")
	if err != nil {
		return err
	}
	defer os.Remove(tmpfile.Name())
	out, err := proto.Marshal(config)
	if err != nil {
		return err
	}
	if err := ioutil.WriteFile(tmpfile.Name(), out, 0644); err != nil {
		return err
	}
	if err := tmpfile.Close(); err != nil {
		return err
	}

	args := []string{binary}
	args = append(args, tmpfile.Name())
	env := os.Environ()
	return syscall.Exec(binary, args, env)
}
