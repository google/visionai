// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// FeatureOptions contain user requested features that
// will influence the generated code.
type FeatureOptions struct {
	// The mode in which to run the analysis.
	RunMode string

	// Add debug capability to the generated code.
	EnableDebug bool

	// Runtime information.
	//
	// TODO(b/271031113) This is actually a monitoring specific
	// option. Probably best to call this something else, rather
	// than having it appear like an IR concept (`asg`).
	RuntimeInfo *asg.RuntimeInfo
}

// Context contains data relevant to a single Graphviz codegen invocation.
type Context struct {
	// Verbose output.
	Verbose bool

	// User configured features.
	FeatureOptions FeatureOptions

	// The ASG instance from which to codegen.
	Asg *asg.Graph

	// The serialized K8sOne protobuf.
	OutputString string

	// Codegen outputs.
	inputPlaceholderNames  []string
	outputPlaceholderNames []string
	yamls                  []string
}

func (c *Context) verifyInitialization() error {
	if c.Asg == nil {
		return fmt.Errorf("no ASG given")
	}
	if c.FeatureOptions.RuntimeInfo == nil {
		return fmt.Errorf("no RuntimeInfo given")
	}
	return nil
}
