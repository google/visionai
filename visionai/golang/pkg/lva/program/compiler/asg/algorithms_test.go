// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"fmt"
	"reflect"
	"testing"
)

func TestFixupSourceAndSinkEdges_EmptyGraph(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 2, 0)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 2, 0)
}

func TestFixupSourceAndSinkEdges_Singleton(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 2, 0)

	u, err := g.AddNode()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 0)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 2)

	checkInEdges(t, u, g.SourceNode())
	checkOutEdges(t, u, g.SinkNode())
}

func TestFixupSourceAndSinkEdges_TwoNodes(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 2, 0)

	// Two nodes. No edges.
	u, err := g.AddNode()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 0)

	v, err := g.AddNode()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 4, 0)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 4, 4)
	checkInEdges(t, u, g.SourceNode())
	checkOutEdges(t, u, g.SinkNode())
	checkInEdges(t, v, g.SourceNode())
	checkOutEdges(t, v, g.SinkNode())

	// Add an edge.
	_, err = g.AddEdge(u, 0, v, 0)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 4, 5)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 4, 3)
	checkInEdges(t, u, g.SourceNode())
	checkOutEdges(t, u, v)
	checkInEdges(t, v, u)
	checkOutEdges(t, v, g.SinkNode())
}

// Note that Graphs containing only non-sentinel nodes that
// only have non-sentinel in-edges will leave the Soure sentinel
// isolated after a Fixup. This case will necessarily contain a cycle.
func TestFixupSourceAndSinkEdges_SingletonWithSelfLoop(t *testing.T) {
	g, err := NewGraph()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 2, 0)

	u, err := g.AddNode()
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 0)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 2)
	checkInEdges(t, u, g.SourceNode())
	checkOutEdges(t, u, g.SinkNode())

	// Add self loop.
	_, err = g.AddEdge(u, 0, u, 0)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 3)

	err = FixupSourceAndSinkEdges(g)
	checkNil(t, err)
	checkNodesAndEdges(t, g, 3, 1)
	checkInEdges(t, u, u)
	checkOutEdges(t, u, u)
	checkOutEdges(t, g.SourceNode())
	checkInEdges(t, g.SinkNode())
}

type dfsInterval struct {
	discoverTime, finishTime int
}

func newdfsInterval() *dfsInterval {
	return &dfsInterval{
		discoverTime: -1,
		finishTime:   -1,
	}
}

func (d *dfsInterval) isValid() bool {
	return d.discoverTime >= 0
	//return d.discoverTime >= 0 && d.finishTime >= 0 && d.discoverTime < d.finishTime
}

func (d *dfsInterval) isDisjointFrom(other *dfsInterval) bool {
	return d.discoverTime > other.finishTime || d.finishTime < other.discoverTime
}

func (d *dfsInterval) covers(other *dfsInterval) bool {
	return d.discoverTime < other.discoverTime && other.finishTime < d.finishTime
}

// A enter callback to track dfs time steps.
func dfsTimerEnter(timeStep *int, dfsIntervalMap map[*Node]*dfsInterval) func(n *Node) error {
	return func(n *Node) error {
		if _, exists := dfsIntervalMap[n]; exists {
			return fmt.Errorf("enter: encountered an undiscovered a node %q that is actually already discovered", n.Name())
		}
		(*timeStep)++
		interval := newdfsInterval()
		interval.discoverTime = *timeStep
		dfsIntervalMap[n] = interval
		return nil
	}
}

// A leave callback to track dfs time steps.
func dfsTimerLeave(timeStep *int, dfsIntervalMap map[*Node]*dfsInterval) func(n *Node) error {
	return func(n *Node) error {
		if _, exists := dfsIntervalMap[n]; !exists {
			return fmt.Errorf("leave: node %q is not present in meta data map but should be", n.Name())
		}
		interval := dfsIntervalMap[n]
		(*timeStep)++
		interval.finishTime = *timeStep
		if interval.discoverTime < 0 {
			return fmt.Errorf("leave: node %q's discover should have been set but is not", n.Name())
		}
		if interval.finishTime <= interval.discoverTime {
			return fmt.Errorf("leave: node %q's discover time should be earlier than the finish time but is not", n.Name())
		}
		return nil
	}
}

