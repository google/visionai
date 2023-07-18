# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for SpeechTransformer."""

import logging
from unittest import mock
from google.api_core import operation
from google.cloud import videointelligence_v1
from google.longrunning import operations_pb2
from google.protobuf import text_format
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer import speech_transformer

_ASSET_NAME = "projects/1/copora/2/assets/3"
_CORPUS_NAME = "projects/1/copora/2"
_GCS_URI = "gs://file.mp4"
_TEST_OPERATION_NAME = "test/operation"
_DATA_SCHEMA_KEY = "speech_anno_key"
_SEARCH_CRITERIA_KEY = "speech_transcript"
_logger = logging.getLogger(__name__)


def make_operation_proto(
    name=_TEST_OPERATION_NAME,
    metadata=None,
    response=None,
    error=None,
    **kwargs,
):
  operation_proto = operations_pb2.Operation(name=name, **kwargs)

  if metadata is not None:
    operation_proto.metadata.Pack(metadata)
  if response is not None:
    operation_proto.response.Pack(response)
  if error is not None:
    operation_proto.error.CopyFrom(error)

  return operation_proto


def make_operation_future(client_operations_responses=None):
  if client_operations_responses is None:
    client_operations_responses = [make_operation_proto()]

  refresh = mock.Mock(
      spec=["__call__"], side_effect=client_operations_responses
  )
  refresh.responses = client_operations_responses
  cancel = mock.Mock(spec=["__call__"])
  operation_future = operation.Operation(
      client_operations_responses[0],
      refresh,
      cancel,
      result_type=videointelligence_v1.AnnotateVideoResponse,
  )

  return operation_future, refresh, cancel


