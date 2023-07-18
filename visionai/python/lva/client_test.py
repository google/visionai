# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Test for client library."""

import datetime
from unittest import mock
from google.api_core import operation
from google.api_core.exceptions import InternalServerError
from google.api_core.exceptions import NotFound
from google.auth import credentials as ga_credentials
from google.longrunning import operations_pb2
from google.protobuf import any_pb2
from google.protobuf import empty_pb2
from google.protobuf import timestamp_pb2
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.lva import client
from visionai.python.lva import graph
from visionai.python.net import channel


_ANALYSES = [
    visionai_v1.Analysis(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-1",
        disable_event_watch=True,
        create_time=timestamp_pb2.Timestamp(seconds=1),
    ),
    visionai_v1.Analysis(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-2",
        disable_event_watch=False,
        create_time=timestamp_pb2.Timestamp(seconds=10),
    ),
]

_PROCESSES = [
    visionai_v1.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process-1",
        create_time=timestamp_pb2.Timestamp(seconds=1),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-1",
        attribute_overrides=["k1=v1", "k2=v2"],
        run_status=visionai_v1.RunStatus(
            state=visionai_v1.RunStatus.State.RUNNING,
        ),
        run_mode=visionai_v1.RunMode.SUBMISSION,
    ),
    visionai_v1.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process-2",
        create_time=timestamp_pb2.Timestamp(seconds=10),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-2",
        attribute_overrides=["k1=v1", "k3=v3"],
        run_status=visionai_v1.RunStatus(
            state=visionai_v1.RunStatus.State.RUNNING,
        ),
        run_mode=visionai_v1.RunMode.SUBMISSION,
    ),
    visionai_v1.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process-3",
        create_time=timestamp_pb2.Timestamp(seconds=10),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-3",
        attribute_overrides=["k1=v1", "k3=v3"],
        run_status=visionai_v1.RunStatus(
            state=visionai_v1.RunStatus.State.COMPLETED,
        ),
        run_mode=visionai_v1.RunMode.SUBMISSION,
    ),
]


