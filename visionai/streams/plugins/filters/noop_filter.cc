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
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// NoopFilter (No-Op) simply forwards the received packet.
// All packets go into one event.
class NoopFilter : public Filter {
 public:
  NoopFilter() {}

  ~NoopFilter() override {}

  absl::Status Init(FilterInitContext* ctx) override {
    return absl::OkStatus();
  }

  absl::Status Run(FilterRunContext* ctx) override {
    // Start the event.
    VAI_ASSIGN_OR_RETURN(auto event_id, ctx->StartEvent());

    // Repeatedly forward packets into the event.
    Packet p;
    while (!is_cancelled_.HasBeenNotified()) {
      auto s = ctx->Poll(&p, absl::Seconds(1));
      if (!s.ok()) {
        continue;
      }
      VAI_RETURN_IF_ERROR(ctx->Push(event_id, std::move(p)));
    }

    // End the event.
    VAI_RETURN_IF_ERROR(ctx->EndEvent(event_id));

    return absl::OkStatus();
  }

  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  absl::Notification is_cancelled_;
};

REGISTER_FILTER_INTERFACE("NoopFilter").Doc(R"doc(
NoopFilter just forwards all the packets it receives into
a single event.
)doc");

REGISTER_FILTER_IMPLEMENTATION("NoopFilter", NoopFilter);

}  // namespace visionai
