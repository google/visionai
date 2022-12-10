// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"testing"
)

func TestVerifySourceSinkCorrectlyLinked_EmptyGraph(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	err = VerifySourceSinkCorrectlyLinked(g)
	checkNil(t, err)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
}

func TestVerifySourceSinkCorrectlyLinked_Singleton(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	_, err = g.AddNode()
	checkNil(t, err)

	err = VerifySourceSinkCorrectlyLinked(g)
	checkNotNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceSinkCorrectlyLinked(g)
	checkNil(t, err)
}

func TestVerifySourceSinkCorrectlyLinked_TwoNodes(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	// Two nodes. No edges.
	u, err := g.AddNode()
	checkNil(t, err)
	v, err := g.AddNode()
	checkNil(t, err)

	err = VerifySourceSinkCorrectlyLinked(g)
	checkNotNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceSinkCorrectlyLinked(g)
	checkNil(t, err)

	// Add an edge.
	_, err = g.AddEdge(u, 0, v, 0)
	checkNil(t, err)

	err = VerifySourceSinkCorrectlyLinked(g)
	checkNotNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceSinkCorrectlyLinked(g)
	checkNil(t, err)
}

func TestVerifySourceSinkCorrectlyLinked_SingletonWithSelfLoop(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	u, err := g.AddNode()
	checkNil(t, err)

	err = VerifySourceSinkCorrectlyLinked(g)
	checkNotNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceSinkCorrectlyLinked(g)
	checkNil(t, err)

	// Add an edge.
	_, err = g.AddEdge(u, 0, u, 0)
	checkNil(t, err)

	err = VerifySourceSinkCorrectlyLinked(g)
	checkNotNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceSinkCorrectlyLinked(g)
	checkNil(t, err)
}

func TestVerifyNodesHaveNoDuplicateInputs(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	// Three isolated nodes.
	u1, err := g.AddNode()
	checkNil(t, err)
	u2, err := g.AddNode()
	checkNil(t, err)
	v, err := g.AddNode()
	checkNil(t, err)

	err = VerifyNodesHaveNoDuplicateInputs(g)
	checkNil(t, err)

	// Unique inputs to v.
	_, err = g.AddEdge(u1, 0, v, 0)
	checkNil(t, err)
	e2, err := g.AddEdge(u2, 0, v, 1)
	checkNil(t, err)

	err = VerifyNodesHaveNoDuplicateInputs(g)
	checkNil(t, err)

	// Duplicate inputs to v.
	err = g.RemoveEdge(e2)
	checkNil(t, err)
	e3, err := g.AddEdge(u2, 0, v, 0)
	checkNil(t, err)

	err = VerifyNodesHaveNoDuplicateInputs(g)
	checkNotNil(t, err)

	// Restore unique inputs to v.
	err = g.RemoveEdge(e3)
	checkNil(t, err)
	_, err = g.AddEdge(u2, 0, v, 1)
	checkNil(t, err)

	err = VerifyNodesHaveNoDuplicateInputs(g)
	checkNil(t, err)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyNodesHaveNoDuplicateInputs(g)
	checkNil(t, err)
}

func TestVerifyGraphHasNoCycles_EmptyGraph(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)
}

func TestVerifyGraphHasNoCycles_Singleton(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)
	_, err = g.AddNode()
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)
}

func TestVerifyGraphHasNoCycles_TwoNodes(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	// Two isolated nodes.
	u, err := g.AddNode()
	checkNil(t, err)
	v, err := g.AddNode()
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)

	// u->v.
	_, err = g.AddEdge(u, 0, v, 0)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)

	// u->v->u.
	e, err := g.AddEdge(v, 0, u, 0)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNotNil(t, err)

	// Restore u->v.
	err = g.RemoveEdge(e)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)
}

func TestVerifyGraphHasNoCycles_TwoInLoopTwoOut(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	i0, err := g.AddNode()
	checkNil(t, err)
	i1, err := g.AddNode()
	checkNil(t, err)
	u, err := g.AddNode()
	checkNil(t, err)
	v, err := g.AddNode()
	checkNil(t, err)
	o0, err := g.AddNode()
	checkNil(t, err)
	o1, err := g.AddNode()
	checkNil(t, err)
	_, err = g.AddEdge(i0, 0, u, 0)
	checkNil(t, err)
	_, err = g.AddEdge(i1, 0, u, 1)
	checkNil(t, err)
	_, err = g.AddEdge(u, 0, v, 2)
	checkNil(t, err)
	_, err = g.AddEdge(v, 0, o0, 0)
	checkNil(t, err)
	_, err = g.AddEdge(v, 1, o1, 1)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)

	_, err = g.AddEdge(v, 2, u, 2)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNotNil(t, err)
}

func TestVerifyGraphHasNoCycles_DisconnectedGraph(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	// Graph with no cycles
	u1, err := g.AddNode()
	checkNil(t, err)
	v1, err := g.AddNode()
	checkNil(t, err)
	_, err = g.AddEdge(u1, 0, v1, 0)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)

	// Add another Graph. First without a cycle.
	u2, err := g.AddNode()
	checkNil(t, err)
	v2, err := g.AddNode()
	checkNil(t, err)
	s2, err := g.AddNode()
	checkNil(t, err)
	_, err = g.AddEdge(u2, 0, v2, 0)
	checkNil(t, err)
	_, err = g.AddEdge(v2, 0, s2, 0)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)

	// Make a cycle in the second graph.
	e, err := g.AddEdge(s2, 0, u2, 0)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNotNil(t, err)

	// Restore chain.
	err = g.RemoveEdge(e)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifyGraphHasNoCycles(g)
	checkNil(t, err)
}

func TestVerifySourceSinkReachability_Singleton(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)

	_, err = g.AddNode()
	checkNil(t, err)

	err = VerifySourceReachability(g)
	checkNotNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNotNil(t, err)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)
}

func TestVerifySourceSinkReachability_TwoNodes(t *testing.T) {
	// Two isolated nodes.
	g, err := NewGraph()
	checkNil(t, err)

	u, err := g.AddNode()
	checkNil(t, err)

	v, err := g.AddNode()
	checkNil(t, err)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)

	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)

	// Remove source connection from v.
	e := v.inEdges[0]
	err = g.RemoveEdge(e)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNotNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)

	// Remove sink connection from v.
	e = v.outEdges[0]
	err = g.RemoveEdge(e)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNotNil(t, err)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)

	// Connect u and v.
	e, err = g.AddEdge(u, 0, v, 0)
	checkNotNil(t, e)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)

	// Form cycle between u and v.
	e, err = g.AddEdge(v, 0, u, 0)
	checkNotNil(t, e)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNil(t, err)
	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	err = VerifySourceReachability(g)
	checkNotNil(t, err)
	err = VerifySinkReverseReachability(g)
	checkNotNil(t, err)
}
