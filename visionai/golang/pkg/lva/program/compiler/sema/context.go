// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"fmt"
	"strings"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// Context contains data relevant to a single semantic analysis invocation.
type Context struct {
	// The ASG on which to run semantic analysis.
	Asg *asg.Graph

	// The list of attributes and their values used for overriding existing
	// values in the input program.
	//
	// The format of each element is
	// "<analyzer_name>:<attribute_name>=<override_value>".
	//
	// Example:
	// `["analyzer1:attr1=val"]` means to override the `attr1` of `analyzer1`
	// with the value `val`.
	AttributeOverrides []string

	analyzerToAttributeOverrides map[string][]string
}

func (c *Context) verifyInitialization() error {
	if c.Asg == nil {
		return fmt.Errorf("no input ASG Graph was provided (got nil)")
	}

	// Create a map between an analyzer name to all of its attribute overrides.
	c.analyzerToAttributeOverrides = make(map[string][]string)
	for _, s := range c.AttributeOverrides {
		override := strings.SplitN(s, ":", 2)
		if len(override) != 2 {
			return fmt.Errorf("encountered an attribute override without an attribute specified. Attributes must follow the analyzer name with a ':'")
		}
		analyzerName := override[0]
		attributeOverride := override[1]
		c.analyzerToAttributeOverrides[analyzerName] = append(c.analyzerToAttributeOverrides[analyzerName], attributeOverride)
	}
	return nil
}

func (c *Context) verifyFinalization() error {
	for k := range c.analyzerToAttributeOverrides {
		return fmt.Errorf("encountered attribute overrides for an unknown analyzer %q", k)
	}
	return nil
}
