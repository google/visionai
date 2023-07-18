# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Unit tests for vod_corpus library."""

from unittest import mock
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.utils import vod_index_endpoint

_PROJECT_NUMBER = 1
_LOCATION_ID = "us-central1"
_INDEX_ENDPOINT_ID = "i2"
_INDEX_ENDPOINT_DISPLAY_NAME = "test corpus"
_INDEX_ENDPOINT_DESCRIPTION = "test discription"


class VodIndexEndpointTest(googletest.TestCase):

  def test_create_index_endpoint(self):
    warehouse_client = mock.MagicMock()
    vod_index_endpoint.create_index_endpoint(
        warehouse_client=warehouse_client,
        project_number=_PROJECT_NUMBER,
        location_id=_LOCATION_ID,
        display_name=_INDEX_ENDPOINT_DISPLAY_NAME,
        description=_INDEX_ENDPOINT_DESCRIPTION,
        index_endpoint_id=_INDEX_ENDPOINT_ID,
    )

    warehouse_client.create_index_endpoint.assert_called_once_with(
        request=visionai_v1.CreateIndexEndpointRequest(
            parent=f"projects/{_PROJECT_NUMBER}/locations/{_LOCATION_ID}",
            index_endpoint=visionai_v1.IndexEndpoint(
                display_name=_INDEX_ENDPOINT_DISPLAY_NAME,
                description=_INDEX_ENDPOINT_DESCRIPTION,
            ),
            index_endpoint_id=_INDEX_ENDPOINT_ID,
        )
    )


if __name__ == "__main__":
  googletest.main()
