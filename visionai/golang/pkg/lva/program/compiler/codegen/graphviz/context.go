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

// Context contains data relevant to a single Graphviz codegen invocation.
type Context struct {
	// The ASG instance from which to codegen.
	Asg *asg.Graph

	// The graphviz dot program text.
	OutputBuffer bytes.Buffer
}

func (c *Context) verifyInitialization() error {
	if c.Asg == nil {
		return fmt.Errorf("no ASG given")
	}
	return nil
}
