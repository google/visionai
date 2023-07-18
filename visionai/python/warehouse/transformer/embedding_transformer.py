# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Transformer to generate internal embeddings that power warehouse search.

EmbeddingTransformer will call warehouse AnalyzeAsset API to generate
embeddings.
"""

import logging
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer.transform_progress import LroTransformProgress
from visionai.python.warehouse.transformer.transform_progress import TransformProgress
from visionai.python.warehouse.transformer.transformer_interface import TransformerInterface


_logger = logging.getLogger(__name__)


class EmbeddingTransformer(TransformerInterface):
  """Generates warehouse embeddings."""

  def __init__(self, warehouse_client: visionai_v1.WarehouseClient):
    """Constructor.

    Args:
      warehouse_client: the client to talk to warehouse.
    """
    super().__init__(warehouse_client)
    _logger.info("Initialize EmbeddingTransformer.")

  def initialize(self) -> None:
    """Initialize the transformer.
    """

  def transform(self, asset_name: str) -> TransformProgress:
    """Performs transform for the given asset resource.

    Args:
      asset_name: the asset name that this transform will operate on.

    Returns:
      LroTransformProgress used to poll status for the transformation.
    """
    request = visionai_v1.AnalyzeAssetRequest(
        name=asset_name,
    )
    operation = self.warehouse_client.analyze_asset(request=request)
    return LroTransformProgress(lro=operation)
