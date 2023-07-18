# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for CorpusIndexingTransformer."""

from unittest import mock

from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import corpus_indexing_transformer

_CORPUS_NAME = "projects/1/copora/2"
_INDEX_DISPLAY_NAME = "Test index"


class CorpusIndexingTransformerTest(googletest.TestCase):

  def test_transform(self):
    index = visionai_v1.Index(display_name=_INDEX_DISPLAY_NAME)
    transformer = corpus_indexing_transformer.CorpusIndexingTransformer(
        warehouse_client=mock.MagicMock(), index=index
    )
    transformer.transform(_CORPUS_NAME)
    request = visionai_v1.CreateIndexRequest(parent=_CORPUS_NAME, index=index)
    transformer.warehouse_client.assert_has_calls(
        [mock.call.create_index(request=request)]
    )


if __name__ == "__main__":
  googletest.main()
