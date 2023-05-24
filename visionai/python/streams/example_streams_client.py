# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Example client binary to list streams in Vertex AI Vision."""

from typing import Sequence
from absl import app
from absl import flags

from visionai.python.streams import client

_SERVICE_ENDPOINT = flags.DEFINE_string(
    name="service_endpoint",
    default="visionai.googleapis.com",
    help="The endpoint to Vertex AI Vsion.",
)

_PROJECT_ID = flags.DEFINE_string(
    name="project_id",
    default="visionai-testing",
    help="The consumer project id.",
)

_LOCATION_ID = flags.DEFINE_string(
    name="location_id",
    default="us-central1",
    help="The location id.",
)

_CLUSTER_ID = flags.DEFINE_string(
    name="cluster_id",
    default="application-cluster-1",
    help="The cluster id.",
)


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")
  connection_options = client.ConnectionOptions(
      _SERVICE_ENDPOINT.value,
      _PROJECT_ID.value,
      _LOCATION_ID.value,
      _CLUSTER_ID.value,
  )
  response = client.list_streams(connection_options)
  print("{}", response)


if __name__ == "__main__":
  app.run(main)