class ClientTest(googletest.TestCase):

  def setUp(self):
    super().setUp()
    self._lva_client = visionai_v1.LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="grpc",
    )
    self._connection_options = channel.ConnectionOptions(
        project_id="test-project",
        location_id="us-central1",
        cluster_id="test-cluster",
    )
    self._parent = "projects/{}/locations/{}/clusters/{}".format(
        self._connection_options.project_id,
        self._connection_options.location_id,
        self._connection_options.cluster_id,
    )
    self._test_graph = graph.Graph()
    self._test_graph.add_node(graph.Node("a1", "MyOp"))
    self._test_graph.add_node(graph.Node("a2", "MyOp"))
    self._test_graph.add_edge(
        graph.Port("a1", "output"), graph.Port("a2", "input")
    )
    self._mock_create_lva_client = self.enter_context(
        mock.patch.object(client, "_create_lva_client", autospec=True)
    )
    self._mock_create_lva_client.return_value = self._lva_client

  def test_list_analyses_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.list_analyses), "__call__"
    ) as call:
      call.return_value = visionai_v1.ListAnalysesResponse(analyses=_ANALYSES)
      response = client.list_analyses(self._connection_options)
      self.assertEqual(len(response), len(_ANALYSES))
      _ = [
          self.assertEqual(got, client.Analysis(want))
          for (got, want) in zip(response, _ANALYSES)
      ]

  def test_list_analyses_not_found(self):
    with mock.patch.object(
        type(self._lva_client.transport.list_analyses), "__call__"
    ) as call:
      call.side_effect = NotFound("not found")
      self.assertRaises(
          NotFound, client.list_analyses, self._connection_options
      )

  def test_create_analysis_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.create_analysis), "__call__"
    ) as call:
      resp = any_pb2.Any()
      resp.Pack(_ANALYSES[0].__dict__["_pb"])
      call.return_value = operations_pb2.Operation(
          done=True,
          name="test-operation",
          response=resp,
      )
      self.assertEqual(
          client.create_analysis(
              self._connection_options,
              self._test_graph,
              "test-analysis",
          ),
          client.Analysis(_ANALYSES[0]),
      )

  def test_create_analysis_invalid_argument(self):
    self.assertRaises(
        ValueError,
        client.create_analysis,
        self._connection_options,
        self._test_graph,
    )
    self.assertRaises(ValueError, client.create_analysis, None, "test-analysis")

  def test_create_analysis_lva_service_failure(self):
    with mock.patch.object(
        type(self._lva_client.transport.create_analysis), "__call__"
    ) as call:
      call.side_effect = InternalServerError("internal error")
      self.assertRaises(
          InternalServerError,
          client.create_analysis,
          self._connection_options,
          self._test_graph,
          "test-analysis",
      )

  def test_get_analysis_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_analysis), "__call__"
    ) as call:
      call.return_value = _ANALYSES[0]
      self.assertEqual(
          client.get_analysis(self._connection_options, "test-analysis"),
          client.Analysis(_ANALYSES[0]),
      )

  def test_get_analysis_not_found(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_analysis), "__call__"
    ) as call:
      call.side_effect = NotFound("not found")
      self.assertRaises(
          NotFound,
          client.get_analysis,
          self._connection_options,
          "test-analysis",
      )

  def test_delete_analysis_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.delete_analysis), "__call__"
    ) as call:
      resp = any_pb2.Any()
      resp.Pack(empty_pb2.Empty())
      call.return_value = operations_pb2.Operation(
          done=True,
          name="test-operation",
          response=resp,
      )
      self.assertIsNone(
          client.delete_analysis(
              self._connection_options,
              "test-analysis",
          )
      )

  def test_delete_analysis_lva_service_failure(self):
    with mock.patch.object(
        type(self._lva_client.transport.delete_analysis), "__call__"
    ) as call:
      call.side_effect = InternalServerError("internal error")
      self.assertRaises(
          InternalServerError,
          client.delete_analysis,
          self._connection_options,
          "test-analysis",
      )

  def test_get_or_create_analysis_get_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_analysis), "__call__"
    ) as call:
      call.return_value = _ANALYSES[0]
      self.assertEqual(
          client.get_or_create_analysis(
              self._connection_options,
              self._test_graph,
              "test-analysis",
          ),
          client.Analysis(_ANALYSES[0]),
      )

  def test_get_or_create_analysis_get_failure(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_analysis), "__call__"
    ) as call:
      call.side_effect = InternalServerError("internal error")
      self.assertRaises(
          InternalServerError,
          client.get_or_create_analysis,
          self._connection_options,
          self._test_graph,
          "test-analysis",
      )

  def test_get_or_create_analysis_create_successfully(self):
    resp = any_pb2.Any()
    resp.Pack(_ANALYSES[0].__dict__["_pb"])
    # pyformat: disable
    with mock.patch.object(
        self._lva_client,
        "get_analysis",
        side_effect=NotFound("not found"),
    ), mock.patch.object(
        self._lva_client,
        "create_analysis",
        return_value=operation.Operation(
            operation=operations_pb2.Operation(
                done=True,
                name="test-operation",
                response=resp,
            ),
            refresh=None,
            cancel=None,
            result_type=visionai_v1.Analysis,
        ),
    ):
    # pyformat: enable
      self.assertEqual(
          client.get_or_create_analysis(
              self._connection_options, self._test_graph, "test-analysis"
          ),
          client.Analysis(_ANALYSES[0]),
      )

  def test_get_or_create_analysis_create_failure(self):
    # pyformat: disable
    with mock.patch.object(
        self._lva_client,
        "get_analysis",
        side_effect=NotFound("not found"),
    ), mock.patch.object(
        self._lva_client,
        "create_analysis",
        side_effect=InternalServerError("internal error"),
    ):
    # pyformat: enable
      self.assertRaises(
          InternalServerError,
          client.get_or_create_analysis,
          self._connection_options,
          self._test_graph,
          "test-analysis",
      )

  def test_wait_process_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.return_value = _PROCESSES[2]
      process = client.Process(_PROCESSES[2])
      self.assertEqual(
          process.wait(
              self._connection_options,
          ),
          process,
      )

  def test_wait_process_timeout(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.return_value = _PROCESSES[1]
      self.assertRaises(
          TimeoutError,
          client.Process(_PROCESSES[1]).wait,
          self._connection_options,
          datetime.timedelta(seconds=1),
      )

  def test_get_process_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.return_value = _PROCESSES[0]
      self.assertEqual(
          client.get_process(self._connection_options, "test-process"),
          client.Process(_PROCESSES[0]),
      )

  def test_get_process_not_found(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.side_effect = NotFound("not found")
      self.assertRaises(
          NotFound, client.get_process, self._connection_options, "test-process"
      )

  def test_list_processes_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.list_processes), "__call__"
    ) as call:
      call.return_value = visionai_v1.ListProcessesResponse(
          processes=_PROCESSES
      )
      response = client.list_processes(self._connection_options)
      self.assertEqual(len(response), len(_PROCESSES))
      _ = [
          self.assertEqual(got, client.Process(want))
          for (got, want) in zip(response, _PROCESSES)
      ]

  def test_list_processes_with_filter_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.list_processes), "__call__"
    ) as call:
      call.return_value = visionai_v1.ListProcessesResponse(
          processes=_PROCESSES
      )
      response = client.list_processes(
          self._connection_options, list_filter='run_status.state="RUNING"'
      )
      self.assertEqual(len(response), len(_PROCESSES))
      _ = [
          self.assertEqual(got, client.Process(want))
          for (got, want) in zip(response, _PROCESSES)
      ]

  def test_list_processes_not_found(self):
    with mock.patch.object(
        type(self._lva_client.transport.list_processes), "__call__"
    ) as call:
      call.side_effect = NotFound("not found")
      self.assertRaises(
          NotFound, client.list_processes, self._connection_options
      )

  def test_delete_process_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.delete_process), "__call__"
    ) as call:
      resp = any_pb2.Any()
      resp.Pack(empty_pb2.Empty())
      call.return_value = operations_pb2.Operation(
          done=True,
          name="test-operation",
          response=resp,
      )
      self.assertIsNone(
          client.delete_process(
              self._connection_options,
              "test-process",
          )
      )

  def test_delete_process_lva_service_failure(self):
    with mock.patch.object(
        type(self._lva_client.transport.delete_process), "__call__"
    ) as call:
      call.side_effect = InternalServerError("internal error")
      self.assertRaises(
          InternalServerError,
          client.delete_process,
          self._connection_options,
          "test-process",
      )

  def test_create_process_successfully(self):
    with mock.patch.object(
        type(self._lva_client.transport.create_process), "__call__"
    ) as call:
      resp = any_pb2.Any()
      resp.Pack(_PROCESSES[0].__dict__["_pb"])
      call.return_value = operations_pb2.Operation(
          done=True,
          name="test-operation",
          response=resp,
      )
      self.assertEqual(
          client.create_process(
              self._connection_options,
              "test-analysis",
              {"k1": "v1", "k2": "v2"},
              visionai_v1.RunMode.SUBMISSION,
              3,
              "",
          ),
          client.Process(_PROCESSES[0]),
      )

  def test_create_process_lva_service_failure(self):
    with mock.patch.object(
        type(self._lva_client.transport.create_process), "__call__"
    ) as call:
      call.side_effect = InternalServerError("internal error")
      self.assertRaises(
          InternalServerError,
          client.create_process,
          self._connection_options,
          "test-analysis",
          {"k1": "v1", "k2": "v2"},
          visionai_v1.RunMode.SUBMISSION,
          3,
          "",
      )


if __name__ == "__main__":
  googletest.main()
