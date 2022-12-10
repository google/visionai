/*
 * Copyright 2021 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_VIDEO_WRITER_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_VIDEO_WRITER_H_

#include <functional>
#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/raw_image.h"

namespace visionai {

// This class writes a single video file from frames it receives.
class GstreamerVideoWriter {
 public:
  // Options to configure the GstreamerVideoWriter.
  struct Options {
    // The path to the output video file.
    std::string file_path;

    // The caps string of all gstreamer buffers that would be fed.
    std::string caps_string;

    // If set true, the non-h264 inputs will be rejected.
    bool h264_only = false;

    // If set true, the h264 inputs will be muxed without transcoding.
    bool h264_mux_only = false;
  };

  // Create an instance in a fully initialized state.
  static absl::StatusOr<std::unique_ptr<GstreamerVideoWriter>> Create(
      const Options&);

  // Add a gstreamer buffer into the output video.
  //
  // It must have the same caps as that specified in Options.
  absl::Status Put(const GstreamerBuffer&);

  // Returns the Gstreamer pipeline string. Primarily used for unit test.
  std::string GetPipelineStr() const { return pipeline_str_; }

  // Copy-control members. Use Create() rather than the constructors.
  explicit GstreamerVideoWriter(const Options&);
  ~GstreamerVideoWriter() = default;
  GstreamerVideoWriter() = delete;
  GstreamerVideoWriter(const GstreamerVideoWriter&) = delete;
  GstreamerVideoWriter& operator=(const GstreamerVideoWriter&) = delete;
  GstreamerVideoWriter(GstreamerVideoWriter&&) = delete;
  GstreamerVideoWriter& operator=(GstreamerVideoWriter&&) = delete;

 private:
  absl::Status Initialize();

  Options options_;
  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;
  std::string pipeline_str_;
  std::string media_type_;
  std::string frame_rate_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_VIDEO_WRITER_H_