func TestDFSFrom_Empty(t *testing.T) {
	// Construct graph.
	g, err := NewGraph()
	checkNil(t, err)

	// Run from Source.
	timeStep := 0
	dfsIntervalMap := make(map[*Node]*dfsInterval)
	enter := dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave := dfsTimerLeave(&timeStep, dfsIntervalMap)
	err = DFSFrom(g, []*Node{g.SourceNode()}, enter, leave)
	checkNil(t, err)

	// Source should have been seen but Sink should not.
	_, exist := dfsIntervalMap[g.SourceNode()]
	checkTrue(t, exist)
	_, exist = dfsIntervalMap[g.SinkNode()]
	checkFalse(t, exist)

	// Run from Sink.
	timeStep = 0
	dfsIntervalMap = make(map[*Node]*dfsInterval)
	enter = dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave = dfsTimerLeave(&timeStep, dfsIntervalMap)
	err = DFSFrom(g, []*Node{g.SinkNode()}, enter, leave)
	checkNil(t, err)

	// Sink should have been seen but Souce should not.
	_, exist = dfsIntervalMap[g.SourceNode()]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[g.SinkNode()]
	checkTrue(t, exist)
}

func TestDFSFrom_Height2BinaryTree(t *testing.T) {
	// Construct graph.
	g, err := NewGraph()
	checkNil(t, err)
	r, err := g.AddNode()
	checkNil(t, err)
	u10, err := g.AddNode()
	checkNil(t, err)
	u11, err := g.AddNode()
	checkNil(t, err)
	u20, err := g.AddNode()
	checkNil(t, err)
	u21, err := g.AddNode()
	checkNil(t, err)
	u22, err := g.AddNode()
	checkNil(t, err)
	u23, err := g.AddNode()
	checkNil(t, err)

	_, _ = g.AddEdge(r, 0, u10, 0)
	_, _ = g.AddEdge(r, 0, u11, 0)
	_, _ = g.AddEdge(u10, 0, u20, 0)
	_, _ = g.AddEdge(u10, 0, u21, 0)
	_, _ = g.AddEdge(u11, 0, u22, 0)
	_, _ = g.AddEdge(u11, 0, u23, 0)

	// Run DFS with timing from the two children of the root.
	timeStep := 0
	dfsIntervalMap := make(map[*Node]*dfsInterval)
	enter := dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave := dfsTimerLeave(&timeStep, dfsIntervalMap)
	starts := []*Node{u10, u11}
	err = DFSFrom(g, starts, enter, leave)
	checkNil(t, err)

	// Source and Sink should not have been found.
	// (notice we intentionally didn't run fixup)
	_, exist := dfsIntervalMap[g.SourceNode()]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[g.SinkNode()]
	checkFalse(t, exist)

	// Root should not have been found.
	_, exist = dfsIntervalMap[r]
	checkFalse(t, exist)

	// Check left subtree.
	u10Interval, exist := dfsIntervalMap[u10]
	checkTrue(t, exist)
	checkTrue(t, u10Interval.isValid())
	u20Interval, exist := dfsIntervalMap[u20]
	checkTrue(t, exist)
	checkTrue(t, u20Interval.isValid())
	u21Interval, exist := dfsIntervalMap[u21]
	checkTrue(t, exist)
	checkTrue(t, u21Interval.isValid())

	checkTrue(t, u10Interval.covers(u20Interval))
	checkTrue(t, u10Interval.covers(u21Interval))
	checkTrue(t, u20Interval.isDisjointFrom(u21Interval))

	// Check right subtree.
	u11Interval, exist := dfsIntervalMap[u11]
	checkTrue(t, exist)
	checkTrue(t, u11Interval.isValid())
	u22Interval, exist := dfsIntervalMap[u22]
	checkTrue(t, exist)
	checkTrue(t, u22Interval.isValid())
	u23Interval, exist := dfsIntervalMap[u23]
	checkTrue(t, exist)
	checkTrue(t, u23Interval.isValid())

	checkTrue(t, u11Interval.covers(u22Interval))
	checkTrue(t, u11Interval.covers(u23Interval))
	checkTrue(t, u22Interval.isDisjointFrom(u23Interval))

	// Subtree intervals should be disjoint.
	checkTrue(t, u10Interval.isDisjointFrom(u11Interval))
}

