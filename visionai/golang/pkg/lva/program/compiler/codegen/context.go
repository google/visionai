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

// Options contains settings to influence the code generator behavior.
type Options struct {
	// Name of the backend to generate code for.
	//
	// e.g. "k8s_one", "graphviz"
	BackendName string

	// TODO(b/271031113): Below are specific features that pertain to the
	// k8s_one backend. It might be better to configure `k8s_one.K8sOneFeatures`.
	// directly.

	// The mode in which to run the analysis.
	//
	// The options are either "live" or "submission".
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

// Context contains data relevant to a single codegen invocation.
type Context struct {
	// Verbose output.
	Verbose bool

	// Options to configure the behavior of the code generator.
	Options *Options

	// The ASG instance from which to codegen.
	//
	// This should be the final ASG after all other compiler phases
	// and is expected to be syntactically and semantically correct.
	//
	// Codegen backends should not modify this.
	Asg *asg.Graph

	// The string containing the compiled output.
	OutputString string
}

func (c *Context) verifyInitialization() error {
	if c.Asg == nil {
		return fmt.Errorf("no ASG given")
	}
	if c.Options == nil {
		return fmt.Errorf("no codegen Options given")
	}
	// TODO(b/271031113): Move this check into the codegen.
	if c.Options.RuntimeInfo == nil {
		return fmt.Errorf("no RuntimeInfo given")
	}
	return nil
}
