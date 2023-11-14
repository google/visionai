# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for TransformProgress."""

import logging
from typing import Sequence
from unittest import mock
from google.api_core import exceptions
from google.api_core import operation
from google.auth import credentials as ga_credentials
from google.cloud import videointelligence_v1

from google.longrunning import operations_pb2
from google.protobuf import duration_pb2
from google.protobuf import struct_pb2
from google.protobuf import timestamp_pb2
from google.rpc import status_pb2
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.lva import client
from visionai.python.net import channel
from visionai.python.warehouse.transformer import transform_progress

_TEST_OPERATION_NAME = "test/operation"
_ASSET_NAME = "projects/1/copora/2/assets/3"
_logger = logging.getLogger(__name__)


def make_operation_proto(
    name=_TEST_OPERATION_NAME,
    metadata=None,
    response=None,
    error=None,
    **kwargs
):
  operation_proto = operations_pb2.Operation(name=name, **kwargs)

  if metadata is not None:
    operation_proto.metadata.Pack(metadata)

  if response is not None:
    operation_proto.response.Pack(response)

  if error is not None:
    operation_proto.error.CopyFrom(error)

  return operation_proto


def make_operation_future(
    client_operations_responses=None, result_type=struct_pb2.Struct
):
  if client_operations_responses is None:
    client_operations_responses = [make_operation_proto()]

  refresh = mock.Mock(
      spec=["__call__"], side_effect=client_operations_responses
  )
  refresh.responses = client_operations_responses
  cancel = mock.Mock(spec=["__call__"])
  operation_future = operation.Operation(
      client_operations_responses[0],
      refresh,
      cancel,
      result_type=result_type,
      metadata_type=struct_pb2.Struct,
  )

  return operation_future, refresh, cancel


