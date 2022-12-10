// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"fmt"
	"strings"
)

// VerifyGraphIsWellFormed verifies that the given graph
// satisfies all the invariants of a well formed ASG.
//
// Returns nil if the given graph passes.
//
// Well formed ASGs are suitable for semantic analysis.
// Parsers and most graph rewriters/transformers are
// expected to produce well formed ASGs as output.
//
// One may consider this function as the definition for
// what it means for a Graph to be well-formed.
func VerifyGraphIsWellFormed(g *Graph) error {
	if g == nil {
		return fmt.Errorf("given a nil Graph")
	}

	if err := VerifyNodesHaveNoDuplicateInputs(g); err != nil {
		return err
	}

	if err := VerifySourceSinkCorrectlyLinked(g); err != nil {
		return err
	}

	if err := VerifyGraphHasNoCycles(g); err != nil {
		return err
	}

	if err := VerifySourceReachability(g); err != nil {
		return err
	}

	if err := VerifySinkReverseReachability(g); err != nil {
		return err
	}

	return nil
}

// VerifySourceSinkCorrectlyLinked verifies that the sentinel
// Source and Sink nodes are correctly linked in the
// given Graph.
//
// Returns nil if the given graph passes.
//
// Graphs that don't pass tend to indicate the presence
// of an internal library error.
func VerifySourceSinkCorrectlyLinked(g *Graph) error {
	if g == nil {
		return fmt.Errorf("given a nil Graph")
	}

	sourceNode := g.SourceNode()
	if sourceNode == nil {
		return fmt.Errorf("the sentinel source Node has been removed")
	}
	if len(sourceNode.inEdges) != 0 {
		return fmt.Errorf("the sentinel source Node has an incoming Edge")
	}
	for _, e := range sourceNode.outEdges {
		if e.soutIdx != -1 || e.dinIdx != -1 {
			return fmt.Errorf("the sentinel source Node should not connect non-negative ports (soutIdx=%d, dinIdx=%d)", e.soutIdx, e.dinIdx)
		}
	}

	sinkNode := g.SinkNode()
	if sinkNode == nil {
		return fmt.Errorf("the sentinel sink Node has been removed")
	}
	if len(sinkNode.outEdges) != 0 {
		return fmt.Errorf("the sentinel sink Node has an outgoing Edge")
	}
	for _, e := range sinkNode.inEdges {
		if e.soutIdx != -1 || e.dinIdx != -1 {
			return fmt.Errorf("the sentinel sink Node should not connect non-negative ports (soutIdx=%d, dinIdx=%d)", e.soutIdx, e.dinIdx)
		}
	}

	for _, n := range g.nodes {
		// Skip deleted nodes and sentinel nodes.
		if n == nil || n.IsSource() || n.IsSink() {
			continue
		}

		// Check source connections.
		needsSourceSentinelEdge := true
		hasSourceSentinelEdge := false
		for _, e := range n.inEdges {
			if e.src.IsSource() {
				if hasSourceSentinelEdge {
					return fmt.Errorf("%q is connected to the Source sentinel more than once", n.Name())
				}
				hasSourceSentinelEdge = true
			} else {
				needsSourceSentinelEdge = false
			}
		}
		if needsSourceSentinelEdge && !hasSourceSentinelEdge {
			return fmt.Errorf("%q needs a Source sentinel Edge but does not have one", n.Name())
		}
		if !needsSourceSentinelEdge && hasSourceSentinelEdge {
			return fmt.Errorf("%q does not need a Source sentinel Edge but has one", n.Name())
		}

		// Check sink connections.
		needsSinkSentinelEdge := true
		hasSinkSentinelEdge := false
		for _, e := range n.outEdges {
			if e.dst.IsSink() {
				if hasSinkSentinelEdge {
					return fmt.Errorf("%q is connected to the Sink sentinel more than once", n.Name())
				}
				hasSinkSentinelEdge = true
			} else {
				needsSinkSentinelEdge = false
			}
		}
		if needsSinkSentinelEdge && !hasSinkSentinelEdge {
			return fmt.Errorf("%q needs a Sink sentinel Edge but does not have one", n.Name())
		}
		if !needsSinkSentinelEdge && hasSinkSentinelEdge {
			return fmt.Errorf("%q does not need a Sink sentinel Edge but has one", n.Name())
		}
	}

	return nil
}

