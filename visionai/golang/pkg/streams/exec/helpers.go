// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package exec

func connArgsFromRootOptions(options *RootOptions, insertClusterID bool) []string {
	args := []string{}
	args = append(args, "--project_id", options.ProjectID)
	args = append(args, "--location_id", options.LocationID)
	if insertClusterID {
		args = append(args, "--cluster_id", options.ClusterID)
	}
	args = append(args, "--service_endpoint", options.ServiceEndpoint)
	return args
}
