// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/filter.h"

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/event_manager.h"
#include "visionai/streams/filtered_element.h"

namespace visionai {

FilterInitContext::FilterInitContext(InitData d) : attrs_(std::move(d.attrs)) {}

FilterRunContext::FilterRunContext(RunData d)
    : input_buffer_(std::move(d.input_buffer)),
      output_buffer_(std::move(d.output_buffer)),
      event_manager_(std::move(d.event_manager)) {}

absl::Status FilterRunContext::Push(absl::string_view event_id, Packet p) {
  VAI_ASSIGN_OR_RETURN(auto f, MakePacketFilteredElement(event_id, std::move(p)),
                   _ << "while converting a packet into a filtered element.");
  if (!output_buffer_->TryEmplace(std::move(f))) {
    LOG_EVERY_T(WARNING, 1)
        << "The filtered element queue is currently full; dropping an element.";
  }
  return absl::OkStatus();
}

absl::Status FilterRunContext::Poll(Packet* p, absl::Duration timeout) {
  if (!input_buffer_->TryPopBack(*p, timeout)) {
    return absl::UnavailableError(
        "No inputs arrived before the timeout expired.");
  }
  return absl::OkStatus();
}

absl::StatusOr<std::string> FilterRunContext::StartEvent() {
  VAI_ASSIGN_OR_RETURN(auto event_id, event_manager_->Open(),
                   _ << "while attempting to open a new event");
  VAI_ASSIGN_OR_RETURN(auto f, MakeOpenFilteredElement(event_id),
                   _ << "while making an (open) control filtered element");
  if (!output_buffer_->TryEmplace(std::move(f))) {
    return absl::ResourceExhaustedError(
        "The filtered element queue is full; could not push a control "
        "element.");
  }
  return event_id;
}

absl::Status FilterRunContext::EndEvent(absl::string_view event_id) {
  VAI_ASSIGN_OR_RETURN(auto f, MakeCloseFilteredElement(event_id),
                   _ << "while making a (close) control filtered element");
  if (!output_buffer_->TryEmplace(std::move(f))) {
    return absl::ResourceExhaustedError(
        "The filtered element queue is full; could not push a control "
        "element.");
  }
  return absl::OkStatus();
}

FilterRegistry* FilterRegistry::Global() {
  static FilterRegistry* global_registry = new FilterRegistry;
  return global_registry;
}

void FilterRegistry::Register(absl::string_view filter_name,
                              FilterFactory filter_factory) {
  auto s = RegisterHelper(filter_name, filter_factory);
  CHECK(s.ok()) << s;
}

absl::Status FilterRegistry::RegisterHelper(absl::string_view filter_name,
                                            FilterFactory filter_factory) {
  if (!registry_.insert({std::string(filter_name), filter_factory}).second) {
    return absl::AlreadyExistsError(absl::StrFormat(
        "Filter type '%s' already has a definition registered.", filter_name));
  }
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<Filter>> FilterRegistry::CreateFilter(
    absl::string_view filter_name) {
  auto it = registry_.find(filter_name);
  if (it == registry_.end()) {
    return absl::NotFoundError(
        absl::StrFormat("Filter type '%s' was not registered.", filter_name));
  }
  return std::unique_ptr<Filter>((it->second)());
}

}  // namespace visionai
