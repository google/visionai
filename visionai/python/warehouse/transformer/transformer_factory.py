# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Library to create transformers."""

import dataclasses
import logging
from typing import Optional, Sequence

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import embedding_transformer
from visionai.python.warehouse.transformer import ocr_transformer as ot
from visionai.python.warehouse.transformer import speech_transformer as st
from visionai.python.warehouse.transformer import transformer_interface

_logger = logging.getLogger(__name__)


@dataclasses.dataclass
class MlTransformersCreationConfig:
  run_embedding: bool = True
  speech_transformer_init_config: Optional[st.SpeechTransformerInitConfig] = (
      None
  )
  ocr_transformer_init_config: Optional[ot.OcrTransformerInitConfig] = None


def create_ml_transformers(
    warehouse_client: visionai_v1.WarehouseClient,
    ml_transformers_creation_config: MlTransformersCreationConfig,
) -> Sequence[transformer_interface.TransformerInterface]:
  """Creates transformers.

  Args:
    warehouse_client: the client to talk to warehouse.
    ml_transformers_creation_config: the config for creating ml transformers.

  Returns:
    A list of transformers to run.
  """
  transformers = []
  if ml_transformers_creation_config.run_embedding:
    transformers.append(
        embedding_transformer.EmbeddingTransformer(warehouse_client)
    )
  if ml_transformers_creation_config.speech_transformer_init_config is not None:
    speech_init_config = (
        ml_transformers_creation_config.speech_transformer_init_config
    )
    speech_transformer = st.SpeechTransformer(
        speech_init_config, warehouse_client
    )
    speech_transformer.initialize()
    transformers.append(speech_transformer)
  if ml_transformers_creation_config.ocr_transformer_init_config is not None:
    ocr_init_config = (
        ml_transformers_creation_config.ocr_transformer_init_config
    )
    ocr_transformer = ot.OcrTransformer(ocr_init_config, warehouse_client)
    ocr_transformer.initialize()
    transformers.append(ocr_transformer)
  return transformers
