// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package driver

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/codegen/codegen"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

// Context contains data relevant across all phases of
// a single compiler invocation.
type Context struct {
	// Controls the verbosity of the compiler.
	Verbose bool

	// Options to configure the behavior of the code generator.
	CodegenOptions *codegen.Options

	// The string containing the input analysis definition.
	//
	// If the program is in a file, then the user must first read
	// that file into a string and pass it here.
	InputProgramText string

	// The string containing the compiled output.
	//
	// After a successful compile, the result will be stored here.
	OutputProgramText string

	// A list of attributes and their values used for overriding
	// existing values in the input program text.
	//
	// The format of each element is
	// "<analyzer_name>:<attribute_name>=<override_value>".
	AttributeOverrides []string

	// The operators infomation, indexed by the analyzer name.
	OperatorsInfo map[string]*operators.OperatorInfo

	// The operator registry.
	operatorRegistry *operators.OperatorRegistry

	// The abstract semantic graph.
	asg *asg.Graph
}

// NewDefaultContext creates a fresh default compiler context.
//
// Most notably, it uses the default operator registry.
//
// The user must still configure the context parameters to get the desired
// compiler behavior.
func NewDefaultContext() (*Context, error) {
	ctx := &Context{}

	opRegistry, err := operators.DefaultRegistry()
	if err != nil {
		return nil, fmt.Errorf("failed to get the default OperatorRegistry: %v", err)
	}
	ctx.operatorRegistry = opRegistry

	ctx.CodegenOptions = &codegen.Options{
		RuntimeInfo: &asg.RuntimeInfo{},
	}

	return ctx, nil
}
