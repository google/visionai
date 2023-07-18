# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Library to interfact with warehouse VOD index endpoints."""

import logging

from visionai.python.gapic.visionai import visionai_v1

_logger = logging.getLogger(__name__)


def create_index_endpoint(
    warehouse_client: visionai_v1.WarehouseClient,
    project_number: int,
    location_id: str,
    display_name: str,
    description: str = "",
    index_endpoint_id: str = None,
    timeout: int = 7200,
) -> visionai_v1.IndexEndpoint:
  """Creates index endpoint with given metadata.

  Args:
    warehouse_client: the client to talk to warehouse.
    project_number: the project number.
    location_id: the location id.
    display_name: the display name for the index endpoint.
    description: the description for the index endpoint.
    index_endpoint_id: the id for the created resource. If not specified, the
      system will auto generate one.
    timeout: a timeout in seconds for waiting for the index endpoint creation.
      By defualt, waits for 2h.

  Returns:
    The created index endpoint.
  Raises:
      google.api_core.GoogleAPICallError: If the operation errors or if
          the timeout is reached before the operation completes.
  """
  request = visionai_v1.CreateIndexEndpointRequest(
      parent=f"projects/{project_number}/locations/{location_id}",
      index_endpoint=visionai_v1.IndexEndpoint(
          display_name=display_name,
          description=description,
      ),
  )
  if index_endpoint_id:
    request.index_endpoint_id = index_endpoint_id
  operation = warehouse_client.create_index_endpoint(request=request)

  _logger.info(
      "Wait for index endpoint operation: %s", operation.operation.name
  )
  index_endpoint = operation.result(timeout=timeout)
  _logger.info("Created index endpoint %s", index_endpoint)
  return index_endpoint
