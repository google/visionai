# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Unit tests for vod_corpus library."""

from unittest import mock
from visionai.python.testing import googletest
from visionai.python.warehouse.transformer import corpus_indexing_transformer as cit
from visionai.python.warehouse.utils import vod_corpus

_PROJECT_NUMBER = 1
_LOCATION_ID = "us-central1"
_CORPUS_DISPLAY_NAME = "test corpus"
_CORPUS_DISCRIPTION = "test discription"
_CORPUS_NAME = "projects/1/locations/us-central1/corpora/2"
_INDEX_DISPLAY_NAME = "test index"


class VodCorpusTest(googletest.TestCase):

  def test_create_corpus(self):
    warehouse_client = mock.MagicMock()
    vod_corpus.create_corpus(
        warehouse_client,
        _PROJECT_NUMBER,
        _LOCATION_ID,
        _CORPUS_DISPLAY_NAME,
        _CORPUS_DISCRIPTION,
    )
    warehouse_client.create_corpus.assert_called()

  def test_index_corpus(self):
    mock_corpus_indexing_transformer = mock.MagicMock()
    mock_create_corpus_indexing_transformer = self.enter_context(
        mock.patch.object(cit, "CorpusIndexingTransformer", autospec=True)
    )
    mock_create_corpus_indexing_transformer.return_value = (
        mock_corpus_indexing_transformer
    )
    warehouse_client = mock.MagicMock()
    vod_corpus.index_corpus(warehouse_client, _CORPUS_NAME, _INDEX_DISPLAY_NAME)
    mock_corpus_indexing_transformer.transform.assert_called()


if __name__ == "__main__":
  googletest.main()
