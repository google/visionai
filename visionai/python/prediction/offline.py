# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""This file contains readily usable offline prediction routines."""

from visionai.python.lva import client
from visionai.python.recipes import blur_gcs_video
from visionai.python.net import channel


def blur_gcs_video_sync(
    connection_options: channel.ConnectionOptions,
    gcs_infile: str,
    gcs_outfile: str,
    fps: int = 6,
) -> client.Process:
  """Blurs an MP4 file stored in Cloud Storage.

  Given an MP4 file on Cloud Storage, apply person blurring to it, and save the
  result into another Cloud Storage file.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    gcs_infile: The gcs url to an MP4 file that is to be blurred.
    gcs_outfile: A gcs url of the MP4 file to save the blurred video.

  Returns:
    A `Process` representing the work.
  """
  blur_gcs_video_recipe = blur_gcs_video.BlurGcsVideo()
  g = blur_gcs_video_recipe.create_graph()
  analysis = client.get_or_create_analysis(
      connection_options,
      g,
      blur_gcs_video_recipe.default_blur_gcs_video_analysis_id(),
  )
  process = client.create_process(
      connection_options,
      analysis.analysis_id,
      blur_gcs_video_recipe.get_blur_gcs_video_overrides(
          gcs_infile, gcs_outfile, fps
      ),
  )
  return process
