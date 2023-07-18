# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for EmbeddingTransformer."""

from unittest import mock

from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import embedding_transformer

_ASSET_NAME = "projects/1/copora/2/assets/3"


class EmbeddingTransformerTest(googletest.TestCase):

  def test_transform(self):
    transformer = embedding_transformer.EmbeddingTransformer(
        warehouse_client=mock.MagicMock()
    )
    transformer.transform(_ASSET_NAME)
    request = visionai_v1.AnalyzeAssetRequest(
        name=_ASSET_NAME,
    )
    transformer.warehouse_client.assert_has_calls(
        [mock.call.analyze_asset(request=request)]
    )


if __name__ == "__main__":
  googletest.main()
