# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Example client binary to list streams in Vertex AI Vision."""

import dataclasses

from google import auth as google_auth
from google.auth.transport import grpc as google_auth_transport_grpc
from google.auth.transport import requests as google_auth_transport_requests

from visionai.python.protos.googleapis.v1 import streams_service_pb2
from visionai.python.protos.googleapis.v1 import streams_service_pb2_grpc


@dataclasses.dataclass
class ConnectionOptions:
  service_endpoint: str
  project_id: str
  location_id: str
  cluster_id: str


def list_streams(connection_options):
  """List Streams from Vertex AI Vision.

  Args:
    connection_options: A `ConnectionOptions` targetting a specific Vertex AI
      Vision instance.

  Returns:
    A `ListStreamsResponse`.
  """
  credentials, _ = google_auth.default()
  request = google_auth_transport_requests.Request()
  channel = google_auth_transport_grpc.secure_authorized_channel(
      credentials, request, connection_options.service_endpoint
  )
  stub = streams_service_pb2_grpc.StreamsServiceStub(channel)
  request = streams_service_pb2.ListStreamsRequest()
  parent = (
      "projects/{project_id}/locations/{location_id}/clusters/{cluster_id}"
      .format(
          project_id=connection_options.project_id,
          location_id=connection_options.location_id,
          cluster_id=connection_options.cluster_id,
      )
  )
  metadata = [("x-goog-request-params", "parent={}".format(parent))]
  request.parent = parent
  response = stub.ListStreams(request, metadata=metadata)
  return response
