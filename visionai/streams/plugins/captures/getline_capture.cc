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

// GetlineCapture reads a line from stdin and pushes it as a string packet.
class GetlineCapture : public Capture {
 public:
  GetlineCapture() {}

  ~GetlineCapture() override {}

  absl::Status Init(CaptureInitContext* ctx) override {
    return absl::OkStatus();
  }

  absl::Status Run(CaptureRunContext* ctx) override {
    while (!is_cancelled_.HasBeenNotified()) {
      std::string line;
      std::getline(std::cin, line);
      VAI_ASSIGN_OR_RETURN(auto p, MakePacket(std::move(line)));
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
    }
    return absl::OkStatus();
  }

  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  absl::Notification is_cancelled_;
};

REGISTER_CAPTURE_INTERFACE("GetlineCapture").Doc(R"doc(
GetlineCapture reads a line from stdin and pushes it as a string packet.
")doc");

REGISTER_CAPTURE_IMPLEMENTATION("GetlineCapture", GetlineCapture);

}  // namespace visionai
