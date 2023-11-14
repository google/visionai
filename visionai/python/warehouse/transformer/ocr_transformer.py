# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Transformer to do OCR per asset.

OcrTransformer will generate OCR text detections and write to warehouse.
"""

import dataclasses
import functools
import logging
from typing import Dict, Optional, Sequence
from google.api_core import exceptions
from google.api_core import operation
from google.cloud import videointelligence_v1
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.gapic.visionai.visionai_v1 import AnnotationCustomizedStruct
from visionai.python.gapic.visionai.visionai_v1 import AnnotationList
from visionai.python.gapic.visionai.visionai_v1 import AnnotationValue
from visionai.python.gapic.visionai.visionai_v1 import DataSchemaDetails
from visionai.python.gapic.visionai.visionai_v1 import Partition
from visionai.python.gapic.visionai.visionai_v1 import UserSpecifiedAnnotation
from visionai.python.lva import client
from visionai.python.lva import graph
from visionai.python.net import channel
from visionai.python.ops import gen_ops
from visionai.python.warehouse.transformer import transform_progress
from visionai.python.warehouse.transformer.transform_progress import LvaTransformProgress
from visionai.python.warehouse.transformer.transform_progress import TransformProgress
from visionai.python.warehouse.transformer.transform_progress import WaitAndWriteWarehouseTransformProgress
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
_FRAME_INFO = "frame-info"
_TIMESTAMP = "timestamp-microseconds"
_NORMALIZED_BOUNDING_POLY = "normalized-bounding-poly"
_X = "x"
_Y = "y"
# Constants to construct data schema when not using video intelligence.
_OCR_ANNOTATIONS_DATA_SCHEMA_KEY_USING_LVA = "text-lva"
_BOUNDING_BOX = "bounding-box"
_X_MIN = "x-min"
_X_MAX = "x-max"
_Y_MIN = "y-min"
_Y_MAX = "y-max"


def _construct_ocr_data_schema(
    use_video_intelligence: bool,
) -> visionai_v1.DataSchemaDetails:
  """Constructs data schema for OCR annotations.

  Args:
    use_video_intelligence: Wether use video intelligence for OCR.

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
  if use_video_intelligence:
    normalized_vertex = {}
    normalized_vertex[_X] = normalized_vertex[_Y] = float_data_schema_detail
    frame_info = {}
    frame_info[_NORMALIZED_BOUNDING_POLY] = DataSchemaDetails(
        granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
        type_=DataSchemaDetails.DataType.LIST,
        list_config=DataSchemaDetails.ListConfig(
            value_schema=DataSchemaDetails(
                type_=DataSchemaDetails.DataType.CUSTOMIZED_STRUCT,
                granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
                customized_struct_config=DataSchemaDetails.CustomizedStructConfig(
                    field_schemas=normalized_vertex
                ),
            )
        ),
    )
  else:
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


