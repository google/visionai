# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Library for working with LVA graphs."""

import queue
from typing import Any, Dict, List, Tuple
from visionai.python.gapic.visionai import visionai_v1


class Port:
  """Port objects abstract the port in the LVA graph."""

  def __init__(self, node: str, name: str):
    self.node = node
    self.name = name


class Node:
  """Node objects abstract the vertex in the LVA graph.

  Example:
    n = node("a1", "MyOp")
  """

  def __init__(
      self,
      name: str,
      operator: str,
      input_ports: List[str] = None,
      output_ports: List[str] = None,
      attributes: Dict[str, Any] = None,
  ):
    """Initializes a node.

    Args:
      name: A name that uniquely identifies this node within this graph.
      operator: The Operator name that this node represents.
      input_ports: The input ports that incoming edges can connect to.
      output_ports: The output ports that outgoing edges can start from.
      attributes: The attributes of the node.
    """
    self.name = name
    self.operator = operator
    self.input_ports = input_ports
    self.output_ports = output_ports
    self.attributes = {}
    if attributes is not None:
      for k, v in attributes.items():
        self.attributes[k] = _get_attribute_value(v)

  def inputs(self) -> Tuple[Port, ...]:
    return tuple(map(lambda x: Port(self.name, x), self.input_ports))

  def outputs(self) -> Tuple[Port, ...]:
    return tuple(map(lambda x: Port(self.name, x), self.output_ports))


class Edge:
  """Edge objects abstract the edge in the LVA graph."""

  def __init__(self, src_node: str, src_out: str, dest_node: str, dest_in: str):
    """Initializes an edge.

    Args:
      src_node: The name of the source node.
      src_out: The name of an output of src_node.
      dest_node: The name of the destination node.
      dest_in: The name of the input of dest_node.
    """
    self.src_node = src_node
    self.dest_node = dest_node
    self.src_out = src_out
    self.dest_in = dest_in


class Graph:
  """Graph objects abstract an LVA AnalysisDefinition.

  Graph objects represents an LVA AnalysisDefinition, and it also supplies
  methods that allow them to build it intuitively.

  Example:
  ```
    g = graph.Graph()
    a1 = graph.Node("a1", "MyOp", output_ports=["output"])
    a2 = graph.Node("a2", "MyOp", input_ports=["input"])
    g.add_node(a1)
    g.add_node(a2)
    g.add_edge(a1.outputs()[0], a2.inputs()[0])
    adef = g.get_analysis_definition()
  ```
  """

  def __init__(self):
    """Initializes a graph."""
    self._nodes: Dict[str, Node] = {}
    self._edges_in: Dict[str, List[Edge]] = {}
    self._edges_out: Dict[str, List[Edge]] = {}
    self._out_degrees: Dict[str, int] = {}
    return

  def add_node(self, node: Node) -> Node:
    """Adds a node to the graph.

    Args:
      node: The node in the graph.

    Returns:
      The added node.
    """
    if node.name in self._nodes:
      raise ValueError('node `{}` has already been added'.format(node.name))
    self._nodes[node.name] = node
    return node

  def add_edge(self, src_port: Port, dest_port: Port):
    """Adds an edge from src_port to dest_port."""
    self._add_edge(
        src_node=src_port.node,
        src_out=src_port.name,
        dest_node=dest_port.node,
        dest_in=dest_port.name,
    )

  def _add_edge(
      self,
      src_node: str,
      src_out: str,
      dest_node: str,
      dest_in: str,
  ):
    """Adds an edge from an output to an input between two nodes.

    This method connects the src_out output of node src_node to the dest_in
    input of node dest_node.

    Args:
      src_node: The name of the source node.
      src_out: The name of an output of src_node. It must be a name of one of
        the output arguments of the Operator that src_node represents.
      dest_node: The name of the destination node.
      dest_in: The name of an input of dest_node. It must be a name of one of
        the input arguments of the Operator that dest_node represents.
    """
    if src_node not in self._nodes:
      raise ValueError(
          'source node `{}` does not exist in the graph'.format(src_node)
      )
    if dest_node not in self._nodes:
      raise ValueError(
          'destination node `{}` does not exist in the graph'.format(dest_node)
      )
    if src_node not in self._edges_out:
      self._edges_out[src_node] = []
    if dest_node not in self._edges_in:
      self._edges_in[dest_node] = []
    if src_node not in self._out_degrees:
      self._out_degrees[src_node] = 0
    self._edges_in[dest_node].append(
        Edge(src_node, src_out, dest_node, dest_in)
    )
    self._edges_out[src_node].append(
        Edge(src_node, src_out, dest_node, dest_in)
    )
    self._out_degrees[src_node] += 1

  def get_analysis_definition(self) -> visionai_v1.AnalysisDefinition:
    """Get the analysis definition from the graph."""
    analyzers: List[visionai_v1.AnalyzerDefinition] = []
    q: queue.Queue[Node] = queue.Queue()
    out_degrees = self._out_degrees
    for node in self._nodes.values():
      if node.name not in out_degrees or out_degrees[node.name] == 0:
        q.put(node)
    while not q.empty():
      node = q.get()
      analyzer = visionai_v1.AnalyzerDefinition(
          analyzer=node.name,
          operator=node.operator,
          attrs=node.attributes,
      )
      if node.name in self._edges_in:
        for edge in self._edges_in[node.name]:
          analyzer.inputs.append(
              visionai_v1.AnalyzerDefinition.StreamInput(
                  input=f'{edge.src_node}:{edge.src_out}'
              )
          )
          out_degrees[edge.src_node] -= 1
          if out_degrees[edge.src_node] == 0:
            q.put(self._nodes[edge.src_node])
      analyzers.append(analyzer)
    return visionai_v1.AnalysisDefinition(analyzers=analyzers)


def _get_attribute_value(val: Any) -> visionai_v1.AttributeValue:
  """Converts the value with python Any type to the AttributeValue.

  Args:
    val: The value with any type.

  Returns:
    The attribute value.
  """
  attr = visionai_v1.AttributeValue()
  if isinstance(val, str):
    attr.s = str(val).encode('utf-8')
  elif isinstance(val, bool):
    attr.b = val
  elif isinstance(val, float):
    attr.f = val
  elif isinstance(val, int):
    attr.i = val
  else:
    raise ValueError('type {} is not supported'.format(type(val)))
  return attr
