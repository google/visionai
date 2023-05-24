# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Library for creating authenticated gRPC channels."""

import dataclasses

from google import auth as google_auth
from google.auth.transport import grpc as google_auth_transport_grpc
from google.auth.transport import requests as google_auth_transport_requests


@dataclasses.dataclass
class ConnectionOptions:
  project_id: str
  location_id: str
  cluster_id: str
  env: str = 'prod'


def _get_service_endpoint(env: str) -> str:
  if env == 'staging':
    return 'staging-visionai.sandbox.googleapis.com'
  if env == 'autopush':
    return 'autopush-visionai.sandbox.googleapis.com'
  return 'visionai.googleapis.com'


def create_channel(options: ConnectionOptions):
  """Create the grpc channel connecting to Google services.

  Args:
    options: Connection options which specify the arguments for connecting to
      Google services.

  Returns:
    The created grpc channel.
  """
  credentials, _ = google_auth.default()
  request = google_auth_transport_requests.Request()
  return google_auth_transport_grpc.secure_authorized_channel(
      credentials, request, _get_service_endpoint(options.env)
  )
