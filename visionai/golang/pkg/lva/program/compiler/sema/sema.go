// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// Sema is the main entry point into the semantic analyzer.
//
// It takes a sema context and performs semantic analysis and
// verification on the ASG that it contains.
func Sema(ctx *Context) error {
	if err := ctx.verifyInitialization(); err != nil {
		return err
	}

	if err := doLocalTasks(ctx); err != nil {
		return err
	}

	if err := ctx.verifyFinalization(); err != nil {
		return err
	}

	return nil
}

// doLocalTasks performs all semantic tasks that require only
// information present at a node and its neighbors.
func doLocalTasks(ctx *Context) error {
	leave := func(n *asg.Node) error {
		if err := checkAndSetArgumentInformation(n); err != nil {
			return err
		}
		if err := checkAndSetAttributeInformation(n); err != nil {
			return err
		}
		if err := checkAndSetAttributeOverrides(n, ctx.analyzerToAttributeOverrides); err != nil {
			return err
		}
		if err := updateNodeInfomation(n); err != nil {
			return err
		}
		return nil
	}
	if err := asg.ReverseDFS(ctx.Asg, nil, leave); err != nil {
		return err
	}
	return nil
}
