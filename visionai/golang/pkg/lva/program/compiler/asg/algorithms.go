// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"fmt"
)

// DFS traverses the given Graph in depth first order from the SourceNode.
//
// It allows the caller to customize visitor behaviour at specific
// event points:
// + enter is run when a Node is discovered. Ignored if nil.
// + leave is run when a Node is finished being explored. Ignored if nil.
//
// DFS will return a non-nil error if the callbacks return an error or if
// the traversal itself encounters an error.
//
// This is typically run on a well-formed ASG to allow the caller
// to focus on compiler specific tasks.
func DFS(g *Graph, enter func(*Node) error, leave func(*Node) error) error {
	starts := []*Node{g.SourceNode()}
	return DFSFrom(g, starts, enter, leave)
}

// DFSFrom is like DFS, but allows the caller to specify a set of start nodes.
// This is mainly useful for general uses of depth first search while
// DFS is more suitable for compiler specific ASG/IR processing.
func DFSFrom(g *Graph, starts []*Node, enter func(*Node) error, leave func(*Node) error) error {
	return dfsFromHelper(g, starts, enter, leave, false)
}

// ReverseDFS traverses the given Graph in depth first order from the SinkNode,
// except it follows edges in the reverse direction.
//
// It is equivalent to running DFS on the reverse/transpose of the given Graph.
func ReverseDFS(g *Graph, enter func(*Node) error, leave func(*Node) error) error {
	starts := []*Node{g.SinkNode()}
	return ReverseDFSFrom(g, starts, enter, leave)
}

// ReverseDFSFrom is the reverse/transposed equivalent of DFSFrom.
func ReverseDFSFrom(g *Graph, starts []*Node, enter func(*Node) error, leave func(*Node) error) error {
	return dfsFromHelper(g, starts, enter, leave, true)
}

// dfsFromHelper performs is the main DFS method that may be configured to
// deliver the desired behavior of the public functions.
// + Supports a configurable starting set of nodes.
// + Supports enter/leave visit points.
// + Supports reverse traversals.
func dfsFromHelper(g *Graph, starts []*Node, enter func(*Node) error, leave func(*Node) error, reverse bool) error {
	type visitStatus int
	const (
		undiscovered visitStatus = iota
		visiting
		completed
	)
	type frame struct {
		status visitStatus
		node   *Node
	}

	var stack []*frame
	for _, n := range starts {
		if n == nil {
			return fmt.Errorf("given a nil to a starting *Node")
		}
		f := &frame{
			status: undiscovered,
			node:   n,
		}
		stack = append(stack, f)
	}

	discovered := make(map[*Node]struct{})
	for len(stack) > 0 {
		f := stack[len(stack)-1]
		stack = stack[:len(stack)-1]

		if f.status == completed {
			continue
		}

		if f.status == visiting {
			if leave != nil {
				if err := leave(f.node); err != nil {
					return fmt.Errorf("error when running the leave callback on node %q: %v", f.node.Name(), err)
				}
			}
			f.status = completed
			continue
		}

		discovered[f.node] = struct{}{}
		if enter != nil {
			if err := enter(f.node); err != nil {
				return fmt.Errorf("error when running the enter callback on node %q: %v", f.node.Name(), err)
			}
		}
		f.status = visiting
		stack = append(stack, f)

		incidentEdges := f.node.outEdges
		if reverse {
			incidentEdges = f.node.inEdges
		}
		for _, e := range incidentEdges {
			child := e.dst
			if reverse {
				child = e.src
			}
			if _, exists := discovered[child]; !exists {
				newf := &frame{
					status: undiscovered,
					node:   child,
				}
				stack = append(stack, newf)
			}
		}
	}
	return nil
}

// FixupSourceAndSinkEdges modifies the given Graph such that sentinel
// Edges, i.e. Edges incident between Nodes on input/output index -1,
// are attached from (to) the sentinel Source (Sink) Node to (from) an
// ordinary Node if and only the ordinary Node has no incoming (outgoing)
// non-sentinel Edges.
func FixupSourceAndSinkEdges(g *Graph) error {
	if g == nil {
		return fmt.Errorf("given a nil Graph")
	}

	// Remove existing Source/Sink edges.
	var existingSentinelEdges []*Edge
	for _, e := range g.edges {
		if e == nil {
			continue
		}
		if e.src.IsSource() || e.dst.IsSink() {
			existingSentinelEdges = append(existingSentinelEdges, e)
		}
	}
	for _, e := range existingSentinelEdges {
		err := g.RemoveEdge(e)
		if err != nil {
			return fmt.Errorf("failed to remove a sentinel edge: %v", err)
		}
	}

	// Re-establish sentinel edges.
	for _, n := range g.nodes {
		// Skip deleted nodes and sentinel nodes.
		if n == nil || n.IsSource() || n.IsSink() {
			continue
		}

		if len(n.inEdges) == 0 {
			_, err := g.AddEdge(g.SourceNode(), -1, n, -1)
			if err != nil {
				return fmt.Errorf("failed to add a source sentinel edge: %v", err)
			}
		}
		if len(n.outEdges) == 0 {
			_, err := g.AddEdge(n, -1, g.SinkNode(), -1)
			if err != nil {
				return fmt.Errorf("failed to add a sink sentinel edge: %v", err)
			}
		}
	}
	return nil
}
