/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_FILE_SOURCE_CAPTURE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_FILE_SOURCE_CAPTURE_H_

#include "absl/synchronization/notification.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"

namespace visionai {

// FileSourceImageCapture reads media data from a local file (e.g., mp4 files)
// and outputs decoded RawImages.
class FileSourceImageCapture : public Capture {
 public:
  FileSourceImageCapture() {}
  ~FileSourceImageCapture() override {}

  absl::Status Init(CaptureInitContext* ctx) override;
  absl::Status Run(CaptureRunContext* ctx) override;
  absl::Status Cancel() override;

 private:
  std::string source_uri_;
  std::string frame_rate_;
  bool loop_ = false;
  absl::Notification is_cancelled_;

  // Constructs the GStreamer pipeline command to read from local files.
  std::string GstPipelineStr();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_FILE_SOURCE_CAPTURE_H_
