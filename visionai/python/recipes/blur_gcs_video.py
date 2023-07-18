# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Recipe for an LVA graph for blurring Cloud Storage videos."""

from typing import Dict

from visionai.python.lva import graph
from visionai.python.ops import gen_ops


class BlurGcsVideo:
  """BlurGcsVideo objects is a recipe for blurring Cloud Storage videos."""

  _DEFAULT_BLUR_GCS_VIDEO_ANALYSIS_ID = "default-blur-gcs-video"
  _INPUT_VIDEO_GCS_PATH_ATTRIBUTE = "input_video_gcs_path"
  _OUTPUT_VIDEO_GCS_PATH_ATTRIBUTE = "output_video_gcs_path"
  _FRAME_RATE_ATTRIBUTE = "frame_rate"
  _SOURCE_ANALYZER = "gcs_video_source"
  _SINK_ANALYZER = "gcs_video_sink"
  _TRANSFORMER_ANALYZER = "person_blur"
  _INPUT_ARGUMENT = "input_stream"
  _OUTPUT_ARGUMENT = "output_stream"

  def default_blur_gcs_video_analysis_id(self) -> str:
    """Get the default analysis-id for blurring gcs videos."""
    return self._DEFAULT_BLUR_GCS_VIDEO_ANALYSIS_ID

  def get_blur_gcs_video_overrides(
      self,
      gcs_infile: str,
      gcs_outfile: str,
      fps: int,
  ) -> Dict[str, str]:
    """Produces the attribute overrides for blur gcs video."""
    # TODO(b/278571197): May do some more comprehensive checks here. Can open
    # another bug if this might take some more time.
    if not gcs_infile:
      raise ValueError("The input url to the GCS file cannot be empty.")

    if not gcs_infile.startswith("gs://"):
      raise ValueError("The input url isn't a GCS file")

    if not gcs_outfile:
      raise ValueError("The output url to the GCS file cannot be empty.")

    if not gcs_outfile.startswith("gs://"):
      raise ValueError("The output url isn't a GCS file")

    return {
        self._SOURCE_ANALYZER
        + ":"
        + self._INPUT_VIDEO_GCS_PATH_ATTRIBUTE: gcs_infile,
        self._SINK_ANALYZER
        + ":"
        + self._OUTPUT_VIDEO_GCS_PATH_ATTRIBUTE: gcs_outfile,
        self._TRANSFORMER_ANALYZER + ":" + self._FRAME_RATE_ATTRIBUTE: str(fps),
    }

  def create_graph(self) -> graph.Graph:
    """Creates a pre-made graph used for blurring Cloud Storage videos.

    Returns:
      A `Graph` that blurs Cloud Storage videos.
    """
    g = graph.Graph()
    gcs_video_source_output = gen_ops.gcs_video_source(
        "", self._SOURCE_ANALYZER, g
    )
    de_id_output = gen_ops.de_id(
        gcs_video_source_output,
        True,
        "BLURRING",
        6,
        self._TRANSFORMER_ANALYZER,
        g,
    )
    gen_ops.gcs_video_sink(de_id_output, "", self._SINK_ANALYZER, g)
    return g
