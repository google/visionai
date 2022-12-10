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

// FileSourceCapture reads media data from a local file (e.g., mp4 files)
// and outputs encoded frames. If the input media type is not "video/x-h264"
// will print it as an error in the vaictl.
class FileSourceCapture : public Capture {
 public:
  FileSourceCapture() {}
  ~FileSourceCapture() override {}

  absl::Status Init(CaptureInitContext* ctx) override;
  absl::Status Run(CaptureRunContext* ctx) override;
  absl::Status Cancel() override;

 private:
  std::string source_uri_;
  // Defaults to max int which is almost infinite looping. The function is
  // mostly for testing, so we don't need true infinite. This makes testing
  // easier, and allows specifying this value too.
  int64_t loop_count_ = std::numeric_limits<int64_t>::max();
  absl::Notification is_cancelled_;

  // Constructs the GStreamer pipeline command to read from local files.
  std::string GstPipelineStr();

  // Checks whether the input media type is "video/x-h264".
  absl::Status IsH264Input();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_FILE_SOURCE_CAPTURE_H_