class SpeechTransformerTest(googletest.TestCase):

  def test_transform(self):
    init_config = speech_transformer.SpeechTransformerInitConfig(
        language_code="en-US",
        audio_tracks=[],
        corpus_name=_CORPUS_NAME,
        speech_data_schema_key=_DATA_SCHEMA_KEY,
        speech_transcript_search_criteria_key=_SEARCH_CRITERIA_KEY,
    )
    transformer = speech_transformer.SpeechTransformer(
        init_config=init_config,
        warehouse_client=mock.MagicMock(),
        video_intelligence_client=mock.MagicMock(),
    )
    transformer.initialize()
    transformer.warehouse_client.create_search_config.assert_called_once_with(
        visionai_v1.CreateSearchConfigRequest(
            text_format.Parse(
                f"""
                  parent: "{_CORPUS_NAME}"
                  search_config {{
                    search_criteria_property {{
                        mapped_fields: "{_DATA_SCHEMA_KEY}.transcript"
                    }}
                  }}
                  search_config_id:"{_SEARCH_CRITERIA_KEY}"
                """,
                visionai_v1.types.CreateSearchConfigRequest.pb()(),
            )
        )
    )
    transformer.warehouse_client.get_asset.return_value = visionai_v1.Asset(
        asset_gcs_source=visionai_v1.AssetSource.AssetGcsSource(
            gcs_uri=_GCS_URI
        )
    )
    response = text_format.Parse(
        """
          annotation_results {
            speech_transcriptions {
              alternatives {
                transcript: "transcript 1.1.1"
                confidence: 1.0
                words {
                  start_time {
                    seconds: 1
                  }
                  end_time {
                    seconds: 11
                  }
                  word: "transcript"
                }
                words {
                  start_time {
                    seconds: 21
                  }
                  end_time {
                    seconds: 31
                  }
                  word: "1.1.1"
                }
              }
              alternatives {
                transcript: "transcript2"
                confidence: 0.6
                words {
                  start_time {
                    seconds: 1
                  }
                  end_time {
                    seconds: 31
                  }
                  word: "transcript2"
                }
              }
            }
            speech_transcriptions {
              alternatives {
                transcript: "transcript3"
                confidence: 0.7
                words {
                  start_time {
                    seconds: 61
                  }
                  end_time {
                    seconds: 81
                  }
                  word: "transcript3"
                }
              }
            }
          }
          annotation_results {
            speech_transcriptions {
              alternatives {
                transcript: "transcript4"
                confidence: 0.8
                words {
                  start_time {
                    seconds: 91
                  }
                  end_time {
                    seconds: 99
                  }
                  word: "transcript4"
                }
              }
            }
          }
          """,
        videointelligence_v1.types.AnnotateVideoResponse.pb()(),
    )

    responses = [
        make_operation_proto(),
        # Second operation response includes the result.
        make_operation_proto(done=True, response=response),
    ]

    future, _, _ = make_operation_future(responses)
    transformer.video_intelligence_client.annotate_video.return_value = future
    mock_from_gapic = self.enter_context(
        mock.patch.object(operation, "from_gapic", autospec=True)
    )
    mock_from_gapic.return_value = future

    transform_progress = transformer.transform(_ASSET_NAME)
    transformer.warehouse_client.create_data_schema.assert_called()
    transformer.warehouse_client.get_asset.assert_called()
    expected_request = videointelligence_v1.AnnotateVideoRequest(
        input_uri=_GCS_URI,
        features=[videointelligence_v1.Feature.SPEECH_TRANSCRIPTION],
    )
    expected_request.video_context.speech_transcription_config.language_code = (
        "en-US"
    )
    expected_request.video_context.speech_transcription_config.audio_tracks = []
    expected_request.video_context.speech_transcription_config.enable_automatic_punctuation = (
        True
    )
    transformer.video_intelligence_client.assert_has_calls(
        [mock.call.annotate_video(request=expected_request)]
    )

    self.assertTrue(transform_progress.result())
    transformer.warehouse_client.create_annotation.assert_any_call(
        visionai_v1.CreateAnnotationRequest(
            text_format.Parse(
                f"""
                    parent: "{_ASSET_NAME}"
                    annotation {{
                      user_specified_annotation {{
                        key: "{_DATA_SCHEMA_KEY}"
                        value {{
                          customized_struct_value {{
                            elements {{
                              key: "confidence"
                              value {{
                                float_value: 1.0
                              }}
                            }}
                            elements {{
                              key: "transcript"
                              value {{
                                str_value: "transcript 1.1.1"
                              }}
                            }}
                          }}
                        }}
                        partition {{
                          relative_temporal_partition {{
                            start_offset {{
                              seconds: 1
                            }}
                            end_offset {{
                              seconds: 31
                            }}
                          }}
                        }}
                      }}
                    }}""",
                visionai_v1.types.CreateAnnotationRequest.pb()(),
            )
        ),
        retry=mock.ANY,
    )
    transformer.warehouse_client.create_annotation.assert_any_call(
        visionai_v1.CreateAnnotationRequest(
            text_format.Parse(
                f"""
                  parent: "{_ASSET_NAME}"
                  annotation {{
                    user_specified_annotation {{
                      key: "{_DATA_SCHEMA_KEY}"
                      value {{
                        customized_struct_value {{
                          elements {{
                            key: "confidence"
                            value {{
                              float_value: 0.6
                            }}
                          }}
                          elements {{
                            key: "transcript"
                            value {{
                              str_value: "transcript2"
                            }}
                          }}
                        }}
                      }}
                      partition {{
                        relative_temporal_partition {{
                          start_offset {{
                            seconds: 1
                          }}
                          end_offset {{
                            seconds: 31
                          }}
                        }}
                      }}
                    }}
                  }}""",
                visionai_v1.types.CreateAnnotationRequest.pb()(),
            )
        ),
        retry=mock.ANY,
    )
    transformer.warehouse_client.create_annotation.assert_any_call(
        visionai_v1.CreateAnnotationRequest(
            text_format.Parse(
                f"""
                  parent: "{_ASSET_NAME}"
                  annotation {{
                    user_specified_annotation {{
                      key: "{_DATA_SCHEMA_KEY}"
                      value {{
                        customized_struct_value {{
                          elements {{
                            key: "confidence"
                            value {{
                              float_value: 0.7
                            }}
                          }}
                          elements {{
                            key: "transcript"
                            value {{
                              str_value: "transcript3"
                            }}
                          }}
                        }}
                      }}
                      partition {{
                        relative_temporal_partition {{
                          start_offset {{
                            seconds: 61
                          }}
                          end_offset {{
                            seconds: 81
                          }}
                        }}
                      }}
                    }}
                  }}""",
                visionai_v1.types.CreateAnnotationRequest.pb()(),
            )
        ),
        retry=mock.ANY,
    )
    transformer.warehouse_client.create_annotation.assert_any_call(
        visionai_v1.CreateAnnotationRequest(
            text_format.Parse(
                f"""
                  parent: "{_ASSET_NAME}"
                  annotation {{
                    user_specified_annotation {{
                      key: "{_DATA_SCHEMA_KEY}"
                      value {{
                        customized_struct_value {{
                          elements {{
                            key: "confidence"
                            value {{
                              float_value: 0.8
                            }}
                          }}
                          elements {{
                            key: "transcript"
                            value {{
                              str_value: "transcript4"
                            }}
                          }}
                        }}
                      }}
                      partition {{
                        relative_temporal_partition {{
                          start_offset {{
                            seconds: 91
                          }}
                          end_offset {{
                            seconds: 99
                          }}
                        }}
                      }}
                    }}
                  }}""",
                visionai_v1.types.CreateAnnotationRequest.pb()(),
            )
        ),
        retry=mock.ANY,
    )


if __name__ == "__main__":
  googletest.main()
