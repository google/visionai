# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Transformer to do speech transcription per asset.

SpeechTransformer will call video intelligence API to generate speech
transcriptions and write to warehouse.
"""
import dataclasses
import functools
import logging
from typing import Optional, Sequence
from google.api_core import exceptions
from google.api_core import operation
from google.cloud import videointelligence_v1

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.gapic.visionai.visionai_v1 import AnnotationCustomizedStruct
from visionai.python.gapic.visionai.visionai_v1 import AnnotationValue
from visionai.python.gapic.visionai.visionai_v1 import DataSchemaDetails
from visionai.python.gapic.visionai.visionai_v1 import Partition
from visionai.python.gapic.visionai.visionai_v1 import UserSpecifiedAnnotation
from visionai.python.warehouse.transformer import transform_progress
from visionai.python.warehouse.transformer.transform_progress import WaitAndWriteWarehouseTransformProgress
from visionai.python.warehouse.transformer.transform_progress import TransformProgress
from visionai.python.warehouse.transformer.transformer_interface import TransformerInterface


_logger = logging.getLogger(__name__)
_SPEECH_ANNOTATIONS_DATA_SCHEMA_KEY = "speech"
_SPEECH_TRANSCRIPT_SEARCH_CRITERIA_KEY = "speech"
_TRANSCRIPT = "transcript"
_CONFIDENCE = "confidence"


def _construct_speech_data_schema() -> visionai_v1.DataSchemaDetails:
  """Constructs data schema for speech transcription annotations.

  Returns:
    data schema for speech transcription annotations.
  """
  data_schemas = {}
  data_schemas[_TRANSCRIPT] = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.STRING,
      search_strategy=DataSchemaDetails.SearchStrategy(
          search_strategy_type=DataSchemaDetails.SearchStrategy.SearchStrategyType.SMART_SEARCH
      ),
  )
  data_schemas[_CONFIDENCE] = DataSchemaDetails(
      granularity=DataSchemaDetails.Granularity.GRANULARITY_PARTITION_LEVEL,
      type_=DataSchemaDetails.DataType.FLOAT,
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
  annotations = []
  for annotation_result in annotate_video_response.annotation_results:
    for speech_transcription in annotation_result.speech_transcriptions:
      for alternative in speech_transcription.alternatives:
        if not alternative.words:
          _logger.warning("No words info in speech alternative %s", alternative)
          continue
        start_time = alternative.words[0].start_time
        end_time = alternative.words[-1].end_time
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
                        _TRANSCRIPT: AnnotationValue(
                            str_value=alternative.transcript
                        ),
                        _CONFIDENCE: AnnotationValue(
                            float_value=alternative.confidence
                        ),
                    }
                )
            ),
        )
        annotations.append(annotation)
  return annotations


@dataclasses.dataclass
class SpeechTransformerInitConfig:
  """The initialize config for speech transform.

  Attributes:
    corpus_name: the resource name of the corpus for which assets will be
      transformed.
    language_code: the language code of the videos that will be analyzed, such
      as 'en-US'. Required.
    speech_data_schema_key: the key used for speech transcription annotations.
      By default, it is "speech".
    speech_transcript_search_criteria_key: the criteria key used to search for
      speech transcripts. By default, it is "speech", so query format is
      "speech:query".
    audio_tracks: for file formats, such as MXF or MKV, supporting multiple
      audio tracks, specify up to two tracks. Default: use track 0.
  """

  corpus_name: str
  language_code: str
  speech_data_schema_key: str = _SPEECH_ANNOTATIONS_DATA_SCHEMA_KEY
  speech_transcript_search_criteria_key: str = (
      _SPEECH_TRANSCRIPT_SEARCH_CRITERIA_KEY
  )
  audio_tracks: Sequence[int] = ()


class SpeechTransformer(TransformerInterface):
  """Runs speech transcription for the given asset."""

  def __init__(
      self,
      init_config: SpeechTransformerInitConfig,
      warehouse_client: visionai_v1.WarehouseClient,
      video_intelligence_client: Optional[
          videointelligence_v1.VideoIntelligenceServiceClient
      ] = None,
  ):
    """Constructor.

    Args:
      init_config: the config used for initialized the transformer.
      warehouse_client: the client to talk to warehouse.
      video_intelligence_client: the client to talk to video intelligence API.
    """
    super().__init__(warehouse_client)

    self.video_intelligence_client = (
        video_intelligence_client
        if video_intelligence_client is not None
        else videointelligence_v1.VideoIntelligenceServiceClient()
    )
    self._init_config = init_config

  def initialize(self) -> None:
    """Initialize the transformer."""
    _logger.info("Initialize SpeechTransformer.")
    # Sets up data schema.
    request = visionai_v1.CreateDataSchemaRequest(
        parent=self._init_config.corpus_name,
        data_schema=visionai_v1.DataSchema(
            key=self._init_config.speech_data_schema_key,
            schema_details=_construct_speech_data_schema(),
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
                          "{annotation_key}.{transcript_field}".format(
                              annotation_key=self._init_config.speech_data_schema_key,
                              transcript_field=_TRANSCRIPT,
                          )
                      }
                  )
              ),
              search_config_id=self._init_config.speech_transcript_search_criteria_key,
              parent=self._init_config.corpus_name,
          )
      )
    except exceptions.AlreadyExists:
      logging.warning(
          "Search config %s already exists.", request.data_schema.key
      )
    else:
      _logger.info("Search config created %s", search_config)

  def transform(self, asset_name: str) -> TransformProgress:
    """Performs transform for the given asset resource.

    Args:
      asset_name: the asset name that this transform will operate on.

    Returns:
      LroTransformProgress used to poll status for the transformation.
    """
    get_asset_request = visionai_v1.GetAssetRequest(name=asset_name)
    get_asset_response = self.warehouse_client.get_asset(get_asset_request)
    # TODO(zhangxiaotian): Create annotation for language hints.
    request = videointelligence_v1.AnnotateVideoRequest(
        input_uri=get_asset_response.asset_gcs_source.gcs_uri,
        features=[videointelligence_v1.Feature.SPEECH_TRANSCRIPTION],
    )
    request.video_context.speech_transcription_config = (
        videointelligence_v1.SpeechTranscriptionConfig(
            language_code=self._init_config.language_code,
            audio_tracks=self._init_config.audio_tracks,
            enable_automatic_punctuation=True,
        )
    )
    annotate_video_operation = self.video_intelligence_client.annotate_video(
        request=request
    )
    _logger.info(
        "Speech transcription lro %s", annotate_video_operation.operation
    )
    annotate_video_operation_with_polling_config = operation.from_gapic(
        annotate_video_operation.operation,
        self.video_intelligence_client.transport.operations_client,
        videointelligence_v1.AnnotateVideoResponse,
        metadata_type=videointelligence_v1.AnnotateVideoProgress,
        retry=transform_progress.DEFAULT_POLLING,
    )
    speech_transform_progress = WaitAndWriteWarehouseTransformProgress(
        asset_name,
        self.warehouse_client,
        annotate_video_operation_with_polling_config,
        functools.partial(
            _construct_annotations, self._init_config.speech_data_schema_key
        ),
    )

    return speech_transform_progress
