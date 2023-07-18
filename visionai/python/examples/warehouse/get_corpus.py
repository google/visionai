# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
r"""An simple example to get a warehouse corpus.

It demonstrates using gapic auto generated client libraries that are copied into
the google3 folder with modifications specified in copy.bara.sky.

blaze run //third_party/visionai/python/examples/warehouse:get_corpus -- \
    --project_number=<project-number> --location_id=<location-id> \
    --corpus_id=<corpus-id>

"""

from absl import app
from absl import flags
from visionai.python.gapic.visionai import visionai_v1

_PROJECT_NUMBER = flags.DEFINE_string("project_number", None, "project_number")
_LOCATION_ID = flags.DEFINE_string("location_id", None, "region")
_CORPUS_ID = flags.DEFINE_string("corpus_id", None, "corpus_id")


def main(unused_argv) -> None:
  # Create a client
  client = visionai_v1.WarehouseClient(
      client_options={"api_endpoint": "warehouse-visionai.googleapis.com"},
  )

  # Initialize request argument(s)
  request = visionai_v1.GetCorpusRequest(
      name=client.corpus_path(
          _PROJECT_NUMBER.value, _LOCATION_ID.value, _CORPUS_ID.value
      ),
  )

  # Make the request
  response = client.get_corpus(request=request)

  # Handle the response
  print(response)


if __name__ == "__main__":
  app.run(main)