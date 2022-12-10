// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/event_writer.h"

#include <memory>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/framework/attr_def.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
template <typename T>
const AttrDef* FindAttrDef(absl::string_view name, const T& def) {
  for (int i = 0; i < def.attr_size(); ++i) {
    if (def.attr(i).name() == name) {
      return &def.attr(i);
    }
  }
  return nullptr;
}
}  // namespace

absl::Status EventWriterInitContext::Initialize(
    const EventWriterConfig& config) {
  cluster_selection_ = config.cluster_selection();

  VAI_ASSIGN_OR_RETURN(auto event_writer_def,
                   EventWriterDefRegistry::Global()->LookUp(config.name()),
                   _ << "while looking up the EventWriterDef in the registry");
  for (const auto& p : config.attr()) {
    auto attr_def = FindAttrDef(p.first, *event_writer_def);
    if (attr_def == nullptr) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Given a value for attribute '%s', but that "
          "attribute is not defined for the EventWriter type '%s'",
          p.first, config.name()));
    }
    VAI_ASSIGN_OR_RETURN(
        auto attr_value, ParseAttrValue(attr_def->type(), p.second),
        _ << "while parsing the value for attribute '" << p.first << "'");
    if (!attrs_.insert({p.first, std::move(attr_value)}).second) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "A value for attribute '%s' was specified more than once.", p.first));
    }
  }
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<EventWriterInitContext>>
EventWriterInitContext::Create(const EventWriterConfig& config) {
  auto ctx = std::make_unique<EventWriterInitContext>();
  VAI_RETURN_IF_ERROR(ctx->Initialize(config))
      << "while initializing the EventWriterInitContext";
  return std::move(ctx);
}

absl::StatusOr<ClusterSelection> EventWriterInitContext::GetClusterSelection()
    const {
  return cluster_selection_;
}

EventWriterRegistry* EventWriterRegistry::Global() {
  static EventWriterRegistry* global_registry = new EventWriterRegistry;
  return global_registry;
}

void EventWriterRegistry::Register(absl::string_view event_writer_name,
                                   EventWriterFactory event_writer_factory) {
  auto s = RegisterHelper(event_writer_name, event_writer_factory);
  CHECK(s.ok()) << s;
}

absl::Status EventWriterRegistry::RegisterHelper(
    absl::string_view event_writer_name,
    EventWriterFactory event_writer_factory) {
  if (!registry_.insert({std::string(event_writer_name), event_writer_factory})
           .second) {
    return absl::AlreadyExistsError(absl::StrFormat(
        "EventWriter type '%s' already has a definition registered.",
        event_writer_name));
  }
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<EventWriter>>
EventWriterRegistry::CreateEventWriter(absl::string_view event_writer_name) {
  auto it = registry_.find(event_writer_name);
  if (it == registry_.end()) {
    return absl::NotFoundError(absl::StrFormat(
        "EventWriter type '%s' was not registered.", event_writer_name));
  }
  return std::unique_ptr<EventWriter>((it->second)());
}

}  // namespace visionai
