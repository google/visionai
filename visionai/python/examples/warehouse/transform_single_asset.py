# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Sample application to perform warehouse transformations for one existing asset."""

import logging

from absl import app
from absl import flags

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.net import channel
from visionai.python.warehouse.transformer import asset_indexing_transformer as ait
from visionai.python.warehouse.transformer import ocr_transformer
from visionai.python.warehouse.transformer import speech_transformer
from visionai.python.warehouse.transformer import transformer_factory
from visionai.python.warehouse.utils import vod_asset
from visionai.python.warehouse.utils import vod_corpus

_PROJECT_NUMBER = flags.DEFINE_integer(
    "project_number", None, "Project number."
)
_LOCATION_ID = flags.DEFINE_string("location_id", "us-central1", "Location id.")
_CORPUS_ID = flags.DEFINE_integer("corpus_id", None, "Corpus id.")
_ASSET_ID = flags.DEFINE_string("asset_id", None, "Asset id.")
_ENV = flags.DEFINE_enum(
    "env",
    "PROD",
    ["AUTOPUSH", "STAGING", "PROD"],
    "The environment.",
)
_INDEX_DISPLAY_NAME = flags.DEFINE_string(
    "index_display_name", "Demo Index", "Index display name."
)
_logger = logging.getLogger(__name__)


def main(unused_argv) -> None:
  corpus_name = visionai_v1.WarehouseClient.corpus_path(
      _PROJECT_NUMBER.value, _LOCATION_ID.value, _CORPUS_ID.value
  )
  asset_name = visionai_v1.WarehouseClient.asset_path(
      _PROJECT_NUMBER.value,
      _LOCATION_ID.value,
      _CORPUS_ID.value,
      _ASSET_ID.value,
  )
  warehouse_endpoint = channel.get_warehouse_service_endpoint(
      channel.Environment[_ENV.value]
  )
  warehouse_client = visionai_v1.WarehouseClient(
      client_options={"api_endpoint": warehouse_endpoint}
  )
  index_name = vod_corpus.index_corpus(
      warehouse_client, corpus_name, _INDEX_DISPLAY_NAME.value
  )
  ml_config = transformer_factory.MlTransformersCreationConfig(
      run_embedding=True,
      speech_transformer_init_config=speech_transformer.SpeechTransformerInitConfig(
          corpus_name=corpus_name, language_code="en-US"
      ),
      ocr_transformer_init_config=ocr_transformer.OcrTransformerInitConfig(
          corpus_name=corpus_name,
          env=channel.Environment[_ENV.value],
      ),
  )
  ml_transformers = transformer_factory.create_ml_transformers(
      warehouse_client, ml_config
  )
  asset_indexing_transformer = ait.AssetIndexingTransformer(
      warehouse_client, index_name
  )
  vod_asset.transform_single_asset(
      asset_name,
      ml_transformers,
      asset_indexing_transformer,
  )
  all_transformers = ml_transformers + [asset_indexing_transformer]
  for transformer in all_transformers:
    transformer.teardown()

if __name__ == "__main__":
  app.run(main)
