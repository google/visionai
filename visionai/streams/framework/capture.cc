// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/capture.h"

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"

namespace visionai {

CaptureInitContext::CaptureInitContext(InitData d)
    : input_urls_(std::move(d.input_urls)), attrs_(std::move(d.attrs)) {}

absl::Status CaptureInitContext::GetInputUrl(std::string* url) const {
  if (input_urls_.size() != 1) {
    return absl::FailedPreconditionError(absl::StrFormat(
        "Expected exactly 1 input url, but got %d.", input_urls_.size()));
  }
  if (url == nullptr) {
    return absl::InvalidArgumentError(
        "Got a null input url receiving pointer.");
  }
  *url = input_urls_[0];
  return absl::OkStatus();
}

absl::Status CaptureInitContext::GetInputUrls(
    std::vector<std::string>* urls) const {
  if (input_urls_.size() <= 1) {
    return absl::FailedPreconditionError(absl::StrFormat(
        "Expected more than 1 input url, but got %d.", input_urls_.size()));
  }
  if (urls == nullptr) {
    return absl::InvalidArgumentError(
        "Got a null input urls receiving pointer.");
  }
  *urls = input_urls_;
  return absl::OkStatus();
}

CaptureRunContext::CaptureRunContext(RunData d)
    : output_buffer_(std::move(d.output_buffer)) {}

CaptureRegistry* CaptureRegistry::Global() {
  static CaptureRegistry* global_registry = new CaptureRegistry;
  return global_registry;
}

void CaptureRegistry::Register(absl::string_view capture_name,
                               CaptureFactory capture_factory) {
  auto s = RegisterHelper(capture_name, capture_factory);
  CHECK(s.ok()) << s;
}

absl::Status CaptureRegistry::RegisterHelper(absl::string_view capture_name,
                                             CaptureFactory capture_factory) {
  if (!registry_.insert({std::string(capture_name), capture_factory}).second) {
    return absl::AlreadyExistsError(absl::StrFormat(
        "Capture type '%s' already has a definition registered.",
        capture_name));
  }
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<Capture>> CaptureRegistry::CreateCapture(
    absl::string_view capture_name) {
  auto it = registry_.find(capture_name);
  if (it == registry_.end()) {
    return absl::NotFoundError(
        absl::StrFormat("Capture type '%s' was not registered.", capture_name));
  }
  return std::unique_ptr<Capture>((it->second)());
}

}  // namespace visionai