func TestReverseDFSFrom_Empty(t *testing.T) {
	// Construct graph.
	g, err := NewGraph()
	checkNil(t, err)

	// Run from Sink.
	timeStep := 0
	dfsIntervalMap := make(map[*Node]*dfsInterval)
	enter := dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave := dfsTimerLeave(&timeStep, dfsIntervalMap)
	err = ReverseDFSFrom(g, []*Node{g.SinkNode()}, enter, leave)
	checkNil(t, err)

	// Sink should have been seen but Source should not.
	_, exist := dfsIntervalMap[g.SinkNode()]
	checkTrue(t, exist)
	_, exist = dfsIntervalMap[g.SourceNode()]
	checkFalse(t, exist)

	// Run from Source.
	timeStep = 0
	dfsIntervalMap = make(map[*Node]*dfsInterval)
	enter = dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave = dfsTimerLeave(&timeStep, dfsIntervalMap)
	err = ReverseDFSFrom(g, []*Node{g.SourceNode()}, enter, leave)
	checkNil(t, err)

	// Source should have been seen but Sink should not.
	_, exist = dfsIntervalMap[g.SinkNode()]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[g.SourceNode()]
	checkTrue(t, exist)
}

func TestReverseDFSFrom_Height2BinaryTree(t *testing.T) {
	// Construct graph.
	g, err := NewGraph()
	checkNil(t, err)
	r, err := g.AddNode()
	checkNil(t, err)
	u10, err := g.AddNode()
	checkNil(t, err)
	u11, err := g.AddNode()
	checkNil(t, err)
	u20, err := g.AddNode()
	checkNil(t, err)
	u21, err := g.AddNode()
	checkNil(t, err)
	u22, err := g.AddNode()
	checkNil(t, err)
	u23, err := g.AddNode()
	checkNil(t, err)

	_, _ = g.AddEdge(r, 0, u10, 0)
	_, _ = g.AddEdge(r, 0, u11, 0)
	_, _ = g.AddEdge(u10, 0, u20, 0)
	_, _ = g.AddEdge(u10, 0, u21, 0)
	_, _ = g.AddEdge(u11, 0, u22, 0)
	_, _ = g.AddEdge(u11, 0, u23, 0)

	// Run ReverseDFS with timing from the two children of the root.
	timeStep := 0
	dfsIntervalMap := make(map[*Node]*dfsInterval)
	enter := dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave := dfsTimerLeave(&timeStep, dfsIntervalMap)
	starts := []*Node{u10, u11}
	err = ReverseDFSFrom(g, starts, enter, leave)
	checkNil(t, err)

	// Source and Sink should not have been found.
	// (notice we intentionally didn't run fixup)
	_, exist := dfsIntervalMap[g.SourceNode()]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[g.SinkNode()]
	checkFalse(t, exist)

	// Only the root and its immediate chilren are found.
	_, exist = dfsIntervalMap[r]
	checkTrue(t, exist)
	_, exist = dfsIntervalMap[u10]
	checkTrue(t, exist)
	_, exist = dfsIntervalMap[u11]
	checkTrue(t, exist)
	_, exist = dfsIntervalMap[u20]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[u21]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[u22]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[u23]
	checkFalse(t, exist)
}

