# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for transformer_factory."""

from unittest import mock

from google.cloud import videointelligence_v1

from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.lva import client
from visionai.python.warehouse.transformer import embedding_transformer
from visionai.python.warehouse.transformer import ocr_transformer as ot
from visionai.python.warehouse.transformer import speech_transformer as st
from visionai.python.warehouse.transformer import transformer_factory

_CORPUS_NAME = "projects/1/locations/us-central1/corpora/2"
_LANGUAGE_CODE = "en-US"


class TransformerFactoryTest(googletest.TestCase):

  def test_create_ml_transformers(self):
    config = transformer_factory.MlTransformersCreationConfig(
        run_embedding=True,
        speech_transformer_init_config=st.SpeechTransformerInitConfig(
            _CORPUS_NAME, _LANGUAGE_CODE
        ),
        ocr_transformer_init_config=ot.OcrTransformerInitConfig(_CORPUS_NAME),
    )
    mock_video_intelligence_client = mock.MagicMock()
    mock_create_video_intelligence_client = self.enter_context(
        mock.patch.object(
            videointelligence_v1,
            "VideoIntelligenceServiceClient",
            autospec=True,
        )
    )
    mock_create_video_intelligence_client.return_value = (
        mock_video_intelligence_client
    )
    warehouse_client = mock.MagicMock()
    warehouse_client.create_data_schema.return_value = visionai_v1.DataSchema()
    mock_create_lva_client = self.enter_context(
        mock.patch.object(client, "_create_lva_client", autospec=True)
    )
    lva_client = mock.MagicMock()
    mock_create_lva_client.return_value = lva_client
    ml_transformers = transformer_factory.create_ml_transformers(
        warehouse_client=warehouse_client,
        ml_transformers_creation_config=config,
    )
    has_speech_transformer = False
    has_embedding_transformer = False
    has_ocr_transformer = False
    for transformer in ml_transformers:
      has_speech_transformer = (
          True
          if isinstance(transformer, st.SpeechTransformer)
          else has_speech_transformer
      )
      has_embedding_transformer = (
          True
          if isinstance(transformer, embedding_transformer.EmbeddingTransformer)
          else has_embedding_transformer
      )
      has_ocr_transformer = (
          True
          if isinstance(transformer, ot.OcrTransformer)
          else has_ocr_transformer
      )

    self.assertTrue(has_speech_transformer)
    self.assertTrue(has_embedding_transformer)
    self.assertTrue(has_ocr_transformer)


if __name__ == "__main__":
  googletest.main()
