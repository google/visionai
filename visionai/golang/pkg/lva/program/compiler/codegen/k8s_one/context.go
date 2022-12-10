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

// Context contains data relevant to a single Graphviz codegen invocation.
type Context struct {
	// Verbose output.
	Verbose bool

	// Add debug capability to the generated code.
	EnableDebug bool

	// The ASG instance from which to codegen.
	Asg *asg.Graph

	// Associated runtime information
	RuntimeInfo *asg.RuntimeInfo

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
	if c.RuntimeInfo == nil {
		return fmt.Errorf("no RuntimeInfo given")
	}
	return nil
}