func TestReverseDFSFrom_Height2BinaryTreeReversed(t *testing.T) {
	// Construct graph.
	g, err := NewGraph()
	checkNil(t, err)
	r, err := g.AddNode()
	checkNil(t, err)
	u10, err := g.AddNode()
	checkNil(t, err)
	u11, err := g.AddNode()
	checkNil(t, err)
	u20, err := g.AddNode()
	checkNil(t, err)
	u21, err := g.AddNode()
	checkNil(t, err)
	u22, err := g.AddNode()
	checkNil(t, err)
	u23, err := g.AddNode()
	checkNil(t, err)

	// Edges run towards the "root".
	_, _ = g.AddEdge(u10, 0, r, 0)
	_, _ = g.AddEdge(u11, 0, r, 0)
	_, _ = g.AddEdge(u20, 0, u10, 0)
	_, _ = g.AddEdge(u21, 0, u10, 0)
	_, _ = g.AddEdge(u22, 0, u11, 0)
	_, _ = g.AddEdge(u23, 0, u11, 0)

	// Run ReverseDFS with timing from the two "children" of the root.
	timeStep := 0
	dfsIntervalMap := make(map[*Node]*dfsInterval)
	enter := dfsTimerEnter(&timeStep, dfsIntervalMap)
	leave := dfsTimerLeave(&timeStep, dfsIntervalMap)
	starts := []*Node{u10, u11}
	err = ReverseDFSFrom(g, starts, enter, leave)
	checkNil(t, err)

	// Source and Sink should not have been found.
	// (notice we intentionally didn't run fixup)
	_, exist := dfsIntervalMap[g.SourceNode()]
	checkFalse(t, exist)
	_, exist = dfsIntervalMap[g.SinkNode()]
	checkFalse(t, exist)

	// Root should not have been found.
	_, exist = dfsIntervalMap[r]
	checkFalse(t, exist)

	// Check left subtree.
	u10Interval, exist := dfsIntervalMap[u10]
	checkTrue(t, exist)
	checkTrue(t, u10Interval.isValid())
	u20Interval, exist := dfsIntervalMap[u20]
	checkTrue(t, exist)
	checkTrue(t, u20Interval.isValid())
	u21Interval, exist := dfsIntervalMap[u21]
	checkTrue(t, exist)
	checkTrue(t, u21Interval.isValid())

	checkTrue(t, u10Interval.covers(u20Interval))
	checkTrue(t, u10Interval.covers(u21Interval))
	checkTrue(t, u20Interval.isDisjointFrom(u21Interval))

	// Check right subtree.
	u11Interval, exist := dfsIntervalMap[u11]
	checkTrue(t, exist)
	checkTrue(t, u11Interval.isValid())
	u22Interval, exist := dfsIntervalMap[u22]
	checkTrue(t, exist)
	checkTrue(t, u22Interval.isValid())
	u23Interval, exist := dfsIntervalMap[u23]
	checkTrue(t, exist)
	checkTrue(t, u23Interval.isValid())

	checkTrue(t, u11Interval.covers(u22Interval))
	checkTrue(t, u11Interval.covers(u23Interval))
	checkTrue(t, u22Interval.isDisjointFrom(u23Interval))

	// Subtree intervals should be disjoint.
	checkTrue(t, u10Interval.isDisjointFrom(u11Interval))
}

func checkNodesAndEdges(t *testing.T, g *Graph, nodes, edges int) {
	t.Helper()
	if nodes != g.NumNodes() || edges != g.NumEdges() {
		t.Helper()
		t.Fatalf("want %d nodes and %d edge, got %d nodes and %d edges", nodes, edges, g.NumNodes(), g.NumEdges())
	}
}

func isNil(obj interface{}) bool {
	return obj == nil || reflect.ValueOf(obj).IsNil()
}

func checkNil(t *testing.T, obj interface{}) {
	t.Helper()
	if !isNil(obj) {
		t.Fatalf("want nil, got %v", obj)
	}
}

func checkNotNil(t *testing.T, obj interface{}) {
	t.Helper()
	if isNil(obj) {
		t.Fatalf("want non-nil, got %v", obj)
	}
}

func checkTrue(t *testing.T, got bool) {
	t.Helper()
	if !got {
		t.Fatalf("want true, got %t", got)
	}
}

func checkFalse(t *testing.T, got bool) {
	t.Helper()
	if got {
		t.Fatalf("want false, got %t", got)
	}
}
func checkInEdges(t *testing.T, u *Node, src ...*Node) {
	t.Helper()
	if len(u.InEdges()) != len(src) {
		t.Fatalf("want %d edges, got %d edges", len(src), len(u.InEdges()))
	}
	for id, v := range src {
		if u.InEdges()[id].Src() != v {
			t.Fatalf("want %dth edge connect %v, got %v", id, v, u.InEdges()[id].Src())
		}
	}
}

func checkOutEdges(t *testing.T, u *Node, dst ...*Node) {
	t.Helper()
	if len(u.OutEdges()) != len(dst) {
		t.Fatalf("want %d edges, got %d edges", len(dst), len(u.OutEdges()))
	}
	for id, v := range dst {
		if u.OutEdges()[id].Dst() != v {
			t.Fatalf("want %dth edge connect %v, got %v", id, v, u.OutEdges()[id].Dst())
		}
	}
}

func checkEqual(t *testing.T, want, got interface{}) {
	t.Helper()
	if want != got {
		t.Errorf("want %v, got %v", want, got)
	}
}
