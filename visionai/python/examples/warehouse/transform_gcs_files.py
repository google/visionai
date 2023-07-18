# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Python SDK example to manage & index given list of gcs files using warehouse.

It demonstrates:
1. Creates VOD corpus and streams cluster if needed.
2. Creates and uploads assets for specified gcs files.
3. Creates index for the corpus.
4. Creates ML transformers and asset indexing transformer.
5. Runs the transformers for the assets.
"""

import concurrent
import logging

from absl import app
from absl import flags

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.net import channel
from visionai.python.streams import client as streams_client
from visionai.python.warehouse.transformer import asset_indexing_transformer as ait
from visionai.python.warehouse.transformer import ocr_transformer
from visionai.python.warehouse.transformer import speech_transformer
from visionai.python.warehouse.transformer import transformer_factory
from visionai.python.warehouse.utils import vod_asset
from visionai.python.warehouse.utils import vod_corpus
from visionai.python.warehouse.utils import vod_index_endpoint

_PROJECT_NUMBER = flags.DEFINE_integer(
    "project_number", None, "Project number."
)
_LOCATION_ID = flags.DEFINE_string("location_id", "us-central1", "Location id.")
_CORPUS_DISPLAY_NAME = flags.DEFINE_string(
    "corpus_display_name", "Demo Corpus", "Corpus display name."
)
_CORPUS_DISCRIPTION = flags.DEFINE_string(
    "corpus_description",
    "Demo Corpus to interact with warehouse",
    "Corpus description.",
)
_CORPUS_ID = flags.DEFINE_string(
    "corpus_id", None, "If specified, use existing VOD corpus."
)
_GCS_FILES = flags.DEFINE_list(
    "gcs_files",
    [
        "gs://cloud-samples-data/video/animals.mp4",
        "gs://cloud-samples-data/video/googlework_short.mp4",
        "gs://cloud-samples-data/video/chicago.mp4",
        (
            "gs://cloud-samples-data/video/Machine Learning Solving Problems"
            " Big, Small, and Prickly.mp4"
        ),
        "gs://cloud-samples-data/video/JaneGoodall.mp4",
    ],
    "GCS files.",
)
_ENV = flags.DEFINE_enum(
    "env",
    "PROD",
    ["AUTOPUSH", "STAGING", "PROD"],
    "The environment.",
)
_USE_EXISTING_CLUSTER = flags.DEFINE_bool(
    "use_existing_cluster",
    False,
    "Whether create a new cluster. If this is false, and cluster flag is None,"
    " it will use the cluster created from UI .",
)
_CLUSTER_ID = flags.DEFINE_string(
    "cluster_id",
    None,
    "The cluster. If not specified, by default using the cluster created"
    " via UI.",
)
_DEPLOYED_INDEX_ID = flags.DEFINE_string(
    "deployed_index_id",
    None,
    "If specified, use existing index instead of creating and deploying a new"
    " one.",
)
_INDEX_DISPLAY_NAME = flags.DEFINE_string(
    "index_display_name", "Demo Index", "Index display name."
)
_INDEX_ENDPOINT_DISPLAY_NAME = flags.DEFINE_string(
    "index_endpoint_display_name",
    "Demo index endpoint",
    "Display name for the index endpoint.",
)
_QUERY = flags.DEFINE_string("query", None, "Search query.")
_CLEAN_UP_ASSETS = flags.DEFINE_bool(
    "clean_up_assets", False, "Whether clean up assets."
)
_CLEAN_UP_CORPUS = flags.DEFINE_bool(
    "clean_up_corpus", False, "Whether clean up corpus."
)
_CLEAN_UP_INDEX = flags.DEFINE_bool(
    "clean_up_index", False, "Whether clean up index and index endpoint."
)
_CLEAN_UP_CLUSTER = flags.DEFINE_bool(
    "clean_up_cluster", False, "Whether clean up cluster."
)

_logger = logging.getLogger(__name__)


def main(unused_argv) -> None:
  # Creates a warehouse client to talk with warehouse.
  warehouse_endpoint = channel.get_warehouse_service_endpoint(
      channel.Environment[_ENV.value]
  )
  warehouse_client = visionai_v1.WarehouseClient(
      client_options={"api_endpoint": warehouse_endpoint}
  )

  # Creates a cluster.
  if not _USE_EXISTING_CLUSTER.value:
    if _CLUSTER_ID.value is None:
      raise ValueError("Cluster must be specified when creating new cluster.")
    streams_client.create_cluster(
        channel.ConnectionOptions(
            _PROJECT_NUMBER.value,
            _LOCATION_ID.value,
            _CLUSTER_ID.value,
            channel.Environment[_ENV.value],
        )
    )

  # Creates a corpus.
  if _CORPUS_ID.value is None:
    corpus_name = vod_corpus.create_corpus(
        warehouse_client,
        _PROJECT_NUMBER.value,
        _LOCATION_ID.value,
        _CORPUS_DISPLAY_NAME.value,
        _CORPUS_DISCRIPTION.value,
    ).name
  else:
    corpus_name = visionai_v1.WarehouseClient.corpus_path(
        _PROJECT_NUMBER.value, _LOCATION_ID.value, _CORPUS_ID.value
    )

  # Creates and uploads assets for specified gcs files.
  executor = concurrent.futures.ThreadPoolExecutor(max_workers=8)
  new_asset_futures = []
  for gcs_file in _GCS_FILES.value:
    new_asset_futures.append(
        executor.submit(
            vod_asset.create_and_upload_asset,
            warehouse_client,
            gcs_file,
            corpus_name,
        )
    )
  done_or_error, _ = concurrent.futures.wait(
      new_asset_futures, return_when="ALL_COMPLETED"
  )
  asset_names = []
  for done_future in done_or_error:
    try:
      _logger.info("Create and upload asset succeeded %s", done_future.result())
      asset_names.append(done_future.result())
    except Exception as e:
      _logger.exception(e)

  if _DEPLOYED_INDEX_ID.value is None:
    # Creates index for the corpus.
    index_name = vod_corpus.index_corpus(
        warehouse_client, corpus_name, _INDEX_DISPLAY_NAME.value
    )
    # Creates index endpoint and deploys the created index above to the index
    # endpoint.
    index_endpoint_name = vod_index_endpoint.create_index_endpoint(
        warehouse_client,
        _PROJECT_NUMBER.value,
        _LOCATION_ID.value,
        _INDEX_ENDPOINT_DISPLAY_NAME.value,
    ).name
    deploy_operation = warehouse_client.deploy_index(
        visionai_v1.DeployIndexRequest(
            index_endpoint=index_endpoint_name,
            deployed_index=visionai_v1.DeployedIndex(
                index=index_name,
            ),
        )
    )
    _logger.info(
        "Wait for index to be deployed %s.", deploy_operation.operation.name
    )
    # Wait for the deploy index operation. Depends on the data size to be
    # indexed, the timout may need to be increased.
    deploy_operation.result(timeout=7200)
    _logger.info("Index is deployed.")
  else:
    index_name = "%s/indexes/%s" % (corpus_name, _DEPLOYED_INDEX_ID.value)
    index = warehouse_client.get_index(
        visionai_v1.GetIndexRequest(name=index_name)
    )
    if index.state != visionai_v1.Index.State.CREATED:
      _logger.critical("Invalid index. The index state must be Created.")
    if not index.deployed_indexes:
      _logger.critical("Invalid index. The index must be deployed.")
    index_endpoint_name = index.deployed_indexes[0].index_endpoint

  # Creates ML transformers.
  ocr_config = ocr_transformer.OcrTransformerInitConfig(
      corpus_name=corpus_name,
      env=channel.Environment[_ENV.value],
  )
  if _CLUSTER_ID.value:
    ocr_config.cluster_id = _CLUSTER_ID.value
  ml_config = transformer_factory.MlTransformersCreationConfig(
      run_embedding=True,
      speech_transformer_init_config=speech_transformer.SpeechTransformerInitConfig(
          corpus_name=corpus_name, language_code="en-US"
      ),
      ocr_transformer_init_config=ocr_config,
  )
  ml_transformers = transformer_factory.create_ml_transformers(
      warehouse_client, ml_config
  )
  # Creates indexing transformer to index assets.
  asset_indexing_transformer = ait.AssetIndexingTransformer(
      warehouse_client, index_name
  )
  # Runs the transformers for the assets.
  futures = []
  for asset_name in asset_names:
    futures.append(
        executor.submit(
            vod_asset.transform_single_asset,
            asset_name,
            ml_transformers,
            asset_indexing_transformer,
        )
    )
  done_or_error, _ = concurrent.futures.wait(
      futures, return_when="ALL_COMPLETED"
  )
  # TODO(zhangxiaotian): improve this to log which asset the error is from.

  for future in done_or_error:
    try:
      future.result()
    except Exception as e:
      _logger.exception(e)

  all_transformers = ml_transformers + [asset_indexing_transformer]
  for transformer in all_transformers:
    transformer.teardown()

  # Search
  search_response = warehouse_client.search_index_endpoint(
      visionai_v1.SearchIndexEndpointRequest(
          index_endpoint=index_endpoint_name, text_query=_QUERY.value
      )
  )
  _logger.info("Search response: %s", search_response)

  # Cleans up (index, assets, corpus, cluster)
  if _CLEAN_UP_ASSETS.value:
    for asset_name in asset_names:
      warehouse_client.delete_asset(
          visionai_v1.DeleteAssetRequest(name=asset_name)
      )
      _logger.info("Deleted asset %s", asset_name)

  if _CLEAN_UP_INDEX.value:
    undeploy_operation = warehouse_client.undeploy_index(
        visionai_v1.UndeployIndexRequest(index_endpoint=index_endpoint_name)
    )
    _logger.info(
        "Wait for index to be undeployed %s.",
        undeploy_operation.operation.name,
    )
    # Wait for the undeploy index operation.
    undeploy_operation.result(timeout=1800)
    _logger.info("Index is undeployed.")
    warehouse_client.delete_index(
        visionai_v1.DeleteIndexRequest(name=index_name)
    )
    _logger.info("Deleted index %s", index_name)
    warehouse_client.delete_index_endpoint(
        visionai_v1.DeleteIndexEndpointRequest(name=index_endpoint_name)
    )
    _logger.info("Deleted index endpoint %s", index_endpoint_name)

  if _CLEAN_UP_CORPUS.value:
    warehouse_client.delete_corpus(
        visionai_v1.DeleteCorpusRequest(name=corpus_name)
    )
    _logger.info("Deleted corpus %s", corpus_name)

  if _CLEAN_UP_CLUSTER.value:
    if _CLUSTER_ID.value is None:
      _logger.warning(
          "Can't clean up cluster since cluster_id is not specified."
      )
    else:
      streams_client.delete_cluster(
          channel.ConnectionOptions(
              _PROJECT_NUMBER.value,
              _LOCATION_ID.value,
              _CLUSTER_ID.value,
              channel.Environment[_ENV.value],
          ),
      )


if __name__ == "__main__":
  app.run(main)
