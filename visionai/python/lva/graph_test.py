# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Tests for the graph library."""

from google.protobuf import text_format

from visionai.python.protos.googleapis.v1 import lva_pb2
from visionai.python.testing import googletest
from visionai.python.lva import graph


class GraphTest(googletest.TestCase):

  def test_add_nodes(self):
    g = graph.Graph()
    g.add_node(graph.Node("a", "a"))
    self.assertRaises(ValueError, g.add_node, graph.Node("a", "a"))

  def test_add_edges(self):
    g = graph.Graph()
    self.assertRaises(
        ValueError,
        g.add_edge,
        graph.Port("src", "src-out"),
        graph.Port("dst", "dst-in"),
    )
    g.add_node(graph.Node("src", "SRC"))
    self.assertRaises(
        ValueError,
        g.add_edge,
        graph.Port("src", "src-out"),
        graph.Port("dst", "dst-in"),
    )
    g.add_node(graph.Node("dst", "DST"))
    g.add_edge(graph.Port("src", "src-out"), graph.Port("dst", "dst-in"))

  def test_simple_graph(self):
    g = graph.Graph()
    a1 = graph.Node("a1", "MyOp", output_ports=["output"])
    a2 = graph.Node("a2", "MyOp", input_ports=["input"])
    g.add_node(a1)
    g.add_node(a2)
    g.add_edge(a1.outputs()[0], a2.inputs()[0])
    adef = g.get_analysis_definition()
    self.assertEqual(
        adef,
        text_format.Parse(
            """analyzers {
  analyzer: "a2"
  operator: "MyOp"
  inputs {
    input: "a1:output"
  }
}
analyzers {
  analyzer: "a1"
  operator: "MyOp"
}""",
            lva_pb2.AnalysisDefinition(),
        ),
    )


if __name__ == "__main__":
  googletest.main()
