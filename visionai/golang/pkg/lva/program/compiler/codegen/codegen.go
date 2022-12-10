// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package codegen

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/codegen/graphviz/graphviz"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/codegen/k8s_one/k8s"
)

// Codegen is the main entry point into the code generator backends.
//
// It takes a well formed ASG and produces a string that holds the generated output.
func Codegen(ctx *Context) error {
	if err := ctx.verifyInitialization(); err != nil {
		return err
	}

	// Dispatch to the chosen codegen backend.
	var err error
	switch backend := ctx.BackendName; backend {
	case "graphviz":
		err = doGraphviz(ctx)
	case "k8s_one":
		err = doK8sOne(ctx)
	default:
		err = fmt.Errorf("unrecognized codegen backend %q", backend)
	}
	if err != nil {
		return err
	}

	return nil
}

// Call into the graphviz backend.
func doGraphviz(ctx *Context) error {
	gvCtx := &graphviz.Context{
		Asg: ctx.Asg,
	}

	err := graphviz.Codegen(gvCtx)
	if err != nil {
		return err
	}
	ctx.OutputString = gvCtx.OutputBuffer.String()

	return nil
}

// Call into the k8s_one backend.
func doK8sOne(ctx *Context) error {
	kctx := &k8s.Context{
		Verbose:     ctx.Verbose,
		EnableDebug: ctx.EnableDebug,
		Asg:         ctx.Asg,
		RuntimeInfo: ctx.RuntimeInfo,
	}

	err := k8s.Codegen(kctx)
	if err != nil {
		return err
	}
	ctx.OutputString = kctx.OutputString

	return nil
}
