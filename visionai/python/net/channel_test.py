# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Tests for the channel library."""

from visionai.python.testing import googletest
from visionai.python.net import channel


class ChannelTest(googletest.TestCase):
  def test_get_service_endpoint(self):
    self.assertEqual(
        channel.get_service_endpoint(channel.Environment.AUTOPUSH),
        'autopush-visionai.sandbox.googleapis.com',
    )
    self.assertEqual(
        channel.get_service_endpoint(channel.Environment.STAGING),
        'staging-visionai.sandbox.googleapis.com',
    )
    self.assertEqual(
        channel.get_service_endpoint(channel.Environment.PROD),
        'visionai.googleapis.com',
    )

  def test_get_warehouse_service_endpoint(self):
    self.assertEqual(
        channel.get_warehouse_service_endpoint(channel.Environment.AUTOPUSH),
        'autopush-warehouse-visionai.sandbox.googleapis.com',
    )
    self.assertEqual(
        channel.get_warehouse_service_endpoint(channel.Environment.STAGING),
        'staging-warehouse-visionai.sandbox.googleapis.com',
    )
    self.assertEqual(
        channel.get_warehouse_service_endpoint(channel.Environment.PROD),
        'warehouse-visionai.googleapis.com',
    )


if __name__ == '__main__':
  googletest.main()
