# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Python SDK exmaple to create processes to blur mp4 videos in a GCS bucket.

It demonstrates:
1. List mp4 files under a GCS bucket
2. Create process to blur each mp4 video and wait for it to finish.

Steps to run the program:
1. pip install google-cloud-storage
2. python3 blur_gcs_video.py --project=<project-id> --location_id=<location-id>
--cluster_id=<cluster-id> --bucket_name=<bucket-name> --frame_rate=<fps-value>

Example:
Given a GCS bucket:
test-bucket
  test1.mp4
  test2.mp4

After running the example program, we will get

test-bucket
  test1.mp4
  test2.mp4
  test1_deid_output.mp4
  test2_deid_output.mp4
"""
import asyncio

from absl import app
from absl import flags
from google.cloud import storage

from visionai.python.lva import client
from visionai.python.net import channel
from visionai.python.prediction import offline


_PROJECT_ID = flags.DEFINE_string("project_id", None, "project_id")
_LOCATION_ID = flags.DEFINE_string("location_id", None, "region")
_CLUSTER_ID = flags.DEFINE_string(
    "cluster_id", None, "the cluster the process will run on"
)
_BUCKET_NAME = flags.DEFINE_string(
    "bucket_name", None, "GCS bucket which stores example mp4 videos"
)
_ENV = flags.DEFINE_enum(
    "env",
    "PROD",
    ["AUTOPUSH", "STAGING", "PROD"],
    "The environment.",
)
_FPS = flags.DEFINE_integer("frame_rate", 6, "Video frame rate")


def _validate_args():
  """Validate input arguments."""
  if _PROJECT_ID.value is None:
    raise ValueError("The project_id isn't provided")
  if _LOCATION_ID.value is None:
    raise ValueError("The location_id isn't provided")
  if _CLUSTER_ID.value is None:
    raise ValueError("The cluster_id isn't provided")
  if _BUCKET_NAME.value is None:
    raise ValueError("The bucket_name isn't provided")


def _list_mp4_files() -> list[str]:
  """List all mp4 files in the bucket."""
  blobs = storage.Client().list_blobs(_BUCKET_NAME.value)
  mp4_files = []
  print("Listing mp4 files...")
  for blob in blobs:
    name = blob.name
    if not name.endswith(".mp4"):
      continue
    print(name)
    mp4_files.append(name)
  return mp4_files


def _create_processes(
    mp4_files: list[str], connection_options: channel.ConnectionOptions
) -> list[client.Process]:
  """Create process for each mp4 file."""
  processes = []
  bucket_name = _BUCKET_NAME.value
  print("Creating deid processes...")
  for name in mp4_files:
    gcs_input = "gs://" + bucket_name + "/" + name
    gcs_output = (
        "gs://" + bucket_name + "/" + name.split(".")[0] + "_deid_output.mp4"
    )

    process = offline.blur_gcs_video_sync(
        connection_options,
        gcs_input,
        gcs_output,
        _FPS.value,
    )
    print(f"process {process.process_id} is created")
    processes.append(process)
  return processes


async def _wait_for_process(processes, connection_options):
  """Wait for the lva processes to finish."""
  print("Waiting for processes to finish...")
  for process in processes:
    process_id = process.process_id
    try:
      process.wait(connection_options)
    except TimeoutError:
      print(f"{process_id} takes too long to finish")
    process_state = client.get_process(
        connection_options, process_id
    ).run_status_state
    print(f"process {process_id} state is {process_state}")
  print("All processes have finished, please check the GCS bucket!")


def main(unused_argv) -> None:
  _validate_args()
  # List all mp4 files in the bucket.
  gcs_inputs = _list_mp4_files()

  connection_options = channel.ConnectionOptions(
      project_id=_PROJECT_ID.value,
      location_id=_LOCATION_ID.value,
      cluster_id=_CLUSTER_ID.value,
      env=channel.Environment[_ENV.value],
  )
  # Create process for each mp4 file.
  processes = _create_processes(gcs_inputs, connection_options)

  # Wait for processes to finish.
  asyncio.run(_wait_for_process(processes, connection_options))


if __name__ == "__main__":
  app.run(main)
