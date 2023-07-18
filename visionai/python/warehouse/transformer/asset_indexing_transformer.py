# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Transformer to index a single asset.

AssetIndexingTransformer will call warehouse IndexAsset API to index the asset
to the specified index.
"""

import logging
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer.transform_progress import LroTransformProgress
from visionai.python.warehouse.transformer.transform_progress import TransformProgress
from visionai.python.warehouse.transformer.transformer_interface import TransformerInterface


_logger = logging.getLogger(__name__)


class AssetIndexingTransformer(TransformerInterface):
  """Transformer to index a single asset."""

  def __init__(
      self, warehouse_client: visionai_v1.WarehouseClient, index_name: str
  ):
    """Constructor.

    Args:
      warehouse_client: the client to talk to warehouse.
      index_name: the index name that the asset will be indexed to.
    """
    super().__init__(warehouse_client)
    self._index_name = index_name
    _logger.info("Initialize AssetIndexTransformer.")

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
    request = visionai_v1.IndexAssetRequest(
        name=asset_name,
        index=self._index_name,
    )
    operation = self.warehouse_client.index_asset(request=request)
    _logger.info("Index asset LRO: %s", operation.operation.name)
    return LroTransformProgress(lro=operation)