class TransformProgressTest(googletest.TestCase):

  def test_lro_transform_progress_done(self):
    expected_result = struct_pb2.Struct()
    responses = [
        make_operation_proto(),
        # Second operation response includes the result.
        make_operation_proto(done=True, response=expected_result),
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)

    self.assertFalse(lro_transform_progress.done())
    self.assertTrue(lro_transform_progress.done())

  def test_lro_transform_progress_err(self):
    expected_exception = status_pb2.Status(message="err msg")
    responses = [
        make_operation_proto(),
        # Second operation response includes the err.
        make_operation_proto(done=True, error=expected_exception),
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)

    self.assertFalse(lro_transform_progress.done())
    self.assertTrue(lro_transform_progress.done())
    self.assertRaises(
        transform_progress.TransformError, lro_transform_progress.result
    )

  def test_lro_transform_progress_result(self):
    expected_result = struct_pb2.Struct()
    responses = [
        make_operation_proto(done=True, response=expected_result),
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)

    self.assertTrue(lro_transform_progress.done())
    self.assertEqual(lro_transform_progress.result(), expected_result)

  def test_lro_transform_progress_running(self):
    responses = [
        make_operation_proto(done=False),
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)

    self.assertFalse(lro_transform_progress.done())

  def test_lro_transform_progress_get_metadata(self):
    expected_metadata = struct_pb2.Struct()
    future, _, _ = make_operation_future(
        [make_operation_proto(metadata=expected_metadata)]
    )
    lro_transform_progress = transform_progress.LroTransformProgress(future)

    self.assertEqual(lro_transform_progress.metadata(), expected_metadata)

  def test_lro_transform_progress_get_response(self):
    expected_result = struct_pb2.Struct()
    expected_result["key"] = "value"
    responses = [
        make_operation_proto(done=True, response=expected_result),
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)
    actrual_result = struct_pb2.Struct()
    lro_transform_progress.response().Unpack(actrual_result)
    self.assertEqual(actrual_result, expected_result)

  def test_lro_transform_progress_get_response_none(self):
    responses = [
        make_operation_proto(done=False),
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)
    self.assertIsNone(lro_transform_progress.response())

  def test_lro_transform_progress_get_operation(self):
    expected_result = struct_pb2.Struct()
    expected_result["key"] = "value"
    expected_operation = make_operation_proto(
        done=True, response=expected_result
    )
    responses = [
        expected_operation,
    ]
    future, _, _ = make_operation_future(responses)
    lro_transform_progress = transform_progress.LroTransformProgress(future)
    self.assertEqual(
        lro_transform_progress.operation(),
        expected_operation,
    )

  def test_wait_and_write_warehouse_transform_progress_done(self):
    responses = [
        make_operation_proto(),
        # Second operation response includes the result.
        make_operation_proto(
            done=True,
            response=videointelligence_v1.types.AnnotateVideoResponse.pb()(),
        ),
    ]
    future, _, _ = make_operation_future(
        responses, result_type=videointelligence_v1.AnnotateVideoResponse
    )
    mock_warehouse_client = mock.MagicMock()

    def _construct_annotations(
        _: videointelligence_v1.AnnotateVideoResponse,
    ) -> Sequence[visionai_v1.Annotation]:
      annotations = []
      for i in range(10):
        annotation = visionai_v1.Annotation()
        annotation.user_specified_annotation = visionai_v1.UserSpecifiedAnnotation(
            key="key",
            partition=visionai_v1.Partition(
                relative_temporal_partition=visionai_v1.Partition.RelativeTemporalPartition(
                    start_offset=duration_pb2.Duration(seconds=i),
                    end_offset=duration_pb2.Duration(seconds=i + 1),
                )
            ),
            value=visionai_v1.AnnotationValue(int_value=i),
        )
        annotations.append(annotation)
      return annotations

    progress = transform_progress.WaitAndWriteWarehouseTransformProgress(
        _ASSET_NAME, mock_warehouse_client, future, _construct_annotations
    )

    self.assertTrue(progress.result())
    expected_create_annotation_calls = []
    for annotation in _construct_annotations(_):
      request = visionai_v1.CreateAnnotationRequest(
          parent=_ASSET_NAME, annotation=annotation
      )
      expected_create_annotation_calls.append(
          mock.call.create_annotation(request, retry=mock.ANY)
      )
    mock_warehouse_client.assert_has_calls(expected_create_annotation_calls)

  def test_wait_and_write_warehouse_transform_progress_err(self):
    expected_exception = status_pb2.Status(message="err msg")
    responses = [
        make_operation_proto(),
        # Second operation response includes the err.
        make_operation_proto(done=True, error=expected_exception),
    ]
    future, _, _ = make_operation_future(responses)
    def _construct_annotations(
        _: videointelligence_v1.AnnotateVideoResponse,
    ) -> Sequence[visionai_v1.Annotation]:
      return []
    progress = transform_progress.WaitAndWriteWarehouseTransformProgress(
        _ASSET_NAME, mock.MagicMock(), future, _construct_annotations
    )
    self.assertRaises(
        transform_progress.TransformError, progress.result
    )

  def test_combined_transform_progress_done(self):
    expected_result_1 = struct_pb2.Struct()
    expected_result_1["key1"] = "value1"
    responses_1 = [
        make_operation_proto(),
        # Second operation response includes the result.
        make_operation_proto(done=True, response=expected_result_1),
    ]
    future_1, _, _ = make_operation_future(responses_1)

    expected_result_2 = struct_pb2.Struct()
    expected_result_2["key2"] = "value2"
    responses_2 = [
        make_operation_proto(),
        make_operation_proto(done=True, response=expected_result_2),
    ]
    future_2, _, _ = make_operation_future(responses_2)
    transform_progress_1 = transform_progress.LroTransformProgress(future_1)
    transform_progress_2 = transform_progress.LroTransformProgress(future_2)
    combined_transform_progress = transform_progress.CombinedTransformProgress([
        transform_progress_1,
        transform_progress_2,
    ])

    self.assertFalse(combined_transform_progress.done())
    self.assertFalse(combined_transform_progress.done())
    self.assertTrue(combined_transform_progress.done())
    expected_result = {
        transform_progress_1.get_identifier(): expected_result_1,
        transform_progress_2.get_identifier(): expected_result_2,
    }
    self.assertDictEqual(combined_transform_progress.result(), expected_result)

  def test_combined_transform_progress_err(self):
    expected_exception = status_pb2.Status(message="err msg")
    responses_1 = [
        make_operation_proto(),
        # Second operation response includes the exception.
        make_operation_proto(done=True, error=expected_exception),
    ]
    future_1, _, _ = make_operation_future(responses_1)

    expected_result_2 = struct_pb2.Struct()
    expected_result_2["key2"] = "value2"
    responses_2 = [
        make_operation_proto(),
        make_operation_proto(done=True, response=expected_result_2),
    ]
    future_2, _, _ = make_operation_future(responses_2)
    transform_progress_1 = transform_progress.LroTransformProgress(future_1)
    transform_progress_2 = transform_progress.LroTransformProgress(future_2)
    combined_transform_progress = transform_progress.CombinedTransformProgress([
        transform_progress_1,
        transform_progress_2,
    ])

    self.assertFalse(combined_transform_progress.done())
    self.assertFalse(combined_transform_progress.done())
    self.assertTrue(combined_transform_progress.done())

    self.assertRaises(
        transform_progress.TransformError, combined_transform_progress.result
    )