// VerifyNodesHaveNoDuplicateInputs verifies that no Nodes
// in the given graph have more than one Edge incident on
// the same input index.
//
// Returns nil if the given graph passes.
func VerifyNodesHaveNoDuplicateInputs(g *Graph) error {
	if g == nil {
		return fmt.Errorf("given a nil Graph")
	}

	for _, n := range g.nodes {
		// Skip deleted nodes and sentinel nodes.
		if n == nil || n.IsSource() || n.IsSink() {
			continue
		}

		dinIdxMap := make(map[int]*Node)
		for _, e := range n.inEdges {
			if dup, exists := dinIdxMap[e.dinIdx]; exists {
				return fmt.Errorf("input index %d of %q has more than one edge incident (please check connections from %q and %q)", e.dinIdx, n.Name(), dup.Name(), e.src.Name())
			}
			dinIdxMap[e.dinIdx] = e.src
		}
	}
	return nil
}

// VerifyGraphHasNoCycles verifies that the given graph does
// not contain a cycle.
//
// Returns nil if the given graph passes.
func VerifyGraphHasNoCycles(g *Graph) error {
	if g == nil {
		return fmt.Errorf("given a nil Graph")
	}

	// Struct to store visitation status and dfs tree links.
	type dfsMeta struct {
		done   bool
		parent *Node
	}
	discovered := make(map[*Node]*dfsMeta)

	// Slice to store cycle result if one is found.
	var cycle []*Node

	// dfsFrom is the recursive DFS call to find back edges.
	// current is the node being discovered, and parent is the
	// node from which current was discovered.
	//
	// Return false to request callers to stop processing
	// (for example, for early stopping if a cycle is found)
	var dfsFrom func(parent, current *Node) bool
	dfsFrom = func(parent, current *Node) bool {
		currentMeta := &dfsMeta{
			done:   false,
			parent: parent,
		}
		discovered[current] = currentMeta

		for _, e := range current.outEdges {
			if childMeta, exists := discovered[e.dst]; exists {
				if childMeta.done {
					continue
				}
				// Found a back edge; trace parents to populate the cycle.
				cycle = append(cycle, e.dst)
				curr := current
				for {
					cycle = append(cycle, curr)
					meta := discovered[curr]
					if meta.parent == e.dst {
						break
					}
					curr = meta.parent
				}
				cycle = append(cycle, e.dst)
				return false
			}
			if !dfsFrom(current, e.dst) {
				return false
			}
		}
		currentMeta.done = true
		return true
	}

	// Top level call over all undiscovered nodes.
	for _, n := range g.nodes {
		if n == nil {
			continue
		}
		if _, exists := discovered[n]; exists {
			continue
		}
		if !dfsFrom(nil, n) {
			break
		}
	}

	if cycle != nil {
		var cycleNodeNames []string
		// The cycle slice as is is in reversed w.r.t. edge directions.
		// Iterate backwards for printing.
		for i := len(cycle) - 1; i >= 0; i-- {
			cycleNodeNames = append(cycleNodeNames, fmt.Sprintf("%q", cycle[i].Name()))
		}
		cycleString := strings.Join(cycleNodeNames, ", ")
		return fmt.Errorf("found a cycle in the Graph (%s)", cycleString)
	}
	return nil
}

// VerifySourceReachability verifies that all Nodes of the given Graph
// are reachable from the Source.
//
// Returns nil if the given graph passes.
func VerifySourceReachability(g *Graph) error {
	return verifySourceOrSinkReachability(g, true)
}

// VerifySinkReverseReachability verifies that all Nodes of the given
// Graph can reach the Sink.
//
// Returns nil if the given graph passes.
func VerifySinkReverseReachability(g *Graph) error {
	return verifySourceOrSinkReachability(g, false)
}

// verifySourceOrSinkReachability verifies source reachability or
// sink reverse reachability.
func verifySourceOrSinkReachability(g *Graph, fromSource bool) error {
	if g == nil {
		return fmt.Errorf("given a nil Graph")
	}

	// Keep track of discovered nodes.
	discovered := make(map[*Node]struct{})
	enter := func(n *Node) error {
		discovered[n] = struct{}{}
		return nil
	}

	// Forward or reverse from source or sink depending on request.
	traversalFunc := DFS
	origin := g.SourceNode()
	if !fromSource {
		traversalFunc = ReverseDFS
		origin = g.SinkNode()
	}
	if err := traversalFunc(g, enter, nil); err != nil {
		return fmt.Errorf("encountered error while traversing graph: %v", err)
	}

	// Verify all nodes have been discovered.
	for _, n := range g.Nodes() {
		if _, exists := discovered[n]; !exists {
			return fmt.Errorf("%q is not reachable from %q", n.Name(), origin.Name())
		}
		delete(discovered, n)
	}
	if len(discovered) != 0 {
		return fmt.Errorf("internal error: reached Nodes that are supposedly not present")
	}

	return nil
}
