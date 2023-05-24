# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Test for client library."""

from concurrent import futures
from unittest import mock
from datetime import datetime, timedelta

import grpc
import grpc_testing
from google3.google.protobuf import timestamp_pb2
from visionai.python.protos.googleapis.v1 import lva_resources_pb2
from visionai.python.protos.googleapis.v1 import lva_service_pb2
from visionai.python.protos.googleapis.v1 import lva_pb2
from google.longrunning import operations_pb2
from google3.google.protobuf import any_pb2
from visionai.python.testing import googletest
from visionai.python.lva import client
from visionai.python.lva import graph
from visionai.python.net import channel


def _get_descriptor(pb2, service, method):
  return pb2.DESCRIPTOR.services_by_name[service].methods_by_name[method]


_ANALYSES = [
    lva_resources_pb2.Analysis(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-1",
        disable_event_watch=True,
        create_time=timestamp_pb2.Timestamp(seconds=1),
    ),
    lva_resources_pb2.Analysis(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-2",
        disable_event_watch=False,
        create_time=timestamp_pb2.Timestamp(seconds=10),
    ),
]

_PROCESSES = [
    lva_resources_pb2.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process-1",
        create_time=timestamp_pb2.Timestamp(seconds=1),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-1",
        attribute_overrides=["k1=v1", "k2=v2"],
        run_status=lva_pb2.RunStatus(
            state=lva_pb2.RunStatus.State.RUNNING,
        ),
        run_mode=lva_pb2.RunMode.SUBMISSION,
    ),
    lva_resources_pb2.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process-2",
        create_time=timestamp_pb2.Timestamp(seconds=10),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-2",
        attribute_overrides=["k1=v1", "k3=v3"],
        run_status=lva_pb2.RunStatus(
            state=lva_pb2.RunStatus.State.RUNNING,
        ),
        run_mode=lva_pb2.RunMode.SUBMISSION,
    ),
    lva_resources_pb2.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process-3",
        create_time=timestamp_pb2.Timestamp(seconds=10),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis-3",
        attribute_overrides=["k1=v1", "k3=v3"],
        run_status=lva_pb2.RunStatus(
            state=lva_pb2.RunStatus.State.COMPLETED,
        ),
        run_mode=lva_pb2.RunMode.SUBMISSION,
    ),
]


