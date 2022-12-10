// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// IntSequenceCapture just pushes int packets at a regular interval.
class IntSequenceCapture : public Capture {
 public:
  IntSequenceCapture() {}

  ~IntSequenceCapture() override {}

  absl::Status Init(CaptureInitContext* ctx) override {
    VAI_RETURN_IF_ERROR(
        ctx->GetAttr<int>("publish_period_ms", &publish_period_ms_));
    return absl::OkStatus();
  }

  absl::Status Run(CaptureRunContext* ctx) override {
    for (int i = 0; !is_cancelled_.HasBeenNotified(); ++i) {
      absl::SleepFor(absl::Milliseconds(publish_period_ms_));
      VAI_ASSIGN_OR_RETURN(auto p, MakePacket(i));
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
    }
    return absl::OkStatus();
  }

  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  int publish_period_ms_ = 100;
  absl::Notification is_cancelled_;
};

REGISTER_CAPTURE_INTERFACE("IntSequenceCapture")
    .Attr("publish_period_ms", "int")
    .Doc(R"doc(
IntSequenceCapture illustrates how one might write a new Capture.

publish_period_ms: The internal between packet pushes.
")doc");

REGISTER_CAPTURE_IMPLEMENTATION("IntSequenceCapture", IntSequenceCapture);

}  // namespace visionai
