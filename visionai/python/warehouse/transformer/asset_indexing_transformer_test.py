# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for AssetIndexingTransformer."""

from unittest import mock

from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import asset_indexing_transformer

_ASSET_NAME = "projects/1/copora/2/assets/3"
_INDEX_NAME = "projects/1/copora/2/assets/3/indexes/4"


class AssetIndexingTransformerTest(googletest.TestCase):

  def test_transform(self):
    transformer = asset_indexing_transformer.AssetIndexingTransformer(
        warehouse_client=mock.MagicMock(), index_name=_INDEX_NAME
    )
    transformer.transform(_ASSET_NAME)
    request = visionai_v1.IndexAssetRequest(name=_ASSET_NAME, index=_INDEX_NAME)
    transformer.warehouse_client.assert_has_calls(
        [mock.call.index_asset(request=request)]
    )


if __name__ == "__main__":
  googletest.main()
