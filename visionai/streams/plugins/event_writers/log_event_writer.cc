// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/event_writers/log_event_writer.h"

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

absl::Status LogEventWriter::Init(EventWriterInitContext* ctx) {
  return absl::OkStatus();
}

absl::Status LogEventWriter::Open(absl::string_view event_id) {
  event_id_ = std::string(event_id);
  return absl::OkStatus();
}

absl::Status LogEventWriter::Write(Packet p) {
  LOG(INFO) << absl::StrFormat("(%s) ", event_id_) << p.DebugString();
  return absl::OkStatus();
}

absl::Status LogEventWriter::Close() {
  return absl::OkStatus();
}

REGISTER_EVENT_WRITER_INTERFACE("LogEventWriter").Doc(R"doc(
LogEventWriter logs the received packet.
It simply prints to stdout.
)doc");

REGISTER_EVENT_WRITER_IMPLEMENTATION("LogEventWriter", LogEventWriter);

}  // namespace visionai
