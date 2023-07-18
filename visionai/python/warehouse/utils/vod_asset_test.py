# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Unit tests for vod_asset library."""

from unittest import mock
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.utils import vod_asset

_GCS_FILE = "gs://file.mp4"
_CORPUS_NAME = "projects/1/locations/us-central1/corpora/2"
_ASSET_NAME = ""


class VodAssetTest(googletest.TestCase):

  def test_create_and_upload_asset(self):
    warehouse_client = mock.MagicMock()
    warehouse_client.create_asset.return_value = visionai_v1.Asset(
        name=_ASSET_NAME
    )
    vod_asset.create_and_upload_asset(
        warehouse_client=warehouse_client,
        gcs_file=_GCS_FILE,
        corpus_name=_CORPUS_NAME,
    )
    warehouse_client.upload_asset.assert_called_once_with(
        visionai_v1.UploadAssetRequest(
            name=_ASSET_NAME,
            asset_source=visionai_v1.AssetSource(
                asset_gcs_source=visionai_v1.AssetSource.AssetGcsSource(
                    gcs_uri=_GCS_FILE
                )
            ),
        )
    )

  def test_transform_single_asset(self):
    mock_ml_transformers = (mock.MagicMock(), mock.MagicMock())
    mock_asset_indexing_transformer = mock.MagicMock()
    vod_asset.transform_single_asset(
        _ASSET_NAME, mock_ml_transformers, mock_asset_indexing_transformer
    )
    for ml_transformer in mock_ml_transformers:
      ml_transformer.transform.assert_called_once_with(_ASSET_NAME)
    mock_asset_indexing_transformer.transform.assert_called_once_with(
        _ASSET_NAME
    )


if __name__ == "__main__":
  googletest.main()
