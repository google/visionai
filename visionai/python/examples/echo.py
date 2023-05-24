# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Example code to do prediction."""

from typing import Sequence

from absl import app

from visionai.python.prediction import client


def main(argv: Sequence[str]) -> None:
  if len(argv) != 2:
    raise app.UsageError("Usage: echo STRING")
  print(client.echo(argv[1]))


if __name__ == "__main__":
  app.run(main)