class LvaTransformProgressTest(googletest.TestCase):

  def _construct_process(self, state: visionai_v1.RunStatus.State):
    return visionai_v1.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process",
        create_time=timestamp_pb2.Timestamp(seconds=1),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis",
        attribute_overrides=["k1=v1", "k2=v2"],
        run_status=visionai_v1.RunStatus(
            state=state,
        ),
        run_mode=visionai_v1.RunMode.SUBMISSION,
    )

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
    self._mock_create_lva_client = self.enter_context(
        mock.patch.object(client, "_create_lva_client", autospec=True)
    )
    self._mock_create_lva_client.return_value = self._lva_client

  def test_lva_transform_progress_done(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.return_value = self._construct_process(
          visionai_v1.RunStatus.State.RUNNING
      )
      lva_transform_progress = transform_progress.LvaTransformProgress(
          connection_options=self._connection_options,
          process_id="test-process",
      )
      self.assertFalse(lva_transform_progress.done())

    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.return_value = self._construct_process(
          visionai_v1.RunStatus.State.FAILED
      )
      lva_transform_progress = transform_progress.LvaTransformProgress(
          connection_options=self._connection_options,
          process_id="test-process",
      )
      self.assertTrue(lva_transform_progress.done())

    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.return_value = self._construct_process(
          visionai_v1.RunStatus.State.COMPLETED
      )
      lva_transform_progress = transform_progress.LvaTransformProgress(
          connection_options=self._connection_options,
          process_id="test-process",
      )
      self.assertTrue(lva_transform_progress.done())

  def test_lva_transform_progress_result(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.side_effect = [
          self._construct_process(visionai_v1.RunStatus.State.RUNNING),
          self._construct_process(visionai_v1.RunStatus.State.COMPLETED),
      ]
      lva_transform_progress = transform_progress.LvaTransformProgress(
          connection_options=self._connection_options,
          process_id="test-process",
      )
      self.assertEqual(
          lva_transform_progress.result(),
          client.Process(
              self._construct_process(visionai_v1.RunStatus.State.COMPLETED)
          ),
      )

  def test_lva_transform_progress_exception(self):
    with mock.patch.object(
        type(self._lva_client.transport.get_process), "__call__"
    ) as call:
      call.side_effect = exceptions.GoogleAPICallError("get process error")
      lva_transform_progress = transform_progress.LvaTransformProgress(
          connection_options=self._connection_options,
          process_id="test-process",
      )
      self.assertRaises(
          transform_progress.TransformError, lva_transform_progress.result
      )


if __name__ == "__main__":
  googletest.main()
