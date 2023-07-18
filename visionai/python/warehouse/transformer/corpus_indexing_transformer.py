# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Transformer to index a single corpus.

CorpusIndexingTransformer will call warehouse CreateIndex API to index the
corpus.
"""

import logging
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer.transform_progress import LroTransformProgress
from visionai.python.warehouse.transformer.transform_progress import TransformProgress
from visionai.python.warehouse.transformer.transformer_interface import TransformerInterface


_logger = logging.getLogger(__name__)


class CorpusIndexingTransformer(TransformerInterface):
  """Transformer to index a single corpus."""

  def __init__(
      self,
      warehouse_client: visionai_v1.WarehouseClient,
      index: visionai_v1.Index,
  ):
    """Constructor.

    Args:
      warehouse_client: the client to talk to warehouse.
      index: the metadata of the index to be created.
    """
    super().__init__(warehouse_client)
    self._index = index
    _logger.info("Initialize CorpusIndexingTransformer.")

  def initialize(self) -> None:
    """Initialize the transformer.
    """

  def transform(self, corpus_name: str) -> TransformProgress:
    """Performs transform for the given corpus resource.

    Args:
      corpus_name: the corpus name that this transform will operate on.

    Returns:
      LroTransformProgress used to poll status for the transformation.
    """
    request = visionai_v1.CreateIndexRequest(
        parent=corpus_name,
        index=self._index,
    )
    _logger.info("create index %s", request)
    operation = self.warehouse_client.create_index(request=request)
    return LroTransformProgress(lro=operation)
