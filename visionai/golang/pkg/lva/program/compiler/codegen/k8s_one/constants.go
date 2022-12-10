// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"path/filepath"
)

const (
	analysisNameTemplateString         = "{{.AnalysisName}}"
	namespaceTemplateString            = "{{.Namespace}}"
	seriesServerEndpointTemplateString = "{{.SeriesServerEndpoint}}"
	dockerRegistryTemplateString       = "{{.DockerRegistry}}"
	dockerImageTagTemplateString       = "{{.DockerImageTag}}"
	inputPlaceholderTemplateString     = "{{index .InputSeries %d}}"
	outputPlaceholderTemplateString    = "{{index .OutputSeries %d}}"
	lesseeTemplateString               = "{{.Lessee}}"
	projectIDTemplateString            = "{{.ProjectID}}"
	locationIDTemplateString           = "{{.LocationID}}"
	clusterIDTemplateString            = "{{.ClusterID}}"
	inputStreamIDTemplateString        = "{{index .InputStream %d}}"
	outputStreamIDTemplateString       = "{{index .OutputStream %d}}"
	eventIDTemplateString              = "{{.EventID}}"
	analyzerSuffixTemplateString       = "{{.AnalyzerSuffix}}"

	mainContainerName = "main"
	mainBinaryName    = "main"
	opDeclFileName    = "op_decl.pbtxt"

	containerWorkdir = "/google"
	configVolumeName = "config"

	internalGrpcOutputPort = 16000
	defaultLatencyBudgetMs = 10000
)

var (
	containerConfigPath = filepath.Join(containerWorkdir, "config")
)
