// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/capture_module.h"

#include <algorithm>
#include <memory>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/attr_def.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"
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

CaptureModule::CaptureModule(const visionai::CaptureConfig& config)
    : config_(config) {}

CaptureModule& CaptureModule::AttachOutput(
    std::shared_ptr<RingBuffer<Packet>> capture_output_buffer) {
  capture_output_buffer_ = std::move(capture_output_buffer);
  return *this;
}

absl::StatusOr<std::unique_ptr<CaptureInitContext>>
CaptureModule::CreateCaptureInitContext() {
  VAI_ASSIGN_OR_RETURN(auto capture_def,
                   CaptureDefRegistry::Global()->LookUp(config_.name()),
                   _ << "while looking up the CaptureDef in the registry");

  absl::flat_hash_map<std::string, AttrValue> attrs;
  for (const auto& p : config_.attr()) {
    auto attr_def = FindAttrDef(p.first, *capture_def);
    if (attr_def == nullptr) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Given a value for attribute '%s', but that "
                          "attribute is not defined for the Capture type '%s'",
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

  CaptureInitContext::InitData d;
  d.input_urls.resize(config_.source_urls().size());
  std::copy(config_.source_urls().begin(), config_.source_urls().end(),
            d.input_urls.begin());
  d.attrs = std::move(attrs);
  return std::make_unique<CaptureInitContext>(std::move(d));
}

absl::StatusOr<std::unique_ptr<CaptureRunContext>>
CaptureModule::CreateCaptureRunContext() {
  if (capture_output_buffer_ == nullptr) {
    return absl::InternalError(
        "The capture output buffer has not been attached.");
  }
  CaptureRunContext::RunData d;
  d.output_buffer = capture_output_buffer_;
  return std::make_unique<CaptureRunContext>(std::move(d));
}

absl::StatusOr<std::unique_ptr<Capture>> CaptureModule::CreateCapture() {
  VAI_ASSIGN_OR_RETURN(auto capture,
                   CaptureRegistry::Global()->CreateCapture(config_.name()),
                   _ << "while creating a Capture");
  return std::move(capture);
}

absl::Status CaptureModule::Prepare() {
  VAI_ASSIGN_OR_RETURN(capture_init_ctx_, CreateCaptureInitContext(),
                   _ << "while attempting to create the CaptureInitContext");
  VAI_ASSIGN_OR_RETURN(capture_run_ctx_, CreateCaptureRunContext(),
                   _ << "while attempting to create the CaptureRunContext");
  VAI_ASSIGN_OR_RETURN(capture_, CreateCapture(),
                   _ << "while attempting to create the Capture");
  return absl::OkStatus();
}

absl::Status CaptureModule::Init() {
  if (capture_ == nullptr) {
    return absl::InternalError(
        "The Capture is a nullptr. Is CaptureModule::Initialize called?");
  }
  if (capture_init_ctx_ == nullptr) {
    return absl::InternalError(
        "The CaptureInitContext is a nullptr. Is CaptureModule::Initialize "
        "called?");
  }
  VAI_RETURN_IF_ERROR(capture_->Init(capture_init_ctx_.get()))
      << "while initializing the Capture";
  return absl::OkStatus();
}

absl::Status CaptureModule::Run() {
  if (capture_ == nullptr) {
    return absl::InternalError(
        "The Capture is a nullptr. Is CaptureModule::Initialize called?");
  }
  if (capture_run_ctx_ == nullptr) {
    return absl::InternalError(
        "The CaptureRunContext is a nullptr. Is CaptureModule::Initialize "
        "called?");
  }
  return capture_->Run(capture_run_ctx_.get());
}

absl::Status CaptureModule::Cancel() {
  if (capture_ == nullptr) {
    return absl::InternalError(
        "The Capture is a nullptr. Is CaptureModule::Initialize called?");
  }
  return capture_->Cancel();
}

}  // namespace visionai
