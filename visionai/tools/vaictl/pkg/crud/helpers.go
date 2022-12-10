// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"visionai/golang/pkg/streams/exec"
	"visionai/tools/vaictl/pkg/common"
)

func newRootOptions() *exec.RootOptions {
	return &exec.RootOptions{
		ServiceEndpoint: common.ServiceEndpoint,
		ProjectID:       common.ProjectID,
		LocationID:      common.LocationID,
		ClusterID:       common.ClusterID,
	}
}
