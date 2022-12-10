// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package parse

import (
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// Parse is the main entry point into the parser.
//
// It takes a parser context and builds an ASG. Moreover, it will
// verify that such an ASG is well-formed and report an error if
// it is not.
func Parse(ctx *Context) (*asg.Graph, error) {
	if err := ctx.verifyInitialization(); err != nil {
		return nil, err
	}

	if err := buildAsg(ctx); err != nil {
		return nil, err
	}

	if err := asg.VerifyGraphIsWellFormed(ctx.aGraph); err != nil {
		return nil, err
	}
	return ctx.aGraph, nil
}
