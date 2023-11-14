# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for OcrTransform."""

import logging
from unittest import mock
from google.api_core import operation
from google.auth import credentials as ga_credentials
from google.cloud import videointelligence_v1
from google.longrunning import operations_pb2
from google.protobuf import timestamp_pb2
from google.protobuf import text_format
from visionai.python.testing import googletest
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.lva import client
from visionai.python.net import channel
from visionai.python.warehouse.transformer import ocr_transformer

_ASSET_NAME = "projects/1/locations/us-central1/corpora/2/assets/3"
_CORPUS_NAME = "projects/1/locations/us-central1/corpora/2"
_DATA_SCHEMA_KEY = "ocr_annotation_key"
_SEARCH_CRITERIA_KEY = "ocr_search_criteria_key"
_TEST_OPERATION_NAME = "test/operation"
_GCS_URI = "gs://file.mp4"
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


class OcrTransformerTest(googletest.TestCase):

  def _construct_process(self, state: visionai_v1.RunStatus.State):
    return visionai_v1.Process(
        name="projects/test-project/locations/us-central1/clusters/test-cluster/processes/test-process",
        create_time=timestamp_pb2.Timestamp(seconds=1),
        analysis="projects/test-project/locations/us-central1/clusters/test-cluster/analyses/test-analysis",
        attribute_overrides=["k1=v1", "k2=v2"],
        run_status=visionai_v1.RunStatus(
            state=state,
        ),
        run_mode=visionai_v1.RunMode.SUBMISSION,
    )

  def setUp(self):
    super().setUp()
    self._lva_client = visionai_v1.LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="grpc",
    )
    self._connection_options = channel.ConnectionOptions(
        project_id="test-project",
        location_id="us-central1",
        cluster_id="test-cluster",
    )
    self._parent = "projects/{}/locations/{}/clusters/{}".format(
        self._connection_options.project_id,
        self._connection_options.location_id,
        self._connection_options.cluster_id,
    )
    self._mock_create_lva_client = self.enter_context(
        mock.patch.object(client, "_create_lva_client", autospec=True)
    )
    self._mock_create_lva_client.return_value = self._lva_client
    self._mock_get_or_create_analysis = self.enter_context(
        mock.patch.object(client, "get_or_create_analysis", autospec=True)
    )
    self._mock_create_process = self.enter_context(
        mock.patch.object(client, "create_process", autospec=True)
    )
    self._mock_get_process = self.enter_context(
        mock.patch.object(client, "get_process", autospec=True)
    )
    self._mock_list_processes = self.enter_context(
        mock.patch.object(client, "list_processes", autospec=True)
    )
    self._mock_delete_analysis = self.enter_context(
        mock.patch.object(client, "delete_analysis", autospec=True)
    )
    self._mock_get_process.side_effect = [
        client.Process(
            self._construct_process(visionai_v1.RunStatus.State.RUNNING)
        ),
        client.Process(
            self._construct_process(visionai_v1.RunStatus.State.COMPLETED)
        ),
    ]

  def test_transform_using_lva(self):
    init_config = ocr_transformer.OcrTransformerInitConfig(
        corpus_name=_CORPUS_NAME,
        ocr_data_schema_key=_DATA_SCHEMA_KEY,
        ocr_search_criteria_key=_SEARCH_CRITERIA_KEY,
        use_video_intelligence=False,
    )
    transformer = ocr_transformer.OcrTransformer(
        init_config=init_config,
        warehouse_client=mock.MagicMock(),
    )
    transformer.initialize()
    transformer.warehouse_client.create_search_config.assert_called_once_with(
        visionai_v1.CreateSearchConfigRequest(
            text_format.Parse(
                f"""
                  parent: "{_CORPUS_NAME}"
                  search_config {{
                    search_criteria_property {{
                        mapped_fields: "{_DATA_SCHEMA_KEY}.text"
                    }}
                  }}
                  search_config_id:"{_SEARCH_CRITERIA_KEY}"
                """,
                visionai_v1.types.CreateSearchConfigRequest.pb()(),
            )
        )
    )
    transformer.warehouse_client.parse_corpus_path.return_value = {
        "project_number": 1,
        "location": "us-central1",
    }
    transform_progress = transformer.transform(_ASSET_NAME)
    transformer.warehouse_client.create_data_schema.assert_called_once_with(
        visionai_v1.CreateDataSchemaRequest(
            text_format.Parse(
                f"""parent: "{_CORPUS_NAME}"
                    data_schema {{
                      key: "{_DATA_SCHEMA_KEY}"
                      schema_details {{
                        type_: CUSTOMIZED_STRUCT
                        granularity: GRANULARITY_PARTITION_LEVEL
                        customized_struct_config {{
                          field_schemas {{
                            key: "confidence"
                            value {{
                              type_: FLOAT
                              granularity: GRANULARITY_PARTITION_LEVEL
                            }}
                          }}
                          field_schemas {{
                            key: "frame-info"
                            value {{
                              type_: LIST
                              granularity: GRANULARITY_PARTITION_LEVEL
                              list_config {{
                                value_schema {{
                                  type_: CUSTOMIZED_STRUCT
                                  granularity: GRANULARITY_PARTITION_LEVEL
                                  customized_struct_config {{
                                    field_schemas {{
                                      key: "bounding-box"
                                      value {{
                                        type_: CUSTOMIZED_STRUCT
                                        granularity: GRANULARITY_PARTITION_LEVEL
                                        customized_struct_config {{
                                          field_schemas {{
                                            key: "x-max"
                                            value {{
                                              type_: FLOAT
                                              granularity: GRANULARITY_PARTITION_LEVEL
                                            }}
                                          }}
                                          field_schemas {{
                                            key: "x-min"
                                            value {{
                                              type_: FLOAT
                                              granularity: GRANULARITY_PARTITION_LEVEL
                                            }}
                                          }}
                                          field_schemas {{
                                            key: "y-max"
                                            value {{
                                              type_: FLOAT
                                              granularity: GRANULARITY_PARTITION_LEVEL
                                            }}
                                          }}
                                          field_schemas {{
                                            key: "y-min"
                                            value {{
                                              type_: FLOAT
                                              granularity: GRANULARITY_PARTITION_LEVEL
                                            }}
                                          }}
                                        }}
                                      }}
                                    }}
                                    field_schemas {{
                                      key: "timestamp-microseconds"
                                      value {{
                                        type_: INTEGER
                                        granularity: GRANULARITY_PARTITION_LEVEL
                                      }}
                                    }}
                                  }}
                                }}
                              }}
                            }}
                          }}
                          field_schemas {{
                            key: "text"
                            value {{
                              type_: STRING
                              granularity: GRANULARITY_PARTITION_LEVEL
                              search_strategy {{
                                search_strategy_type: SMART_SEARCH
                              }}
                            }}
                          }}
                        }}
                      }}
                    }}
                """, visionai_v1.types.CreateDataSchemaRequest.pb()())))
    self.assertEqual(
        transform_progress.result(),
        client.Process(
            self._construct_process(visionai_v1.RunStatus.State.COMPLETED)
        ),
    )

  def test_transform_using_video_intelligence(self):
    init_config = ocr_transformer.OcrTransformerInitConfig(
        corpus_name=_CORPUS_NAME,
        ocr_data_schema_key=_DATA_SCHEMA_KEY,
        ocr_search_criteria_key=_SEARCH_CRITERIA_KEY,
    )
    transformer = ocr_transformer.OcrTransformer(
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
                        mapped_fields: "{_DATA_SCHEMA_KEY}.text"
                    }}
                  }}
                  search_config_id:"{_SEARCH_CRITERIA_KEY}"
                """,
                visionai_v1.types.CreateSearchConfigRequest.pb()(),
            )
        )
    )
    transformer.warehouse_client.parse_corpus_path.return_value = {
        "project_number": 1,
        "location": "us-central1",
    }
    transformer.warehouse_client.get_asset.return_value = visionai_v1.Asset(
        asset_gcs_source=visionai_v1.AssetSource.AssetGcsSource(
            gcs_uri=_GCS_URI
        )
    )
    response = text_format.Parse(
        """
          annotation_results {
            text_annotations {
              text: "google"
              segments {
                segment {
                  start_time_offset {
                    seconds: 79
                    nanos: 329250000
                  }
                  end_time_offset {
                    seconds: 79
                    nanos: 329250000
                  }
                }
                confidence: 0.8157249689102173
                frames {
                  rotated_bounding_box {
                    vertices {
                      x: 0.3578124940395355
                      y: 0.4763889014720917
                    }
                    vertices {
                      x: 0.47126349806785583
                      y: 0.5696819424629211
                    }
                    vertices {
                      x: 0.46470382809638977
                      y: 0.5948932766914368
                    }
                    vertices {
                      x: 0.35125282406806946
                      y: 0.5016002058982849
                    }
                  }
                  time_offset {
                    seconds: 79
                    nanos: 329250000
                  }
                }
              }
            }
          }
          annotation_results {
            text_annotations {
              text: "cloud"
              segments {
                segment {
                  start_time_offset {
                    seconds: 92
                    nanos: 842750000
                  }
                  end_time_offset {
                    seconds: 97
                    nanos: 972875000
                  }
                }
                confidence: 0.9931923151016235
                frames {
                  rotated_bounding_box {
                    vertices {
                      x: 0.24140624701976776
                      y: 0.5222222208976746
                    }
                    vertices {
                      x: 0.75390625
                      y: 0.5222222208976746
                    }
                    vertices {
                      x: 0.75390625
                      y: 0.6013888716697693
                    }
                    vertices {
                      x: 0.24140624701976776
                      y: 0.6013888716697693
                    }
                  }
                  time_offset {
                    seconds: 92
                    nanos: 842750000
                  }
                }
                frames {
                  rotated_bounding_box {
                    vertices {
                      x: 0.23828125
                      y: 0.5041666626930237
                    }
                    vertices {
                      x: 0.7578125
                      y: 0.5041666626930237
                    }
                    vertices {
                      x: 0.7578125
                      y: 0.6194444298744202
                    }
                    vertices {
                      x: 0.23828125
                      y: 0.6194444298744202
                    }
                  }
                  time_offset {
                    seconds: 92
                    nanos: 967875000
                  }
                }
                frames {
                  rotated_bounding_box {
                    vertices {
                      x: 0.23749999701976776
                      y: 0.5097222328186035
                    }
                    vertices {
                      x: 0.7554687261581421
                      y: 0.5097222328186035
                    }
                    vertices {
                      x: 0.7554687261581421
                      y: 0.6180555820465088
                    }
                    vertices {
                      x: 0.23749999701976776
                      y: 0.6180555820465088
                    }
                  }
                  time_offset {
                    seconds: 93
                    nanos: 93000000
                  }
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
    transformer.warehouse_client.create_data_schema.assert_called_once_with(
        visionai_v1.CreateDataSchemaRequest(
            text_format.Parse(
                f"""parent: "{_CORPUS_NAME}"
                    data_schema {{
                      key: "{_DATA_SCHEMA_KEY}"
                      schema_details {{
                        type_: CUSTOMIZED_STRUCT
                        granularity: GRANULARITY_PARTITION_LEVEL
                        customized_struct_config {{
                          field_schemas {{
                            key: "confidence"
                            value {{
                              type_: FLOAT
                              granularity: GRANULARITY_PARTITION_LEVEL
                            }}
                          }}
                          field_schemas {{
                            key: "frame-info"
                            value {{
                              type_: LIST
                              granularity: GRANULARITY_PARTITION_LEVEL
                              list_config {{
                                value_schema {{
                                  type_: CUSTOMIZED_STRUCT
                                  granularity: GRANULARITY_PARTITION_LEVEL
                                  customized_struct_config {{
                                    field_schemas {{
                                      key: "normalized-bounding-poly"
                                      value {{
                                        type_: LIST
                                        granularity: GRANULARITY_PARTITION_LEVEL
                                        list_config {{
                                          value_schema {{
                                            type_: CUSTOMIZED_STRUCT
                                            granularity: GRANULARITY_PARTITION_LEVEL
                                            customized_struct_config {{
                                              field_schemas {{
                                                key: "x"
                                                value {{
                                                  type_: FLOAT
                                                  granularity: GRANULARITY_PARTITION_LEVEL
                                                }}
                                              }}
                                              field_schemas {{
                                                key: "y"
                                                value {{
                                                  type_: FLOAT
                                                  granularity: GRANULARITY_PARTITION_LEVEL
                                                }}
                                              }}
                                            }}
                                          }}
                                        }}
                                      }}
                                    }}
                                    field_schemas {{
                                      key: "timestamp-microseconds"
                                      value {{
                                        type_: INTEGER
                                        granularity: GRANULARITY_PARTITION_LEVEL
                                      }}
                                    }}
                                  }}
                                }}
                              }}
                            }}
                          }}
                          field_schemas {{
                            key: "text"
                            value {{
                              type_: STRING
                              granularity: GRANULARITY_PARTITION_LEVEL
                              search_strategy {{
                                search_strategy_type: SMART_SEARCH
                              }}
                            }}
                          }}
                        }}
                      }}
                    }}
                              """,
                visionai_v1.types.CreateDataSchemaRequest.pb()(),
            )
        )
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
                                float_value: 0.8157249689102173
                              }}
                            }}
                            elements {{
                              key: "frame-info"
                              value {{
                                list_value {{
                                  values {{
                                    customized_struct_value {{
                                      elements {{
                                        key: "normalized-bounding-poly"
                                        value {{
                                          list_value {{
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.3578124940395355
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.4763889014720917
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.47126349806785583
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5696819424629211
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.46470382809638977
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5948932766914368
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.35125282406806946
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5016002058982849
                                                  }}
                                                }}
                                              }}
                                            }}
                                          }}
                                        }}
                                      }}
                                      elements {{
                                        key: "timestamp-microseconds"
                                        value {{
                                          int_value: 79329250
                                        }}
                                      }}
                                    }}
                                  }}
                                }}
                              }}
                            }}
                            elements {{
                              key: "text"
                              value {{
                                str_value: "google"
                              }}
                            }}
                          }}
                        }}
                        partition {{
                          relative_temporal_partition {{
                            start_offset {{
                              seconds: 79
                              nanos: 329250000
                            }}
                            end_offset {{
                              seconds: 79
                              nanos: 329250000
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
                                float_value: 0.9931923151016235
                              }}
                            }}
                            elements {{
                              key: "frame-info"
                              value {{
                                list_value {{
                                  values {{
                                    customized_struct_value {{
                                      elements {{
                                        key: "normalized-bounding-poly"
                                        value {{
                                          list_value {{
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.24140624701976776
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5222222208976746
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.75390625
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5222222208976746
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.75390625
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.6013888716697693
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.24140624701976776
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.6013888716697693
                                                  }}
                                                }}
                                              }}
                                            }}
                                          }}
                                        }}
                                      }}
                                      elements {{
                                        key: "timestamp-microseconds"
                                        value {{
                                          int_value: 92842750
                                        }}
                                      }}
                                    }}
                                  }}
                                  values {{
                                    customized_struct_value {{
                                      elements {{
                                        key: "normalized-bounding-poly"
                                        value {{
                                          list_value {{
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.23828125
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5041666626930237
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.7578125
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5041666626930237
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.7578125
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.6194444298744202
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.23828125
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.6194444298744202
                                                  }}
                                                }}
                                              }}
                                            }}
                                          }}
                                        }}
                                      }}
                                      elements {{
                                        key: "timestamp-microseconds"
                                        value {{
                                          int_value: 92967875
                                        }}
                                      }}
                                    }}
                                  }}
                                  values {{
                                    customized_struct_value {{
                                      elements {{
                                        key: "normalized-bounding-poly"
                                        value {{
                                          list_value {{
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.23749999701976776
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5097222328186035
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.7554687261581421
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.5097222328186035
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.7554687261581421
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.6180555820465088
                                                  }}
                                                }}
                                              }}
                                            }}
                                            values {{
                                              customized_struct_value {{
                                                elements {{
                                                  key: "x"
                                                  value {{
                                                    float_value: 0.23749999701976776
                                                  }}
                                                }}
                                                elements {{
                                                  key: "y"
                                                  value {{
                                                    float_value: 0.6180555820465088
                                                  }}
                                                }}
                                              }}
                                            }}
                                          }}
                                        }}
                                      }}
                                      elements {{
                                        key: "timestamp-microseconds"
                                        value {{
                                          int_value: 93093000
                                        }}
                                      }}
                                    }}
                                  }}
                                }}
                              }}
                            }}
                            elements {{
                              key: "text"
                              value {{
                                str_value: "cloud"
                              }}
                            }}
                          }}
                        }}
                        partition {{
                          relative_temporal_partition {{
                            start_offset {{
                              seconds: 92
                              nanos: 842750000
                            }}
                            end_offset {{
                              seconds: 97
                              nanos: 972875000
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

  def test_teardown_success(self):
    self._mock_list_processes.side_effect = [[]]
    init_config = ocr_transformer.OcrTransformerInitConfig(
        corpus_name=_CORPUS_NAME,
        ocr_data_schema_key=_DATA_SCHEMA_KEY,
        ocr_search_criteria_key=_SEARCH_CRITERIA_KEY,
        use_video_intelligence=False,
    )
    transformer = ocr_transformer.OcrTransformer(
        init_config=init_config,
        warehouse_client=mock.MagicMock(),
    )
    transformer.initialize()
    self.assertTrue(transformer.teardown())
    self._mock_list_processes.assert_called_once()
    self._mock_delete_analysis.assert_called_once()

  def test_teardown_fail(self):
    self._mock_list_processes.side_effect = [
        [
            client.Process(
                self._construct_process(visionai_v1.RunStatus.State.RUNNING)
            )
        ]
    ]
    init_config = ocr_transformer.OcrTransformerInitConfig(
        corpus_name=_CORPUS_NAME,
        ocr_data_schema_key=_DATA_SCHEMA_KEY,
        ocr_search_criteria_key=_SEARCH_CRITERIA_KEY,
        use_video_intelligence=False,
    )
    transformer = ocr_transformer.OcrTransformer(
        init_config=init_config,
        warehouse_client=mock.MagicMock(),
    )
    transformer.initialize()
    self.assertFalse(transformer.teardown())
    self._mock_list_processes.assert_called_once()


if __name__ == "__main__":
  googletest.main()
