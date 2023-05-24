# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Tests for the offline library."""
from unittest import mock

from visionai.python.testing import googletest
from visionai.python.lva import client
from visionai.python.net import channel
from visionai.python.prediction import offline


class OfflineTest(googletest.TestCase):

  def setUp(self):
    super().setUp()
    self._mock_get_or_create_analysis = self.enter_context(
        mock.patch.object(client, "get_or_create_analysis", autospec=True)
    )
    self._mock_create_process = self.enter_context(
        mock.patch.object(client, "create_process", autospec=True)
    )

  def test_blur_gcs_video(self):
    connection_options = channel.ConnectionOptions("p1", "l1", "c1")
    p = offline.blur_gcs_video_sync(connection_options, "gs://foo", "gs://bar")


if __name__ == "__main__":
  googletest.main()
