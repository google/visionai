// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/event_writer_def_registry.h"

#include <functional>
#include <memory>
#include <string>

#include "glog/logging.h"
#include "absl/container/flat_hash_map.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/event_writer_def.pb.h"
#include "visionai/streams/framework/event_writer_def_builder.h"
#include "visionai/streams/framework/registration.h"

namespace visionai {

EventWriterDefRegistry::EventWriterDefRegistry() {}

EventWriterDefRegistry* EventWriterDefRegistry::Global() {
  static EventWriterDefRegistry* global_event_writer_registry =
      new EventWriterDefRegistry;
  return global_event_writer_registry;
}

void EventWriterDefRegistry::Register(
    const EventWriterDefFactory& event_writer_def_factory) {
  auto s = RegisterHelper(event_writer_def_factory);
  CHECK(s.ok()) << s;
}

absl::Status EventWriterDefRegistry::RegisterHelper(
    const EventWriterDefFactory& event_writer_def_factory) {
  std::unique_ptr<EventWriterDef> event_writer_def(new EventWriterDef);

  // This stays in the global static registry and cleaned up
  // during program termination. We only want it cleaned up if the registration
  // is unsuccessful.
  absl::IgnoreLeak(event_writer_def.get());

  auto s = event_writer_def_factory(event_writer_def.get());
  if (s.ok()) {
    if (!registry_.insert({event_writer_def->name(), event_writer_def.get()})
             .second) {
      s = absl::AlreadyExistsError(
          absl::StrFormat("EventWriter type '%s' is already registered.",
                          event_writer_def->name()));
    }
  }
  if (s.ok()) {
    event_writer_def.release();
  } else {
    event_writer_def.reset();
  }
  return s;
}

absl::StatusOr<const EventWriterDef*> EventWriterDefRegistry::LookUp(
    absl::string_view event_writer_name) {
  auto it = registry_.find(event_writer_name);
  if (it == registry_.end()) {
    return absl::NotFoundError(absl::StrFormat(
        "EventWriter type '%s' was not registered.", event_writer_name));
  }
  return it->second;
}

namespace internal_registration {

InitOnStartupMarker EventWriterDefBuilderWrapper::operator()() {
  EventWriterDefRegistry::Global()->Register(
      [builder = std::move(builder_)](EventWriterDef* event_writer_def)
          -> absl::Status { return builder.Finalize(event_writer_def); });
  return {};
}

}  //  namespace internal_registration

}  // namespace visionai
