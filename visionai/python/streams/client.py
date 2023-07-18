# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Base client library for the Streams service."""

import logging
from typing import Optional

from google.api_core import client_options as client_options_lib

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.net import channel

_logger = logging.getLogger(__name__)


def _create_streams_client(
    connection_options: channel.ConnectionOptions,
) -> visionai_v1.StreamsServiceClient:
  return visionai_v1.StreamsServiceClient(
      client_options=client_options_lib.ClientOptions(
          api_endpoint=channel.get_service_endpoint(connection_options.env),
      ),
      transport="grpc",
  )


def list_streams(connection_options: channel.ConnectionOptions):
  """List Streams from Vertex AI Vision.

  Args:
    connection_options: A `ConnectionOptions` targetting a specific Vertex AI
      Vision instance.

  Returns:
    A `ListStreamsResponse`.
  """
  streams_client = _create_streams_client(connection_options)

  request = visionai_v1.ListStreamsRequest()
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
  response = streams_client.list_streams(request, metadata=metadata)
  return response


def create_cluster(
    connection_options: channel.ConnectionOptions, timeout: Optional[int] = 7200
) -> visionai_v1.Cluster:
  """Creates a new cluster and wait for it to complete.

  Args:
    connection_options: A `ConnectionOptions` targetting a specific Vertex AI
      Vision instance.
    timeout: a timeout in seconds for waiting for the cluster creation. By
      defualt, waits for 2h.

  Returns:
    The cluster created.

  Raises:
      google.api_core.GoogleAPICallError: If the operation errors or if
          the timeout is reached before the operation completes.
  """
  streams_client = _create_streams_client(connection_options)
  parent = "projects/{project_id}/locations/{location_id}".format(
      project_id=connection_options.project_id,
      location_id=connection_options.location_id,
  )
  request = visionai_v1.CreateClusterRequest(
      cluster_id=connection_options.cluster_id,
      parent=parent,
  )

  create_cluster_op = streams_client.create_cluster(
      request, metadata=[("x-goog-request-params", "parent={}".format(parent))]
  )
  _logger.info(
      "Wait for CreateCluster operation %s. ", create_cluster_op.operation.name
  )
  _logger.info("Cluster created %s", create_cluster_op.result(timeout=timeout))
  return create_cluster_op.result()


def delete_cluster(
    connection_options: channel.ConnectionOptions, timeout: Optional[int] = 7200
) -> None:
  """Deletes a cluster and wait for it to complete.

  Args:
    connection_options: A `ConnectionOptions` targetting a specific Vertex AI
      Vision instance.
    timeout: a timeout in seconds for waiting for the cluster deletion. By
      defualt, waits for 2h.

  Returns:
    The cluster deleted.

  Raises:
      google.api_core.GoogleAPICallError: If the operation errors or if
          the timeout is reached before the operation completes.
  """
  streams_client = _create_streams_client(connection_options)
  name = (
      "projects/{project_id}/locations/{location_id}/clusters/{cluster_id}"
      .format(
          project_id=connection_options.project_id,
          location_id=connection_options.location_id,
          cluster_id=connection_options.cluster_id,
      )
  )
  request = visionai_v1.DeleteClusterRequest(
      name=name,
  )

  delete_cluster_op = streams_client.delete_cluster(
      request, metadata=[("x-goog-request-params", "name={}".format(name))]
  )
  _logger.info(
      "Wait for DeleteCluster operation %s. ", delete_cluster_op.operation.name
  )
  _logger.info("Cluster deleted %s", delete_cluster_op.result(timeout=timeout))
