# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Test for client library."""

from unittest import mock
from google.auth import credentials as ga_credentials
from google.longrunning import operations_pb2
from google.protobuf import any_pb2, empty_pb2
from google.protobuf import timestamp_pb2
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.net import channel
from visionai.python.streams import client

_STREAMS = [
    visionai_v1.Stream(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/streams/test-stream-1",
        create_time=timestamp_pb2.Timestamp(seconds=1),
    ),
    visionai_v1.Stream(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/streams/test-stream-2",
        create_time=timestamp_pb2.Timestamp(seconds=10),
    ),
]

_CLUSTER = visionai_v1.Cluster(
    name="projects/test-project/locations/us-central1/clusters/test-cluster",
    create_time=timestamp_pb2.Timestamp(seconds=1),
)


class ClientTest(googletest.TestCase):

  def setUp(self):
    super().setUp()
    self._streams_client = visionai_v1.StreamsServiceClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="grpc",
    )
    self._connection_options = channel.ConnectionOptions(
        project_id="test-project",
        location_id="us-central1",
        cluster_id="test-cluster",
    )
    self._mock_create_streams_client = self.enter_context(
        mock.patch.object(client, "_create_streams_client", autospec=True)
    )
    self._mock_create_streams_client.return_value = self._streams_client

  def test_list_streams_successfully(self):
    with mock.patch.object(
        type(self._streams_client.transport.list_streams), "__call__"
    ) as call:
      call.return_value = visionai_v1.ListStreamsResponse(streams=_STREAMS)
      response = client.list_streams(self._connection_options)
      self.assertEqual(response.streams, _STREAMS)

  def test_create_cluster_successfully(self):
    with mock.patch.object(
        type(self._streams_client.transport.create_cluster), "__call__"
    ) as call:
      resp = any_pb2.Any()
      resp.Pack(_CLUSTER.__dict__["_pb"])
      call.return_value = operations_pb2.Operation(
          done=True,
          name="test-operation",
          response=resp,
      )
      self.assertEqual(
          client.create_cluster(self._connection_options, timeout=3600),
          _CLUSTER,
      )

  def test_delete_cluster_successfully(self):
    with mock.patch.object(
        type(self._streams_client.transport.delete_cluster), "__call__"
    ) as call:
      resp = any_pb2.Any()
      resp.Pack(empty_pb2.Empty())
      call.return_value = operations_pb2.Operation(
          done=True,
          name="test-operation",
          response=resp,
      )
      client.delete_cluster(self._connection_options, timeout=3600)

if __name__ == "__main__":
  googletest.main()
