# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Tests for the graph library."""

from google.protobuf import text_format

from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
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

  def test_unsupported_attributes(self):
    self.assertRaises(
        ValueError,
        graph.Node,
        "a2",
        "MyOp",
        input_ports=["input"],
        attributes={
            "key1": {},
        },
    )

  def test_simple_graph(self):
    g = graph.Graph()
    a1 = graph.Node("a1", "MyOp", output_ports=["output"])
    a2 = graph.Node(
        "a2",
        "MyOp",
        input_ports=["input"],
        attributes={
            "key1": "string value",
            "key2": 1,
            "key3": 3.14,
            "key4": True,
        },
    )
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
  attrs: {
      key: "key1",
      value: {
        s: "string value"
      }
  }
  attrs: {
      key: "key2",
      value: {
        i: 1
      }
  }
  attrs: {
      key: "key3",
      value: {
        f: 3.14
      }
  }
  attrs: {
      key: "key4",
      value: {
        b: True
      }
  }
}
analyzers {
  analyzer: "a1"
  operator: "MyOp"
}""",
            visionai_v1.AnalysisDefinition().__dict__["_pb"],
        ),
    )


if __name__ == "__main__":
  googletest.main()
