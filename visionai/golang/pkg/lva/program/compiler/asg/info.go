// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

// --------------------------------------------------------------
// Information about StreamElements.

// StreamInfo contains all information regarding a StreamElement.
type StreamInfo struct {
	Name string
	Type string
	// The number of downstream analyzers. The upstream analyzer is always 1.
	DownstreamAnalyzers int
}

// --------------------------------------------------------------
// Information about AnalyzerElements.

// AnalyzerInfo contains all information regarding an AnalyzerElement.
type AnalyzerInfo struct {
	Name          string
	Operator      *operators.OperatorInfo
	InputStreams  []*InputStreamInfo
	OutputStreams []*OutputStreamInfo
	Attributes    map[string]*AttributeValueInfo
	Resources     *ResourceInfo

	// TODO(b/271186106): These are really code generator options and
	// not IR level constructs. Probably best to refactor this.
	MonitoringInfo *MonitoringInfo
	RuntimeInfo    *RuntimeInfo
}

// InputStreamInfo contains information about on one input stream
// at an AnalyzerElement.
type InputStreamInfo struct {
	Stream *StreamInfo
}

// OutputStreamInfo contains information about on one input stream
// at an AnalyzerElement.
type OutputStreamInfo struct {
	Stream *StreamInfo
}

// AttributeValueInfo contains information about a concrete
// value of an attribute.
type AttributeValueInfo struct {
	Type  string
	Value interface{}
}

// ResourceInfo contains information about resources.
type ResourceInfo struct {
	Cpu             string
	CpuLimits       string
	Memory          string
	MemoryLimits    string
	Gpus            int
	Envvars         map[string]string
	LatencyBudgetMs int
}

// --------------------------------------------------------------
// Information about SentinelElements.

// SentinelInfo contains all information regarding a SentinelElement.
type SentinelInfo struct {
	Name            string
	InputNodeNames  []string
	OutputNodeNames []string
}

// RuntimeInfo contains runtime information associated to the analysis
// being compiled.
type RuntimeInfo struct {
	// The full resource name of the analysis being compiled
	AnalysisName string

	// Whether to include environment variables in the analyzer deployment
	IncludeEnv bool
}

// --------------------------------------------------------------
// Information about analyzer monitoring.

// MonitoringLevel represents the level of monitoring for a component
type MonitoringLevel string

const (
	// NoMonitoring indicates there should be no monitoring
	NoMonitoring MonitoringLevel = "NO_MONITORING"

	// InternalMonitoring indicates there should only be internal monitoring
	InternalMonitoring MonitoringLevel = "INTERNAL_MONITORING"

	// FullMonitoring indicates there should be both internal and external monitoring
	FullMonitoring MonitoringLevel = "FULL_MONITORING"
)

// MonitoringInfo contains all monitoring-related information for an analyzer.
type MonitoringInfo struct {
	Level       MonitoringLevel
	MetricsPort int
}
