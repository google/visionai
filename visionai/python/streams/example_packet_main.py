# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Example packet binary."""

from typing import Sequence
from absl import app
from absl import flags

from visionai.python.streams import packet

_NAME = flags.DEFINE_string(
    name="name",
    default="World",
    help="The packet will greet you with your name.",
)


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")
  p = packet.make_packet("Hello, {}!".format(_NAME.value))
  print("{}".format(p))


if __name__ == "__main__":
  app.run(main)