class ClientTest(googletest.TestCase):

  def assertAnalysisEqual(
      self, a1: client.Analysis, a2: client.Analysis, msg=None
  ):
    if (
        a1.analysis_name != a2.analysis_name
        or a1.analysis_id != a2.analysis_id
        or a1.create_time != a2.create_time
        or a1.update_time != a2.update_time
        or a1.labels != a2.labels
        or a1.analysis_definition != a2.analysis_definition
        or a1.input_streams_mapping != a2.input_streams_mapping
        or a1.output_streams_mapping != a2.output_streams_mapping
        or a1.disable_event_watch != a2.disable_event_watch
    ):
      raise self.failureException(
          msg
          or "Analysis %s and %s not equal" % (a1.analysis_id, a2.analysis_id)
      )

  def assertProcessEqual(
      self, p1: client.Process, p2: client.Process, msg=None
  ):
    if (
        p1.process_name != p2.process_name
        or p1.process_id != p2.process_id
        or p1.analysis_name != p2.analysis_name
        or p1.create_time != p2.create_time
        or p1.update_time != p2.update_time
        or p1.run_status_state != p2.run_status_state
        or p1.run_mode != p2.run_mode
        or p1.attribute_overrides != p2.attribute_overrides
        or p1.event_id != p2.event_id
        or p1.batch_id != p2.batch_id
        or p1.retry_count != p2.retry_count
    ):
      raise self.failureException(
          msg or "Process %s and %s not equal" % (p1.process_id, p2.process_id)
      )

  def setUp(self):
    super().setUp()
    self.addTypeEqualityFunc(client.Analysis, self.assertAnalysisEqual)
    self.addTypeEqualityFunc(client.Process, self.assertProcessEqual)

    self._thread_pool = futures.ThreadPoolExecutor(max_workers=1)
    self._channel = grpc_testing.channel(
        lva_service_pb2.DESCRIPTOR.services_by_name.values(),
        grpc_testing.strict_real_time(),
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
    self._mock_create_channel = self.enter_context(
        mock.patch.object(channel, "create_channel", autospec=True)
    )

  def create_channel(self):
    return self._channel

  def tearDown(self):
    super().tearDown()
    self._thread_pool.shutdown(wait=False)

  def test_list_analyses_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.list_analyses,
        self._connection_options,
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "ListAnalyses")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.ListAnalysesRequest(
            parent=self._parent,
        ),
        actual_request,
    )
    rpc.terminate(
        lva_service_pb2.ListAnalysesResponse(analyses=_ANALYSES),
        (),
        grpc.StatusCode.OK,
        "",
    )
    got_analyses = application_future.result()
    want_analyses = list(map(client._resource_to_analysis, _ANALYSES))

    if len(got_analyses) != len(want_analyses):
      raise self.failureException("Lists have different length")
    [
        self.assertEqual(got, want)
        for (got, want) in zip(got_analyses, want_analyses)
    ]

  def test_list_analyses_not_found(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.list_analyses,
        self._connection_options,
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "ListAnalyses")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.ListAnalysesRequest(
            parent=self._parent,
        ),
        actual_request,
    )
    rpc.terminate(
        lva_service_pb2.ListAnalysesResponse(),
        (),
        grpc.StatusCode.NOT_FOUND,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_create_analysis_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateAnalysis")
    )
    rpc.send_initial_metadata(())
    want_request = lva_service_pb2.CreateAnalysisRequest(
        parent=self._parent,
        analysis_id="test-analysis",
        analysis=lva_resources_pb2.Analysis(
            analysis_definition=self._test_graph.get_analysis_definition(),
            disable_event_watch=True,
        ),
    )
    want_request.analysis.analysis_definition.analyzers.sort(
        key=lambda a: a.analyzer
    )
    actual_request.analysis.analysis_definition.analyzers.sort(
        key=lambda a: a.analyzer
    )
    self.assertEqual(
        want_request.analysis.analysis_definition,
        actual_request.analysis.analysis_definition,
    )
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    resp = any_pb2.Any()
    resp.Pack(_ANALYSES[0])
    lro_rpc.terminate(
        operations_pb2.Operation(done=True, response=resp),
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertEqual(
        application_future.result(), client._resource_to_analysis(_ANALYSES[0])
    )

  def test_create_analysis_invalid_argument(self):
    self._mock_create_channel.return_value = self.create_channel()
    self.assertRaises(
        ValueError,
        client.create_analysis,
        self._connection_options,
        self._test_graph,
    )
    self.assertRaises(ValueError, client.create_analysis, None, "test-analysis")

  def test_create_analysis_lva_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateAnalysis")
    )
    rpc.terminate(
        operations_pb2.Operation(),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_create_analysis_lro_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateAnalysis")
    )
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    lro_rpc.terminate(
        operations_pb2.Operation(),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_get_analysis_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_analysis,
        self._connection_options,
        "test-analysis",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetAnalysis")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetAnalysisRequest(
            name="{}/analyses/{}".format(self._parent, "test-analysis")
        ),
        actual_request,
    )
    rpc.terminate(
        _ANALYSES[0],
        (),
        grpc.StatusCode.OK,
        "",
    )
    analysis = client._resource_to_analysis(_ANALYSES[0])
    analysis.analysis_id = "test-analysis-1"
    self.assertEqual(application_future.result(), analysis)

  def test_get_analysis_not_found(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_analysis,
        self._connection_options,
        "test-analysis",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetAnalysis")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetAnalysisRequest(
            name="{}/analyses/{}".format(self._parent, "test-analysis")
        ),
        actual_request,
    )
    rpc.terminate(
        _ANALYSES[0],
        (),
        grpc.StatusCode.NOT_FOUND,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_delete_analysis_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.delete_analysis,
        self._connection_options,
        "test-analysis",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "DeleteAnalysis")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.DeleteAnalysisRequest(
            name="{}/analyses/{}".format(self._parent, "test-analysis")
        ),
        actual_request,
    )
    rpc.terminate(
        operations_pb2.Operation(name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    lro_rpc.terminate(
        operations_pb2.Operation(done=True),
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertIsNone(application_future.result())

  def test_delete_analysis_lva_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.delete_analysis,
        self._connection_options,
        "test-analysis",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "DeleteAnalysis")
    )
    rpc.terminate(
        operations_pb2.Operation(),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_delete_analysis_lro_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.delete_analysis,
        self._connection_options,
        "test-analysis",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "DeleteAnalysis")
    )
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    lro_rpc.terminate(
        operations_pb2.Operation(),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_get_or_create_analysis_get_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_or_create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetAnalysis")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetAnalysisRequest(
            name="{}/analyses/{}".format(self._parent, "test-analysis")
        ),
        actual_request,
    )
    rpc.terminate(
        _ANALYSES[0],
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertEqual(
        application_future.result(), client._resource_to_analysis(_ANALYSES[0])
    )

  def test_get_or_create_analysis_get_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_or_create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetAnalysis")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetAnalysisRequest(
            name="{}/analyses/{}".format(self._parent, "test-analysis")
        ),
        actual_request,
    )
    rpc.terminate(
        _ANALYSES[0],
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_get_or_create_analysis_create_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_or_create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
        True,
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetAnalysis")
    )
    rpc.send_initial_metadata(())
    rpc.terminate(
        _ANALYSES[0],
        (),
        grpc.StatusCode.NOT_FOUND,
        "",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateAnalysis")
    )
    rpc.send_initial_metadata(())
    want_request = lva_service_pb2.CreateAnalysisRequest(
        parent=self._parent,
        analysis_id="test-analysis",
        analysis=lva_resources_pb2.Analysis(
            analysis_definition=self._test_graph.get_analysis_definition(),
            disable_event_watch=True,
        ),
    )
    want_request.analysis.analysis_definition.analyzers.sort(
        key=lambda a: a.analyzer
    )
    actual_request.analysis.analysis_definition.analyzers.sort(
        key=lambda a: a.analyzer
    )
    self.assertEqual(
        want_request.analysis.analysis_definition,
        actual_request.analysis.analysis_definition,
    )
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    resp = any_pb2.Any()
    resp.Pack(_ANALYSES[0])
    lro_rpc.terminate(
        operations_pb2.Operation(done=True, response=resp),
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertEqual(
        application_future.result(), client._resource_to_analysis(_ANALYSES[0])
    )

  def test_get_or_create_analysis_create_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_or_create_analysis,
        self._connection_options,
        self._test_graph,
        "test-analysis",
        True,
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetAnalysis")
    )
    rpc.send_initial_metadata(())
    rpc.terminate(
        _ANALYSES[0],
        (),
        grpc.StatusCode.NOT_FOUND,
        "",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateAnalysis")
    )
    rpc.send_initial_metadata(())
    want_request = lva_service_pb2.CreateAnalysisRequest(
        parent=self._parent,
        analysis_id="test-analysis",
        analysis=lva_resources_pb2.Analysis(
            analysis_definition=self._test_graph.get_analysis_definition(),
            disable_event_watch=True,
        ),
    )
    want_request.analysis.analysis_definition.analyzers.sort(
        key=lambda a: a.analyzer
    )
    actual_request.analysis.analysis_definition.analyzers.sort(
        key=lambda a: a.analyzer
    )
    self.assertEqual(
        want_request.analysis.analysis_definition,
        actual_request.analysis.analysis_definition,
    )
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_wait_process_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    process = client._resource_to_process(_PROCESSES[2])
    application_future = self._thread_pool.submit(
        process.wait,
        self._connection_options,
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetProcess")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetProcessRequest(
            name="{}/processes/{}".format(self._parent, "test-process-3")
        ),
        actual_request,
    )
    rpc.terminate(
        _PROCESSES[2],
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertEqual(application_future.result(), process)

  def test_wait_process_timeout(self):
    self._mock_create_channel.return_value = self.create_channel()
    process = client._resource_to_process(_PROCESSES[1])
    application_future = self._thread_pool.submit(
        process.wait,
        self._connection_options,
        timedelta(seconds=1),
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetProcess")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetProcessRequest(
            name="{}/processes/{}".format(self._parent, "test-process-2")
        ),
        actual_request,
    )
    rpc.terminate(
        _PROCESSES[1],
        (),
        grpc.StatusCode.OK,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, TimeoutError)

  def test_get_process_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_process,
        self._connection_options,
        "test-process",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetProcess")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetProcessRequest(
            name="{}/processes/{}".format(self._parent, "test-process")
        ),
        actual_request,
    )
    rpc.terminate(
        _PROCESSES[0],
        (),
        grpc.StatusCode.OK,
        "",
    )
    process = client._resource_to_process(_PROCESSES[0])
    process.process_id = "test-process-1"
    self.assertEqual(application_future.result(), process)

  def test_get_process_not_found(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.get_process,
        self._connection_options,
        "test-process",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "GetProcess")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.GetProcessRequest(
            name="{}/processes/{}".format(self._parent, "test-process")
        ),
        actual_request,
    )
    rpc.terminate(
        _PROCESSES[0],
        (),
        grpc.StatusCode.NOT_FOUND,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_list_processes_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.list_processes,
        self._connection_options,
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "ListProcesses")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.ListProcessesRequest(
            parent=self._parent,
        ),
        actual_request,
    )
    rpc.terminate(
        lva_service_pb2.ListProcessesResponse(processes=_PROCESSES),
        (),
        grpc.StatusCode.OK,
        "",
    )
    got_processes = list(application_future.result())
    want_processes = list(map(client._resource_to_process, _PROCESSES))

    if len(got_processes) != len(want_processes):
      raise self.failureException("Lists have different length")
    [
        self.assertEqual(got, want)
        for (got, want) in zip(got_processes, want_processes)
    ]

  def test_list_processes_not_found(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.list_processes,
        self._connection_options,
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "ListProcesses")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.ListProcessesRequest(
            parent=self._parent,
        ),
        actual_request,
    )
    rpc.terminate(
        lva_service_pb2.ListProcessesResponse(processes=_PROCESSES),
        (),
        grpc.StatusCode.NOT_FOUND,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_delete_process_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.delete_process,
        self._connection_options,
        "test-process",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "DeleteProcess")
    )
    rpc.send_initial_metadata(())
    self.assertEqual(
        lva_service_pb2.DeleteProcessRequest(
            name="{}/processes/{}".format(self._parent, "test-process")
        ),
        actual_request,
    )
    rpc.terminate(
        operations_pb2.Operation(name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    lro_rpc.terminate(
        operations_pb2.Operation(done=True),
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertIsNone(application_future.result())

  def test_delete_process_lva_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.delete_process,
        self._connection_options,
        "test-process",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "DeleteProcess")
    )
    rpc.terminate(
        operations_pb2.Operation(),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_delete_process_lro_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.delete_process,
        self._connection_options,
        "test-process",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "DeleteProcess")
    )
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    lro_rpc.terminate(
        operations_pb2.Operation(),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_create_process_successfully(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.create_process,
        self._connection_options,
        "test-analysis",
        {"k1": "v1", "k2": "v2"},
        lva_pb2.RunMode.SUBMISSION,
        3,
        "",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateProcess")
    )
    rpc.send_initial_metadata(())
    want_request = lva_service_pb2.CreateProcessRequest(
        parent=self._parent,
        process_id=actual_request.process_id,
        process=lva_resources_pb2.Process(
            analysis="{}/analyses/{}".format(self._parent, "test-analysis"),
            attribute_overrides=["k1=v1", "k2=v2"],
            run_mode=lva_pb2.RunMode.SUBMISSION,
            retry_count=3,
        ),
    )
    self.assertEqual(want_request, actual_request)
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    resp = any_pb2.Any()
    resp.Pack(_PROCESSES[0])
    lro_rpc.terminate(
        operations_pb2.Operation(done=True, response=resp),
        (),
        grpc.StatusCode.OK,
        "",
    )
    self.assertEqual(
        application_future.result(), client._resource_to_process(_PROCESSES[0])
    )

  def test_create_process_lva_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.create_process,
        self._connection_options,
        "test-analysis",
        {"k1": "v1", "k2": "v2"},
        lva_pb2.RunMode.SUBMISSION,
        3,
        "",
    )
    _, _, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateProcess")
    )
    rpc.send_initial_metadata(())
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)

  def test_create_process_lro_service_failure(self):
    self._mock_create_channel.return_value = self.create_channel()
    application_future = self._thread_pool.submit(
        client.create_process,
        self._connection_options,
        "test-analysis",
        {"k1": "v1", "k2": "v2"},
        lva_pb2.RunMode.SUBMISSION,
        3,
        "",
    )
    _, actual_request, rpc = self._channel.take_unary_unary(
        _get_descriptor(lva_service_pb2, "LiveVideoAnalytics", "CreateProcess")
    )
    rpc.send_initial_metadata(())
    want_request = lva_service_pb2.CreateProcessRequest(
        parent=self._parent,
        process_id=actual_request.process_id,
        process=lva_resources_pb2.Process(
            analysis="{}/analyses/{}".format(self._parent, "test-analysis"),
            attribute_overrides=["k1=v1", "k2=v2"],
            run_mode=lva_pb2.RunMode.SUBMISSION,
            retry_count=3,
        ),
    )
    self.assertEqual(want_request, actual_request)
    rpc.terminate(
        operations_pb2.Operation(done=False, name="test-operation"),
        (),
        grpc.StatusCode.OK,
        "",
    )
    _, actual_lro_request, lro_rpc = self._channel.take_unary_unary(
        _get_descriptor(operations_pb2, "Operations", "GetOperation")
    )
    lro_rpc.send_initial_metadata(())
    self.assertEqual(
        actual_lro_request,
        operations_pb2.GetOperationRequest(name="test-operation"),
    )
    resp = any_pb2.Any()
    resp.Pack(_PROCESSES[0])
    lro_rpc.terminate(
        operations_pb2.Operation(done=True, response=resp),
        (),
        grpc.StatusCode.INTERNAL,
        "",
    )
    application_exception = application_future.exception()
    self.assertIsInstance(application_exception, grpc.RpcError)


if __name__ == "__main__":
  googletest.main()
