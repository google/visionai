// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// Context contains data relevant to a single semantic analysis invocation.
type Context struct {
	// The ASG on which to run semantic analysis.
	Asg *asg.Graph
}

func (c *Context) verifyInitialization() error {
	if c.Asg == nil {
		return fmt.Errorf("no input ASG Graph was provided (got nil)")
	}
	return nil
}
