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

// SimpleSegmentFilter partitions the incoming stream into segments that are
// sent to separate events.
class SimpleSegmentFilter : public Filter {
 public:
  SimpleSegmentFilter() {}

  ~SimpleSegmentFilter() override {}

  absl::Status Init(FilterInitContext* ctx) override {
    VAI_RETURN_IF_ERROR(
        ctx->GetAttr<int>("messages_per_segment", &messages_per_segment_))
        << "while getting messages_per_segment";
    return absl::OkStatus();
  }

  absl::Status Run(FilterRunContext* ctx) override {
    while (!is_cancelled_.HasBeenNotified()) {
      VAI_ASSIGN_OR_RETURN(auto event_id, ctx->StartEvent());
      int i = 0;
      while (!is_cancelled_.HasBeenNotified()) {
        if (messages_per_segment_ > 0 && i >= messages_per_segment_) {
          break;
        }
        Packet p;
        auto s = ctx->Poll(&p, absl::Seconds(1));
        if (!s.ok()) {
          continue;
        }
        VAI_RETURN_IF_ERROR(ctx->Push(event_id, std::move(p)));
        ++i;
      }
      VAI_RETURN_IF_ERROR(ctx->EndEvent(event_id));
    }
    return absl::OkStatus();
  }

  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  absl::Notification is_cancelled_;
  int messages_per_segment_ = 0;
};

REGISTER_FILTER_INTERFACE("SimpleSegmentFilter")
    .Attr("messages_per_segment", "int")
    .Doc(R"doc(
SimpleSegmentFilter partitions the incoming stream into segments that are
`messages_per_segment` long and puts each segments into a separate event.
It does not do any transforms on the messages and acts as a no-op.

messages_per_segment: The number of messages per segment. If a zero or negative
                      value is given, then all messages are put into a single
                      segment (effectively the NoOp filter).
)doc");

REGISTER_FILTER_IMPLEMENTATION("SimpleSegmentFilter", SimpleSegmentFilter);

}  // namespace visionai
