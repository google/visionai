// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package graphviz

import (
	"bytes"
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// Codegen is the main entry point into the graphviz code generator.
func Codegen(ctx *Context) error {
	if err := ctx.verifyInitialization(); err != nil {
		return err
	}

	if err := prologue(&ctx.OutputBuffer); err != nil {
		return err
	}

	leave := func(n *asg.Node) error {
		element := n.Element()
		switch element := element.(type) {
		case *asg.StreamElement:
			return genStream(element.Info, &ctx.OutputBuffer)
		case *asg.AnalyzerElement:
			return genAnalyzer(element.Info, &ctx.OutputBuffer)
		case *asg.SentinelElement:
			return genSentinel(element.Info, &ctx.OutputBuffer)
		default:
			return fmt.Errorf("internal error: unrecognized ASG Element type %T for %q", element, n.Name())
		}
	}
	if err := asg.ReverseDFS(ctx.Asg, nil, leave); err != nil {
		return err
	}

	if err := epilogue(&ctx.OutputBuffer); err != nil {
		return err
	}

	return nil
}

func prologue(output *bytes.Buffer) error {
	text := `digraph G {
node [shape=box];
`
	_, err := output.WriteString(text)
	if err != nil {
		return err
	}
	return nil
}

func epilogue(output *bytes.Buffer) error {
	text := `}
`
	_, err := output.WriteString(text)
	if err != nil {
		return err
	}
	return nil
}