def _construct_annotations(
    data_schema_key: str,
    annotate_video_response: videointelligence_v1.AnnotateVideoResponse,
) -> Sequence[visionai_v1.Annotation]:
  """Constructs warehouse annotations based on the response from Video Intelligence.

  Args:
    data_schema_key: the key of the data schema.
    annotate_video_response: the response from AnnotateVideo API.

  Returns:
    A list of annotations.
  """
  annotations = []
  for annotation_result in annotate_video_response.annotation_results:
    for text_annotation in annotation_result.text_annotations:
      for segment in text_annotation.segments:
        start_time = segment.segment.start_time_offset
        end_time = segment.segment.end_time_offset

        frame_info = []
        for frame in segment.frames:
          normalized_vertices = []
          for vertex in frame.rotated_bounding_box.vertices:
            normalized_vertices.append(
                AnnotationValue(
                    customized_struct_value=AnnotationCustomizedStruct(
                        elements={
                            _X: AnnotationValue(float_value=vertex.x),
                            _Y: AnnotationValue(float_value=vertex.y),
                        }
                    )
                )
            )
          frame_info.append(
              AnnotationValue(
                  customized_struct_value=AnnotationCustomizedStruct(
                      elements={
                          _TIMESTAMP: AnnotationValue(
                              int_value=frame.time_offset.seconds * 1000000
                              + frame.time_offset.microseconds
                          ),
                          _NORMALIZED_BOUNDING_POLY: AnnotationValue(
                              list_value=AnnotationList(
                                  values=normalized_vertices
                              )
                          ),
                      }
                  )
              )
          )
        annotation = visionai_v1.Annotation()
        annotation.user_specified_annotation = UserSpecifiedAnnotation(
            key=data_schema_key,
            partition=Partition(
                relative_temporal_partition=Partition.RelativeTemporalPartition(
                    start_offset=start_time, end_offset=end_time
                )
            ),
            value=AnnotationValue(
                customized_struct_value=AnnotationCustomizedStruct(
                    elements={
                        _TEXT: AnnotationValue(str_value=text_annotation.text),
                        _CONFIDENCE: AnnotationValue(
                            float_value=segment.confidence
                        ),
                        _FRAME_INFO: AnnotationValue(
                            list_value=AnnotationList(
                                values=frame_info,
                            )
                        ),
                    }
                )
            ),
        )
        annotations.append(annotation)
  return annotations


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
      "application-cluster-0" if not specified. Only used if
      use_video_intelligence is set to False.
    env: the env used. By default, uses production if not specified.
    use_video_intelligence: if set to True, calls Video Intelligence to perform
      OCR. Otherwise uses Vision AI analyzer to process the video (which
      requires a cluster to be created before).
  """

  corpus_name: str
  ocr_data_schema_key: str = None
  ocr_search_criteria_key: str = _OCR_SEARCH_CRITERIA_KEY
  language_hints: str = ""
  cluster_id: str = _DEFAULT_CLUSTER
  env: channel.Environment = channel.Environment.PROD
  use_video_intelligence: bool = True

  def __post_init__(self):
    if self.ocr_data_schema_key is None:
      self.ocr_data_schema_key = (
          _OCR_ANNOTATIONS_DATA_SCHEMA_KEY
          if self.use_video_intelligence
          else _OCR_ANNOTATIONS_DATA_SCHEMA_KEY_USING_LVA
      )


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
      video_intelligence_client: Optional[
          videointelligence_v1.VideoIntelligenceServiceClient
      ] = None,
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
    self._init_config = init_config
    if init_config.use_video_intelligence:
      self.video_intelligence_client = (
          video_intelligence_client
          if video_intelligence_client is not None
          else videointelligence_v1.VideoIntelligenceServiceClient()
      )
    else:
      self._connection_options = channel.ConnectionOptions(
          project_id=self._project_number,
          location_id=self._location,
          cluster_id=init_config.cluster_id,
          env=init_config.env,
      )

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
            schema_details=_construct_ocr_data_schema(
                self._init_config.use_video_intelligence
            ),
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

    if not self._init_config.use_video_intelligence:
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
      TransformProgress used to poll status for the transformation.
    """
    if self._init_config.use_video_intelligence:
      return self.transform_by_videointelligence(asset_name)
    else:
      return self.transform_by_lva(asset_name)

  def transform_by_lva(self, asset_name: str) -> TransformProgress:
    """Performs transform for the given asset resource using live video analytics.

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

  def transform_by_videointelligence(
      self, asset_name: str
  ) -> TransformProgress:
    """Performs transform for the given asset resource using video intelligence.

    Args:
      asset_name: the asset name that this transform will operate on.

    Returns:
      WriteWarehouseTransformProgress used to poll status for the
      transformation.
    """
    get_asset_request = visionai_v1.GetAssetRequest(name=asset_name)
    get_asset_response = self.warehouse_client.get_asset(get_asset_request)
    request = videointelligence_v1.AnnotateVideoRequest(
        input_uri=get_asset_response.asset_gcs_source.gcs_uri,
        features=[videointelligence_v1.Feature.TEXT_DETECTION],
    )
    request.video_context.text_detection_config = (
        videointelligence_v1.TextDetectionConfig(
            language_hints=self._init_config.language_hints
        )
    )
    annotate_video_operation = self.video_intelligence_client.annotate_video(
        request=request
    )
    _logger.info("Text detection lro %s", annotate_video_operation.operation)
    annotate_video_operation_with_polling_config = operation.from_gapic(
        annotate_video_operation.operation,
        self.video_intelligence_client.transport.operations_client,
        videointelligence_v1.AnnotateVideoResponse,
        metadata_type=videointelligence_v1.AnnotateVideoProgress,
        retry=transform_progress.DEFAULT_POLLING,
    )
    ocr_transform_progress = WaitAndWriteWarehouseTransformProgress(
        asset_name,
        self.warehouse_client,
        annotate_video_operation_with_polling_config,
        functools.partial(
            _construct_annotations, self._init_config.ocr_data_schema_key
        ),
    )

    return ocr_transform_progress

  def teardown(self) -> bool:
    _logger.info("Teardown OCR transformer.")
    if not self._init_config.use_video_intelligence:
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
      client.delete_analysis(
          self._connection_options, self._analysis.analysis_id
      )
    return True

  def _construct_lva_analyze_id(self):
    return (
        _OCR_WAREHOUSE_ANALYZE_ID_PREFIX
        + self._init_config.ocr_data_schema_key
        + "-lang"
        + self._init_config.language_hints.replace("+", "-")
    )
