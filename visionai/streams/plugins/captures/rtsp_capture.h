/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_RTSP_CAPTURE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_RTSP_CAPTURE_H_

#include "absl/synchronization/notification.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"

namespace visionai {

// RTSPCapture receives encoded frames from an RTSP uri source.
class RTSPCapture : public Capture {
 public:
  RTSPCapture() {}

  ~RTSPCapture() override {}

  // Gets the attributes that the users specify at runtime.
  absl::Status Init(CaptureInitContext* ctx) override;

  // Receives media data from RTSP server and pushes encoded frames to ctx.
  absl::Status Run(CaptureRunContext* ctx) override;

  // Arrange for the possibility for cancellation.
  absl::Status Cancel() override;

 private:
  std::string source_uri_;
  int timeout_seconds_ = 10;
  int64_t last_frame_pts_ = -1;
  int64_t last_frame_dts_ = 0;
  int64_t last_frame_duration_ = -1;

  absl::Notification is_cancelled_;

  std::string GstPipelineStr();

  // Validates whether the rtsp input is encoded in H264
  absl::Status IsRTSPH264Input();

  virtual absl::Status ExecuteH264ValidatorGstreamerRunner(
    const GstreamerRunner::Options& options);

  virtual absl::Status ExecuteGstreamerRunner(
      const GstreamerRunner::Options& options);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_CAPTURES_RTSP_CAPTURE_H_
