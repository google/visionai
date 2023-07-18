# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Library to interfact with warehouse VOD corpus."""

import logging

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import corpus_indexing_transformer as cit

_logger = logging.getLogger(__name__)


def create_corpus(
    warehouse_client: visionai_v1.WarehouseClient,
    project_number: int,
    location_id: str,
    corpus_display_name: str,
    corpus_description: str,
    timeout: int = 7200,
) -> visionai_v1.Corpus:
  """Creates corpus with given corpus metadata.

  Args:
    warehouse_client: the client to talk to warehouse.
    project_number: the project number.
    location_id: the location id.
    corpus_display_name: the display name for the corpus.
    corpus_description: the description for the corpus.
    timeout: a timeout in seconds for waiting for the corpus creation. By
      defualt, waits for 2h.

  Returns:
    The created VOD corpus.
  Raises:
      google.api_core.GoogleAPICallError: If the operation errors or if
          the timeout is reached before the operation completes.
  """
  search_capability = visionai_v1.SearchCapability(
      type_=visionai_v1.SearchCapability.Type.EMBEDDING_SEARCH
  )
  operation = warehouse_client.create_corpus(
      visionai_v1.CreateCorpusRequest(
          parent=f"projects/{project_number}/locations/{location_id}",
          corpus=visionai_v1.Corpus(
              display_name=corpus_display_name,
              description=corpus_description,
              type_=visionai_v1.Corpus.Type.VIDEO_ON_DEMAND,
              search_capability_setting=visionai_v1.SearchCapabilitySetting(
                  search_capabilities=[search_capability]
              ),
          ),
      )
  )
  _logger.info("Wait for corpus operation: %s", operation.operation)

  _logger.info("Created corpus %s", operation.result(timeout=timeout))
  return operation.result()


def index_corpus(
    warehouse_client: visionai_v1.WarehouseClient,
    corpus_name: str,
    index_display_name: str,
    timeout: int = 7200,
) -> str:
  """Sets up the corpus.

  Args:
    warehouse_client: the client to talk to warehouse.
    corpus_name: the corpus name resource.
    index_display_name: the display name of the index to be created.
    timeout: Optional. The timeout to wait for corpus creation. If not
      specified, wait for 2h.

  Returns:
    Index name created for the corpus.
  """
  index = visionai_v1.Index(display_name=index_display_name)
  corpus_indexing_transformer = cit.CorpusIndexingTransformer(
      warehouse_client, index
  )
  corpus_indexing_transform_progress = corpus_indexing_transformer.transform(
      corpus_name
  )
  _logger.info(
      "Wait for corpus index creation operation: %s",
      corpus_indexing_transform_progress.operation().name,
  )
  index = corpus_indexing_transform_progress.result(timeout=timeout)
  _logger.info("Corpus index creation operation result: %s", index)
  return index.name
