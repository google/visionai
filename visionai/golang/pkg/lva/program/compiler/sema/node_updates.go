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

// It is expected to be run as a visitor with ReverseDFS.
func updateNodeInfomation(n *asg.Node) error {
	element := n.Element()
	switch element.(type) {
	case *asg.StreamElement:
		return updateStreamNodeInfomation(n)
	case *asg.AnalyzerElement:
		return nil
	case *asg.SentinelElement:
		return nil
	case nil:
		return fmt.Errorf("internal error: %q does not have a ASG element set", n.Name())
	default:
		return fmt.Errorf("internal error: unrecognized ASG element type %T", n.Name())
	}
}

func updateStreamNodeInfomation(n *asg.Node) error {
	streamElement, ok := n.Element().(*asg.StreamElement)
	if !ok {
		return fmt.Errorf("internal error: %q is not a StreamElement (got %T)", n.Name(), n.Element())
	}
	downstreams := 0
	for _, e := range n.OutEdges() {
		if !e.Dst().IsSink() {
			downstreams++
		}
	}
	streamElement.Info.DownstreamAnalyzers = downstreams
	return nil
}
