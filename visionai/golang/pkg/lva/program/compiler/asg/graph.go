// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

import (
	"fmt"
)

// Graph represents the connectivity of an ASG.
//
// It is a directed multigraph. Nodes have input/output indices
// ("ports") and Edges must connect from a specific output index
// of one Node to a specific input index of another.
//
// It also has two sentinel nodes, "Source" and "Sink", that
// may be used to connect to nodes that have no in edges and
// no out edges, respectively. These special Nodes may only be
// connected through Edges that leave/enter from/to the sentinel
// input/output index -1.
//
// Note that this strictly represents connectivity and does not
// attempt to verify whether the Graph confroms to any notion
// of what constitutes a "well-formed" ASG or one that has real
// meaning. For those, see verifiers.go and further downstream
// compilation phases.
type Graph struct {
	// nodes holds references to the set of all Nodes in the Graph.
	//
	// A Node with id=i is in the Graph if nodes[i] holds a reference
	// to it. It is not present in the Graph if either nodes[i] == nil
	// or if len(nodes) <= i.
	nodes []*Node

	// edges holds references to the set of all Edges in the Graph.
	//
	// An edge with id=i is in the Graph if edges[i] holds a reference
	// to it. It is not present in the Graph if either edges[i] == nil
	// or if len(edges) <= i.
	edges []*Edge
}

// Node is a node in a Graph.
//
// Use Graph.AddNode to create new Node instances.
//
// A node allows incoming (outgoing) edges to attach to
// input (output) indices. Schematically:
//
//     -----
// -->|0   3|-->
// -->|1   1|-->
//     -----
//
// One may also refer to input (output) indices colloquially
// as input (output) "ports".
//
// While these indices may not have much special meaning
// strictly in the context of connectivities, they are used
// in the compiler to decide which argument of an operator
// actually has a streams flowing in and out.
type Node struct {
	// id is the numeric identifier of this Node within the Graph.
	id int

	// inEdges holds references to incoming Edges to this Node.
	// The order is not significant.
	inEdges []*Edge

	// outEdges holds references to outgoing Edges from this Node.
	// The order is not significant.
	outEdges []*Edge

	// element is the kind of syntactic entity this Node represents
	// in the ASG. All non-connectivity related info can be found here.
	element Element
}

// Special ids assigned to the sentinel Nodes.
const (
	sentinelSourceID = 0
	sentinelSinkID   = 1
)

// IsSource returns true if this Node is the sentinel Source Node.
func (n *Node) IsSource() bool {
	return n.id == sentinelSourceID
}

// IsSink returns true if this Node is the sentinel Sink Node.
func (n *Node) IsSink() bool {
	return n.id == sentinelSinkID
}

// Name returns the name, resolved in the following order:
// + If the n.element is set, return the result of its Name.
// + Return the node id as a string.
func (n *Node) Name() string {
	if n.element != nil {
		return n.element.Name()
	}
	return fmt.Sprintf("node-%d", n.id)
}

// InEdges returns inEdges.
func (n *Node) InEdges() []*Edge {
	return n.inEdges
}

// OutEdges returns outEdges.
func (n *Node) OutEdges() []*Edge {
	return n.outEdges
}

// Element returns element.
func (n *Node) Element() Element {
	return n.element
}

// SetElement sets element.
func (n *Node) SetElement(e Element) {
	n.element = e
}

// Edge is an edge in a graph.
//
// Use Graph.AddEdge to create new Edge instances.
//
// An edge specifies the node and output index that it is leaving
// from and the node and input index that it is entering into.
type Edge struct {
	// id is the numeric identifier of this Edge within the Graph.
	id int

	// src is the Node from which this Edge is leaving.
	src *Node

	// soutIdx is output index (port) from which this Edge is leaving src.
	soutIdx int

	// dst is the Node from which this Edge is entering.
	dst *Node

	// dinIdx is input index (port) to which this Edge is entering dst.
	dinIdx int
}

// Src returns src.
func (e *Edge) Src() *Node {
	return e.src
}

// SoutIdx returns soutIdx.
func (e *Edge) SoutIdx() int {
	return e.soutIdx
}

// Dst returns dst.
func (e *Edge) Dst() *Node {
	return e.dst
}

// DinIdx returns dinIdx.
func (e *Edge) DinIdx() int {
	return e.dinIdx
}

