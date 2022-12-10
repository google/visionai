// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package codegen

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// Context contains data relevant to a single codegen invocation.
type Context struct {
	// Name of the backend.
	//
	// e.g. "k8s_one", "graphviz"
	BackendName string

	// Verbose output.
	Verbose bool

	// Add debug capability to the generated code.
	EnableDebug bool

	// The ASG instance from which to codegen.
	//
	// This should be the final ASG after all other compiler phases
	// and is expected to be syntactically and semantically correct.
	//
	// Codegen backends should not modify this.
	Asg *asg.Graph

	// The string containing the compiled output.
	OutputString string

	// Runtime information.
	RuntimeInfo *asg.RuntimeInfo
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
