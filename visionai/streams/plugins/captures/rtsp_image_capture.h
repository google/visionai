/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_RTSP_IMAGE_CAPTURE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_RTSP_IMAGE_CAPTURE_H_

#include "absl/synchronization/notification.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"

namespace visionai {

// RTSPImageCapture receives image data from an RTSP uri source.
class RTSPImageCapture : public Capture {
 public:
  RTSPImageCapture() {}

  ~RTSPImageCapture() override {}

  // Gets the attributes that the users specify at runtime.
  absl::Status Init(CaptureInitContext* ctx) override;

  // Receives media data from RTSP server and pushes the raw image packets to
  // ctx.
  absl::Status Run(CaptureRunContext* ctx) override;

  // Arrange for the possibility for cancellation.
  absl::Status Cancel() override;

 private:
  std::string camera_id_;
  std::string frame_rate_;
  int max_captured_images_;
  std::string source_uri_;
  int buffer_size_ = 100;
  int timeout_seconds_ = 10;

  absl::Notification is_cancelled_;

  std::string InputGstPipelineStr();
  std::string OutputGstPipelineStr();

  // Validates whether the rtsp input is encoded in H264
  absl::Status IsRTSPH264Input();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_RTSP_IMAGE_CAPTURE_H_
