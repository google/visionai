# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Tests for OcrTransform."""

import logging
from unittest import mock
from google.auth import credentials as ga_credentials
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
_logger = logging.getLogger(__name__)


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

  def test_transform(self):
    init_config = ocr_transformer.OcrTransformerInitConfig(
        corpus_name=_CORPUS_NAME,
        ocr_data_schema_key=_DATA_SCHEMA_KEY,
        ocr_search_criteria_key=_SEARCH_CRITERIA_KEY,
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
    transformer.warehouse_client.create_data_schema.assert_called()
    self.assertEqual(
        transform_progress.result(),
        client.Process(
            self._construct_process(visionai_v1.RunStatus.State.COMPLETED)
        ),
    )

  def test_teardown_success(self):
    self._mock_list_processes.side_effect = [[]]
    init_config = ocr_transformer.OcrTransformerInitConfig(
        corpus_name=_CORPUS_NAME,
        ocr_data_schema_key=_DATA_SCHEMA_KEY,
        ocr_search_criteria_key=_SEARCH_CRITERIA_KEY,
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
