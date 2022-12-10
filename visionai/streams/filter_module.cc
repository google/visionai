// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/filter_module.h"

#include <algorithm>
#include <memory>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/attr_def.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"
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

FilterModule::FilterModule(const visionai::FilterConfig& config)
    : config_(config) {}

FilterModule& FilterModule::AttachInput(
    std::shared_ptr<RingBuffer<Packet>> filter_input_buffer) {
  filter_input_buffer_ = std::move(filter_input_buffer);
  return *this;
}

FilterModule& FilterModule::AttachOutput(
    std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer) {
  filter_output_buffer_ = std::move(output_buffer);
  return *this;
}

FilterModule& FilterModule::AttachEventManager(
    std::shared_ptr<EventManager> event_manager) {
  event_manager_ = std::move(event_manager);
  return *this;
}

absl::StatusOr<std::unique_ptr<FilterInitContext>>
FilterModule::CreateFilterInitContext() {
  VAI_ASSIGN_OR_RETURN(auto filter_def,
                   FilterDefRegistry::Global()->LookUp(config_.name()),
                   _ << "while looking up the FilterDef in the registry");

  absl::flat_hash_map<std::string, AttrValue> attrs;
  for (const auto& p : config_.attr()) {
    auto attr_def = FindAttrDef(p.first, *filter_def);
    if (attr_def == nullptr) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Given a value for attribute '%s', but that "
                          "attribute is not defined for the Filter type '%s'",
                          p.first, config_.name()));
    }
    VAI_ASSIGN_OR_RETURN(
        auto attr_value, ParseAttrValue(attr_def->type(), p.second),
        _ << "while parsing the value for attribute '" << p.first << "'");
    if (!attrs.insert({p.first, std::move(attr_value)}).second) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "A value for attribute '%s' was specified more than once.", p.first));
    }
  }

  FilterInitContext::InitData d;
  d.attrs = std::move(attrs);
  return std::make_unique<FilterInitContext>(std::move(d));
}

absl::StatusOr<std::unique_ptr<FilterRunContext>>
FilterModule::CreateFilterRunContext() {
  if (filter_output_buffer_ == nullptr) {
    return absl::InternalError(
        "The filter output buffer has not been attached.");
  }
  FilterRunContext::RunData d;
  d.input_buffer = filter_input_buffer_;
  d.output_buffer = filter_output_buffer_;
  d.event_manager = event_manager_;
  return std::make_unique<FilterRunContext>(std::move(d));
}

absl::StatusOr<std::unique_ptr<Filter>> FilterModule::CreateFilter() {
  VAI_ASSIGN_OR_RETURN(auto filter,
                   FilterRegistry::Global()->CreateFilter(config_.name()),
                   _ << "while creating a Filter");
  return std::move(filter);
}

absl::Status FilterModule::Prepare() {
  VAI_ASSIGN_OR_RETURN(filter_init_ctx_, CreateFilterInitContext(),
                   _ << "while attempting to create the FilterInitContext");
  VAI_ASSIGN_OR_RETURN(filter_run_ctx_, CreateFilterRunContext(),
                   _ << "while attempting to create the FilterRunContext");
  VAI_ASSIGN_OR_RETURN(filter_, CreateFilter(),
                   _ << "while attempting to create the Filter");
  return absl::OkStatus();
}

absl::Status FilterModule::Init() {
  if (filter_ == nullptr) {
    return absl::InternalError(
        "The Filter is a nullptr. Is FilterModule::Initialize called?");
  }
  if (filter_init_ctx_ == nullptr) {
    return absl::InternalError(
        "The FilterInitContext is a nullptr. Is FilterModule::Initialize "
        "called?");
  }
  VAI_RETURN_IF_ERROR(filter_->Init(filter_init_ctx_.get()))
      << "while initializing the Filter";
  return absl::OkStatus();
}

absl::Status FilterModule::Run() {
  if (filter_ == nullptr) {
    return absl::InternalError(
        "The Filter is a nullptr. Is FilterModule::Initialize called?");
  }
  if (filter_run_ctx_ == nullptr) {
    return absl::InternalError(
        "The FilterRunContext is a nullptr. Is FilterModule::Initialize "
        "called?");
  }
  return filter_->Run(filter_run_ctx_.get());
}

absl::Status FilterModule::Cancel() {
  if (filter_ == nullptr) {
    return absl::InternalError(
        "The Filter is a nullptr. Is FilterModule::Initialize called?");
  }
  return filter_->Cancel();
}

}  // namespace visionai
