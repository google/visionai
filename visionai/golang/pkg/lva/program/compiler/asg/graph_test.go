// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"testing"
)

func TestGraphNewGraph(t *testing.T) {
	g, err := NewGraph()
	checkNotNil(t, g)
	checkNil(t, err)

	checkNodesAndEdges(t, g, 2, 0)

	sourceNode := g.SourceNode()
	checkNotNil(t, sourceNode)
	checkEqual(t, sentinelSourceID, sourceNode.id)

	sinkNode := g.SinkNode()
	checkNotNil(t, sinkNode)
	checkEqual(t, sentinelSinkID, sinkNode.id)
}

func TestGraphAddNode(t *testing.T) {
	g, err := NewGraph()
	checkNotNil(t, g)
	checkNil(t, err)

	n, err := g.AddNode()
	checkNotNil(t, n)
	checkNil(t, err)
	checkEqual(t, 0, len(n.inEdges))
	checkEqual(t, 0, len(n.outEdges))

	checkNodesAndEdges(t, g, 3, 0)
}

func TestGraphAddEdge(t *testing.T) {
	g, err := NewGraph()
	checkNotNil(t, g)
	checkNil(t, err)

	u, err := g.AddNode()
	checkNotNil(t, u)
	checkNil(t, err)
	checkEqual(t, 0, len(u.inEdges))
	checkEqual(t, 0, len(u.outEdges))

	v, err := g.AddNode()
	checkNotNil(t, v)
	checkNil(t, err)
	checkEqual(t, 0, len(v.inEdges))
	checkEqual(t, 0, len(v.outEdges))

	e, err := g.AddEdge(u, 0, v, 0)
	checkNotNil(t, e)
	checkNil(t, err)
	checkEqual(t, 0, len(u.inEdges))
	checkEqual(t, 1, len(u.outEdges))
	checkEqual(t, 1, len(v.inEdges))
	checkEqual(t, 0, len(v.outEdges))
	checkEqual(t, u, e.src)
	checkEqual(t, 0, e.soutIdx)
	checkEqual(t, v, e.dst)
	checkEqual(t, 0, e.dinIdx)

	checkNodesAndEdges(t, g, 4, 1)
}

func TestGraphRemoveEdge(t *testing.T) {
	g, err := NewGraph()
	checkNotNil(t, g)
	checkNil(t, err)

	u, err := g.AddNode()
	checkNotNil(t, u)
	checkNil(t, err)
	checkEqual(t, 0, len(u.inEdges))
	checkEqual(t, 0, len(u.outEdges))

	v, err := g.AddNode()
	checkNotNil(t, v)
	checkNil(t, err)
	checkEqual(t, 0, len(v.inEdges))
	checkEqual(t, 0, len(v.outEdges))

	// Add edge (u:0, v:0)
	e, err := g.AddEdge(u, 0, v, 0)
	checkNotNil(t, e)
	checkNil(t, err)
	checkEqual(t, 0, len(u.inEdges))
	checkEqual(t, 1, len(u.outEdges))
	checkEqual(t, 1, len(v.inEdges))
	checkEqual(t, 0, len(v.outEdges))
	checkEqual(t, u, e.src)
	checkEqual(t, 0, e.soutIdx)
	checkEqual(t, v, e.dst)
	checkEqual(t, 0, e.dinIdx)
	checkNodesAndEdges(t, g, 4, 1)
	checkEqual(t, 0, e.id)

	// Remove edge (u:0, v:0)
	err = g.RemoveEdge(e)
	checkNil(t, err)
	checkEqual(t, 0, len(u.inEdges))
	checkEqual(t, 0, len(u.outEdges))
	checkEqual(t, 0, len(v.inEdges))
	checkEqual(t, 0, len(v.outEdges))
	checkNodesAndEdges(t, g, 4, 0)

	// Attempt to remove already removed edge.
	err = g.RemoveEdge(e)
	checkNotNil(t, err)

	// Add edge (u:0, u:0)
	e, err = g.AddEdge(u, 0, u, 0)
	checkNotNil(t, e)
	checkNil(t, err)
	checkEqual(t, 1, len(u.inEdges))
	checkEqual(t, 1, len(u.outEdges))
	checkEqual(t, u, e.src)
	checkEqual(t, 0, e.soutIdx)
	checkEqual(t, u, e.dst)
	checkEqual(t, 0, e.dinIdx)
	checkNodesAndEdges(t, g, 4, 1)
	checkEqual(t, 1, e.id)

	// Remove edge (u:0, u:0)
	err = g.RemoveEdge(e)
	checkNil(t, err)
	checkEqual(t, 0, len(u.inEdges))
	checkEqual(t, 0, len(u.outEdges))
	checkNodesAndEdges(t, g, 4, 0)
}
