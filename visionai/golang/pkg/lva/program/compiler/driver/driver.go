// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package driver

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/codegen/codegen"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/parse/parse"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/sema/sema"
)

func doParse(dctx *Context) error {
	pctx := &parse.Context{}
	pctx.InputProgramText = dctx.InputProgramText
	pctx.OperatorRegistry = dctx.operatorRegistry
	asg, err := parse.Parse(pctx)
	if err != nil {
		return err
	}
	dctx.asg = asg
	return nil
}

func doSema(dctx *Context) error {
	sctx := &sema.Context{}
	sctx.Asg = dctx.asg
	err := sema.Sema(sctx)
	if err != nil {
		return err
	}
	dctx.asg = sctx.Asg
	return nil
}

func doCodegen(dctx *Context) error {
	cctx := &codegen.Context{}
	cctx.Verbose = dctx.Verbose
	cctx.EnableDebug = dctx.EnableDebug
	cctx.Asg = dctx.asg
	cctx.BackendName = dctx.BackendName
	cctx.RuntimeInfo = dctx.RuntimeInfo
	err := codegen.Codegen(cctx)
	if err != nil {
		return err
	}
	dctx.OutputProgramText = cctx.OutputString
	return nil
}

// Compile is the main entry point into the compiler.
func Compile(ctx *Context) error {
	if ctx == nil {
		return fmt.Errorf("given a nil compiler Context")
	}
	if err := doParse(ctx); err != nil {
		return fmt.Errorf("parse error: %v", err)
	}
	if err := doSema(ctx); err != nil {
		return fmt.Errorf("semantic error: %v", err)
	}
	if err := doCodegen(ctx); err != nil {
		return fmt.Errorf("codegen error: %v", err)
	}
	return nil
}
