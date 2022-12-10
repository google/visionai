// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <memory>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// BytesSenderCapture sends bytes at regular intervals.
class BytesSenderCapture : public Capture {
 public:
  BytesSenderCapture() {}

  ~BytesSenderCapture() override {}

  absl::Status Init(CaptureInitContext* ctx) override {
    VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("bytes_per_message", &bytes_per_message_))
        << "while getting bytes_per_message";
    if (bytes_per_message_ <= 0) {
      return absl::InvalidArgumentError(
          absl::StrFormat("The bytes per message must be positive. Got %d.",
                          bytes_per_message_));
    }
    VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("send_period_ms", &send_period_ms_))
        << "while getting send_period_ms";
    if (send_period_ms_ <= 0) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "The send period must be positive. Got %d.", send_period_ms_));
    }
    return absl::OkStatus();
  }

  absl::Status Run(CaptureRunContext* ctx) override {
    double mbps = bytes_per_message_ * 8.0 / send_period_ms_ / 1000.0;
    for (size_t i = 0; !is_cancelled_.WaitForNotificationWithTimeout(
             absl::Milliseconds(send_period_ms_));
         ++i) {
      std::string msg(bytes_per_message_, '.');
      VAI_ASSIGN_OR_RETURN(auto p, MakePacket(std::move(msg)));
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
      LOG_EVERY_N(INFO, 100) << absl::StrFormat(
          "Captured %d byte packets (at ~%.2f Mbps).", i, mbps);
    }
    return absl::OkStatus();
  }

  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  absl::Notification is_cancelled_;

  int bytes_per_message_ = -1;
  int send_period_ms_ = -1;
};

REGISTER_CAPTURE_INTERFACE("BytesSenderCapture")
    .Attr("bytes_per_message", "int")
    .Attr("send_period_ms", "int")
    .Doc(R"doc(
BytesSenderCapture sends bytes at regular intervals.
")doc");

REGISTER_CAPTURE_IMPLEMENTATION("BytesSenderCapture", BytesSenderCapture);

}  // namespace visionai
