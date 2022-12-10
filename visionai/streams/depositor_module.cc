// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/depositor_module.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "absl/cleanup/cleanup.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/streams/constants.h"
#include "visionai/streams/event_sink.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/streams/framework/attr_def.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/random_string.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

DepositorModule::DepositorModule(const visionai::DepositorConfig& config)
    : config_(config) {}

DepositorModule& DepositorModule::AttachInput(
    std::shared_ptr<ProducerConsumerQueue<FilteredElement>> input_buffer) {
  depositor_input_buffer_ = std::move(input_buffer);
  return *this;
}

DepositorModule& DepositorModule::AttachEventManager(
    std::shared_ptr<EventManager> event_manager) {
  event_manager_ = std::move(event_manager);
  return *this;
}

absl::Status DepositorModule::Prepare() { return absl::OkStatus(); }

absl::Status DepositorModule::Run() {
  auto input_poll_timeout =
      absl::Milliseconds(kDefaultDepositorInputPollTimeoutMs);
  if (config_.input_poll_timeout_ms() > 0) {
    input_poll_timeout = absl::Milliseconds(config_.input_poll_timeout_ms());
  }

  while (!is_cancelled_.HasBeenNotified()) {
    FilteredElement f;
    if (!depositor_input_buffer_->TryPop(f, input_poll_timeout)) {
      continue;
    }

    switch (f.type()) {
      case FilteredElementType::kOpen:
        VAI_RETURN_IF_ERROR(HandleOpenFilteredElement(std::move(f)))
            << "while handling an (open) control filtered element";
        break;
      case FilteredElementType::kClose:
        VAI_RETURN_IF_ERROR(HandleCloseFilteredElement(std::move(f)))
            << "while handling a (close) control filtered element";
        break;
      case FilteredElementType::kPacket:
        VAI_RETURN_IF_ERROR(HandlePacketFilteredElement(std::move(f)))
            << "while handling a packet filtered element";
        break;
      default:
        return absl::UnimplementedError(absl::StrFormat(
            "No handler for filter element of type %d", f.type()));
    }
  }
  return absl::OkStatus();
}

absl::Status DepositorModule::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

absl::Status DepositorModule::HandleOpenFilteredElement(FilteredElement f) {
  VAI_ASSIGN_OR_RETURN(
      auto event_sink, event_manager_->GetEventSink(f.event_id()),
      _ << absl::StrFormat("while trying to get the write head for \"%s\"",
                           f.event_id()));
  auto p = sinks_.insert(std::make_pair(f.event_id(), event_sink));
  if (!p.second) {
    return absl::FailedPreconditionError(
        absl::StrFormat("The event id \"%s\" is already open.", f.event_id()));
  }
  return absl::OkStatus();
}

absl::Status DepositorModule::HandleCloseFilteredElement(FilteredElement f) {
  if (!sinks_.contains(f.event_id())) {
    return absl::FailedPreconditionError(
        absl::StrFormat("The event id \"%s\" is not open.", f.event_id()));
  }
  VAI_RETURN_IF_ERROR(event_manager_->Close(f.event_id()))
      << absl::StrFormat("while closing event id \"%s\"", f.event_id());
  sinks_.erase(f.event_id());
  return absl::OkStatus();
}

absl::Status DepositorModule::HandlePacketFilteredElement(FilteredElement f) {
  if (!sinks_.contains(f.event_id())) {
    return absl::FailedPreconditionError(
        absl::StrFormat("The event id \"%s\" is not open.", f.event_id()));
  }
  auto event_sink = sinks_[f.event_id()];
  VAI_ASSIGN_OR_RETURN(auto p, PacketFromFilteredElement(std::move(f)),
                   _ << "while converting a filtered element to a packet");
  VAI_RETURN_IF_ERROR(event_sink->Write(std::move(p)))
      << "while writing a packet into an event sink";
  return absl::OkStatus();
}

}  // namespace visionai
