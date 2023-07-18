# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Transformer to do OCR per asset.

OcrTransformer will generate OCR text detections and write to warehouse.
"""

import dataclasses
import logging
from typing import Dict

from google.api_core import exceptions

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.gapic.visionai.visionai_v1 import DataSchemaDetails
from visionai.python.lva import client
from visionai.python.lva import graph
from visionai.python.net import channel
from visionai.python.ops import gen_ops
from visionai.python.warehouse.transformer.transform_progress import LvaTransformProgress
from visionai.python.warehouse.transformer.transform_progress import TransformProgress
from visionai.python.warehouse.transformer.transformer_interface import TransformerInterface

_logger = logging.getLogger(__name__)

_DEFAULT_CLUSTER = "application-cluster-0"
_OCR_WAREHOUSE_ANALYZE_ID_PREFIX = "ocr-warehouse-"
_PROJECT_NUMBER = "project_number"
_LOCATION = "location"
# Constants to construct data schema for OCR.
_OCR_ANNOTATIONS_DATA_SCHEMA_KEY = "text"
_OCR_SEARCH_CRITERIA_KEY = "text"
_TEXT = "text"
_CONFIDENCE = "confidence"
_TIMESTAMP = "timestamp-microseconds"
_X_MIN = "x-min"
_X_MAX = "x-max"
_Y_MIN = "y-min"
_Y_MAX = "y-max"
_BOUNDING_BOX = "bounding-box"
_FRAME_INFO = "frame-info"


def _construct_ocr_data_schema() -> visionai_v1.DataSchemaDetails:
  """Constructs data schema for OCR annotations.

  Returns:
    data schema for OCR annotations.
  """
  data_schemas = {}
  data_schemas[_TEXT] = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.STRING,
      search_strategy=DataSchemaDetails.SearchStrategy(
          search_strategy_type=DataSchemaDetails.SearchStrategy.SearchStrategyType.SMART_SEARCH
      ),
  )
  float_data_schema_detail = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.FLOAT,
  )
  data_schemas[_CONFIDENCE] = float_data_schema_detail
  bounding_box = {}
  bounding_box[_X_MIN] = bounding_box[_X_MAX] = bounding_box[_Y_MIN] = (
      bounding_box[_Y_MAX]
  ) = float_data_schema_detail
  frame_info = {}
  frame_info[_BOUNDING_BOX] = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.CUSTOMIZED_STRUCT,
      customized_struct_config=DataSchemaDetails.CustomizedStructConfig(
          field_schemas=bounding_box
      ),
  )
  frame_info[_TIMESTAMP] = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.INTEGER,
  )
  data_schemas[_FRAME_INFO] = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.LIST,
      list_config=DataSchemaDetails.ListConfig(
          value_schema=DataSchemaDetails(
              type_=DataSchemaDetails.DataType.CUSTOMIZED_STRUCT,
              granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
              customized_struct_config=DataSchemaDetails.CustomizedStructConfig(
                  field_schemas=frame_info
              ),
          )
      ),
  )
  return DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.CUSTOMIZED_STRUCT,
      customized_struct_config=DataSchemaDetails.CustomizedStructConfig(
          field_schemas=data_schemas
      ),
  )


@dataclasses.dataclass
class OcrTransformerInitConfig:
  """The initialize config for ocr transform.

  Attributes:
    corpus_name: the resource name of the corpus for which assets will be
      transformed.
    ocr_data_schema_key: Optional. The key used for ocr text detection
      annotations. Use default "text" as the key if not specified.
    ocr_search_criteria_key: Optional. The key used for OCR annotations. By
      default, it is "text".
    language_hints: Optional. the language code that this video is in. Formats
      language code concat with '+'. for example, en+zh. Prefers not specify if
      unsure.
    cluster_id: the cluster used by StreamService. Default
      "application-cluster-0" if not specified.
    env: the env used. By default, uses production if not specified.
  """

  corpus_name: str
  ocr_data_schema_key: str = _OCR_ANNOTATIONS_DATA_SCHEMA_KEY
  ocr_search_criteria_key: str = _OCR_SEARCH_CRITERIA_KEY
  language_hints: str = ""
  cluster_id: str = _DEFAULT_CLUSTER
  env: channel.Environment = channel.Environment.PROD


# TODO(zhangxiaotian): use OCR graph in recipes when that is available
# (b/285896930).
class OcrGraph:
  """OcrGraph constructs graph to run OCR and sink to warehouse."""

  _SOURCE_ANALYZER = "warehouse_video_source"
  _SINK_ANALYZER = "text_detection_warehouse_sink"
  _TRANSFORMER_ANALYZER = "text_detection"
  _ASSET_NAME_ATTRIBUTE = "asset_name"
  _INPUT_WAREHOUSE_ENDPOINT_ATTRIBUTE = "warehouse_endpoint"
  _OCR_SERVICE_ADDRESS_ATTRIBUTE = "ocr_service_address"
  _LANGUAGE_HINTS_ATTRIBUTE = "language_hints"
  _OCR_SINK_DATA_SCHEMA_ID_ATTRIBUTE = "text_detection_data_schema_id"

  def get_ocr_overrides(self, asset_name: str) -> Dict[str, str]:
    """Produces the attribute overrides for ocr."""
    if not asset_name:
      raise ValueError("The asset name cannot be empty.")

    return {
        self._SOURCE_ANALYZER + ":" + self._ASSET_NAME_ATTRIBUTE: asset_name,
        self._SINK_ANALYZER + ":" + self._ASSET_NAME_ATTRIBUTE: asset_name,
    }

  def create_graph(
      self,
      env: channel.Environment,
      language_hints: str,
      ocr_data_schema_key: str,
  ) -> graph.Graph:
    """Creates a pre-made graph used for OCR.

    Args:
      env: the enviornment.
      language_hints: language hints for OCR.
      ocr_data_schema_key: the data schema key for OCR annotations in warehouse.

    Returns:
      A `Graph` that runs OCR and writes annotations to warehouse.
    """
    g = graph.Graph()
    warehouse_video_source_output = gen_ops.warehouse_video_source(
        warehouse_endpoint=channel.get_warehouse_service_endpoint(env),
        name=self._SOURCE_ANALYZER,
        g=g,
    )
    ocr_output = gen_ops.text_detection(
        warehouse_video_source_output,
        ocr_service_address=self._get_ocr_server_address(env),
        language_hints=language_hints,
        name=self._TRANSFORMER_ANALYZER,
        g=g,
    )
    gen_ops.text_detection_warehouse_sink(
        annotation=ocr_output,
        warehouse_endpoint=channel.get_warehouse_service_endpoint(env),
        text_detection_data_schema_id=ocr_data_schema_key,
        name=self._SINK_ANALYZER,
        g=g,
    )
    return g

  def _get_ocr_server_address(self, env: channel.Environment) -> str:
    """Gets the ocr proxy endpoint according to specified environment.

    Args:
      env: Environment, either AUTOPUSH, STAGING OR PROD.

    Returns:
      The ocr proxy server endpoint to talk to.
    """
    # TODO(zhangxiaotian): switch to regional endpoints when they are ready.
    if env == channel.Environment.AUTOPUSH:
      return "autopush-visionai-pa.sandbox.googleapis.com"
    if env == channel.Environment.STAGING:
      return "staging-visionai-pa.sandbox.googleapis.com"
    if env == channel.Environment.PROD:
      return "visionai-pa.googleapis.com"

    _logger.error("Unsupported environment %s", env)
    raise ValueError("Invalid environment.")


class OcrTransformer(TransformerInterface):
  """Runs ocr text detection for the given asset."""

  def __init__(
      self,
      init_config: OcrTransformerInitConfig,
      warehouse_client: visionai_v1.WarehouseClient,
  ):
    """Constructor.

    Args:
      init_config: the config used for initialized the transformer.
      warehouse_client: the client to talk to warehouse.
    """
    super().__init__(warehouse_client)
    path = visionai_v1.WarehouseClient.parse_corpus_path(
        init_config.corpus_name
    )
    self._project_number = path[_PROJECT_NUMBER]
    self._location = path[_LOCATION]
    self._connection_options = channel.ConnectionOptions(
        project_id=self._project_number,
        location_id=self._location,
        cluster_id=init_config.cluster_id,
        env=init_config.env,
    )
    self._init_config = init_config

  def initialize(self) -> None:
    """Initialize the transformer."""
    _logger.info(
        "Initialize OcrTransformer using config %s.", self._init_config
    )
    # Sets up data schema.
    request = visionai_v1.CreateDataSchemaRequest(
        parent=self._init_config.corpus_name,
        data_schema=visionai_v1.DataSchema(
            key=self._init_config.ocr_data_schema_key,
            schema_details=_construct_ocr_data_schema(),
        ),
    )
    try:
      response = self.warehouse_client.create_data_schema(request)
    except exceptions.AlreadyExists:
      logging.warning("Data schema %s already exists.", request.data_schema.key)
    else:
      _logger.info("Create data schema %s", response)

    try:
      search_config = self.warehouse_client.create_search_config(
          visionai_v1.CreateSearchConfigRequest(
              search_config=visionai_v1.SearchConfig(
                  search_criteria_property=visionai_v1.SearchCriteriaProperty(
                      mapped_fields={
                          "{annotation_key}.{text_field}".format(
                              annotation_key=self._init_config.ocr_data_schema_key,
                              text_field=_TEXT,
                          )
                      }
                  )
              ),
              search_config_id=self._init_config.ocr_search_criteria_key,
              parent=self._init_config.corpus_name,
          )
      )
    except exceptions.AlreadyExists:
      logging.warning(
          "Search config %s already exists.", request.data_schema.key
      )
    else:
      _logger.info("Search config %s", search_config)

    ocr_graph = OcrGraph()
    g = ocr_graph.create_graph(
        self._init_config.env,
        self._init_config.language_hints,
        self._init_config.ocr_data_schema_key,
    )
    self._analysis = client.get_or_create_analysis(
        self._connection_options,
        g,
        self._construct_lva_analyze_id(),
    )
    _logger.info("created analysis name: %s", self._analysis.name)

  def transform(self, asset_name: str) -> TransformProgress:
    """Performs transform for the given asset resource.

    Args:
      asset_name: the asset name that this transform will operate on.

    Returns:
      LvaTransformProgress used to poll status for the transformation.
    """
    process = client.create_process(
        self._connection_options,
        self._analysis.analysis_id,
        OcrGraph().get_ocr_overrides(asset_name),
    )
    return LvaTransformProgress(self._connection_options, process.process_id)

  def teardown(self) -> bool:
    _logger.info("Teardown OCR transformer.")
    list_filter = (
        'analysis="%s" AND (run_status.state="RUNNING" OR '
        ' run_status.state="INITIALIZING" OR run_status.state="PENDING")'
        % visionai_v1.LiveVideoAnalyticsClient.analysis_path(
            self._connection_options.project_id,
            self._connection_options.location_id,
            self._connection_options.cluster_id,
            self._analysis.analysis_id,
        )
    )
    processes = client.list_processes(self._connection_options, list_filter)

    if processes:
      _logger.warning(
          "Skip tearing down OCR transformer since there are other processes"
          " not complete."
      )
      return False
    client.delete_analysis(self._connection_options, self._analysis.analysis_id)
    return True

  def _construct_lva_analyze_id(self):
    return (
        _OCR_WAREHOUSE_ANALYZE_ID_PREFIX
        + self._init_config.ocr_data_schema_key
        + "-lang"
        + self._init_config.language_hints.replace("+", "-")
    )