// NewGraph creates and initializes a Graph that is ready to be connected.
//
// The returned graph will contain only the two special
// "Source" and "Sink" nodes and no edges.
func NewGraph() (*Graph, error) {
	g := &Graph{}

	// Add the sentinel Source node. This must be the first Node.
	sourceSentinel, err := g.AddNode()
	if err != nil {
		return nil, fmt.Errorf("internal error: %v", err)
	}
	sourceSentinel.element = &SentinelElement{
		Info: &SentinelInfo{
			Name: "Source",
		},
	}

	// Add the sentinel Sink node. This must be the second Node.
	sinkSentinel, err := g.AddNode()
	if err != nil {
		return nil, err
	}
	sinkSentinel.element = &SentinelElement{
		Info: &SentinelInfo{
			Name: "Sink",
		},
	}

	return g, nil
}

// AddNode creates and adds an isolated new node in the graph.
func (g *Graph) AddNode() (*Node, error) {
	// We do not attempt to reuse lower ids that may be available.
	n := &Node{
		id: len(g.nodes),
	}
	g.nodes = append(g.nodes, n)
	return n, nil
}

// AddEdge creates and adds an edge from node src to node dst, leaving
// src at the index soutIdx and arriving dst at the index dinIdx.
func (g *Graph) AddEdge(src *Node, soutIdx int, dst *Node, dinIdx int) (*Edge, error) {
	if src == nil {
		err := fmt.Errorf("given a nil source *Node")
		return nil, err
	}
	if dst == nil {
		err := fmt.Errorf("given a nil destination *Node")
		return nil, err
	}

	// We do not attempt to reuse lower ids that may be available.
	e := &Edge{
		id:      len(g.edges),
		src:     src,
		soutIdx: soutIdx,
		dst:     dst,
		dinIdx:  dinIdx,
	}
	g.edges = append(g.edges, e)
	src.outEdges = append(src.outEdges, e)
	dst.inEdges = append(dst.inEdges, e)
	return e, nil
}

// It is the caller's responsibility to ensure that edgeList has non-zero
// length and that idx < len(edgeList)
func removeFromUnorderedSlice(edgeList []*Edge, idx int) []*Edge {
	lastIdx := len(edgeList) - 1
	edgeList[lastIdx], edgeList[idx] = edgeList[idx], edgeList[lastIdx]
	edgeList = edgeList[:lastIdx]
	return edgeList
}

// RemoveEdge removes the given Edge from the Graph.
func (g *Graph) RemoveEdge(edge *Edge) error {
	// Remove from the src node's outEdges.
	src := edge.src
	srcOutEdgeIdx := -1
	for i, e := range src.outEdges {
		if e == edge {
			srcOutEdgeIdx = i
		}
	}
	if srcOutEdgeIdx < 0 {
		return fmt.Errorf("the given Edge is not among the outEdges of its src Node")
	}
	src.outEdges = removeFromUnorderedSlice(src.outEdges, srcOutEdgeIdx)

	// Remove from the dst node's inEdges.
	dst := edge.dst
	dstInEdgeIdx := -1
	for i, e := range dst.inEdges {
		if e == edge {
			dstInEdgeIdx = i
		}
	}
	if dstInEdgeIdx < 0 {
		return fmt.Errorf("the given Edge is not among the inEdges of its dst Node")
	}
	dst.inEdges = removeFromUnorderedSlice(dst.inEdges, dstInEdgeIdx)

	// Remove from the graph.
	g.edges[edge.id] = nil
	return nil
}

// SourceNode returns the sentinel Source Node.
func (g *Graph) SourceNode() *Node {
	return g.nodes[sentinelSourceID]
}

// SinkNode returns the sentinel Sink Node.
func (g *Graph) SinkNode() *Node {
	return g.nodes[sentinelSinkID]
}

// NumNodes returns the number of Nodes in the graph.
func (g *Graph) NumNodes() int {
	numNodes := 0
	for _, n := range g.nodes {
		if n != nil {
			numNodes++
		}
	}
	return numNodes
}

// Nodes returns a slice containing pointers to nodes currently
// present in the Graph; i.e. deleted Nodes are omitted.
func (g *Graph) Nodes() []*Node {
	present := []*Node{}
	for _, n := range g.nodes {
		if n == nil {
			continue
		}
		present = append(present, n)
	}
	return present
}

// NumEdges returns the number of Edges in the graph.
func (g *Graph) NumEdges() int {
	numEdges := 0
	for _, e := range g.edges {
		if e != nil {
			numEdges++
		}
	}
	return numEdges
}
