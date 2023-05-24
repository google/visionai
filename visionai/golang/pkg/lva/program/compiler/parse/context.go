// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package parse

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

// Context contains data relevant to a single parser invocation.
type Context struct {
	// The string holding an analysis definition to use as input.
	InputProgramText string

	// The operator registry.
	OperatorRegistry *operators.OperatorRegistry

	// The operator information.
	OperatorsInfo map[string]*operators.OperatorInfo

	// The abstract syntax graph.
	aGraph *asg.Graph
}

func (c *Context) verifyInitialization() error {
	if c.OperatorRegistry == nil {
		return fmt.Errorf("given a nil instance of an OperatorRegistry")
	}

	if c.InputProgramText == "" {
		return fmt.Errorf("no input program text was given")
	}
	return nil
}
