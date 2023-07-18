# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Library for interacting with warehouse assets."""

import logging
from typing import Sequence

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import asset_indexing_transformer as ait
from visionai.python.warehouse.transformer import transform_progress
from visionai.python.warehouse.transformer import transformer_interface

_logger = logging.getLogger(__name__)


def create_and_upload_asset(
    warehouse_client: visionai_v1.WarehouseClient,
    gcs_file: str,
    corpus_name: str,
) -> str:
  """Ingests for a single GCS file to warehouse.

  Args:
    warehouse_client: the client to talk to warehouse.
    gcs_file: the gcs file to be ingested.
    corpus_name: the corpus name.

  Returns:
    The name of the asset created.
  """
  # Creates asset
  response = warehouse_client.create_asset(
      visionai_v1.CreateAssetRequest(
          parent=corpus_name,
      )
  )
  _logger.info("Creates asset %s.", response.name)
  # Uploads asset
  upload_asset_op = warehouse_client.upload_asset(
      visionai_v1.UploadAssetRequest(
          name=response.name,
          asset_source=visionai_v1.AssetSource(
              asset_gcs_source=visionai_v1.AssetSource.AssetGcsSource(
                  gcs_uri=gcs_file
              )
          ),
      )
  )
  _logger.info("Wait for UploadAsset %s.", upload_asset_op.operation.name)
  upload_asset_op.result()
  _logger.info("Uploads asset %s to %s.", gcs_file, response.name)
  return response.name


def transform_single_asset(
    asset_name: str,
    ml_transformers: Sequence[transformer_interface.TransformerInterface],
    asset_indexing_transformer: ait.AssetIndexingTransformer,
) -> None:
  """Transforms a single asset.

  Args:
    asset_name: the asset name resource.
    ml_transformers: a list of ml transformers to run before indexing the asset.
    asset_indexing_transformer: transform to index the asset.
  """
  transform_progresses = []
  for transformer in ml_transformers:
    transform_progresses.append(transformer.transform(asset_name))
    _logger.info(
        "transform id %s for asset %s",
        transform_progresses[-1].get_identifier(),
        asset_name,
    )
  if transform_progresses:
    transform_progress.CombinedTransformProgress(transform_progresses).result()

  if asset_indexing_transformer is not None:
    asset_indexing_transform_progress = asset_indexing_transformer.transform(
        asset_name
    )
    _logger.info("Wait for asset indexing transform.")
    asset_indexing_transform_progress.result()

  _logger.info("Successfully transformed for asset %s", asset_name)
