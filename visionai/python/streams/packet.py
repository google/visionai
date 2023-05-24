# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Packet library."""

from visionai.python.streams import _pywrap_packet


def make_packet(s):
  """Creates a Packet.

  Args:
    s: The payload of the Packet.

  Returns:
    A `google::cloud::visionai::v1::Packet`.

  Raises:
    TypeError: If the given payload type is not supported.
    pybind11_abseil.status.StatusNotOk: If the packet creation was not
    successful.
  """
  return _pywrap_packet.make_packet(s)
