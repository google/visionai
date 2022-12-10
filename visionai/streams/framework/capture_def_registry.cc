// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/capture_def_registry.h"

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
#include "visionai/streams/framework/capture_def.pb.h"
#include "visionai/streams/framework/capture_def_builder.h"
#include "visionai/streams/framework/registration.h"

namespace visionai {

CaptureDefRegistry::CaptureDefRegistry() {}

CaptureDefRegistry* CaptureDefRegistry::Global() {
  static CaptureDefRegistry* global_capture_registry = new CaptureDefRegistry;
  return global_capture_registry;
}

void CaptureDefRegistry::Register(
    const CaptureDefFactory& capture_def_factory) {
  auto s = RegisterHelper(capture_def_factory);
  CHECK(s.ok()) << s;
}

absl::Status CaptureDefRegistry::RegisterHelper(
    const CaptureDefFactory& capture_def_factory) {
  std::unique_ptr<CaptureDef> capture_def(new CaptureDef);

  // This stays in the global static registry and cleaned up
  // during program termination. We only want it cleaned up if the registration
  // is unsuccessful.
  absl::IgnoreLeak(capture_def.get());

  auto s = capture_def_factory(capture_def.get());
  if (s.ok()) {
    if (!registry_.insert({capture_def->name(), capture_def.get()}).second) {
      s = absl::AlreadyExistsError(absl::StrFormat(
          "Capture type '%s' is already registered.", capture_def->name()));
    }
  }
  if (s.ok()) {
    capture_def.release();
  } else {
    capture_def.reset();
  }
  return s;
}

absl::StatusOr<const CaptureDef*> CaptureDefRegistry::LookUp(
    absl::string_view capture_name) {
  auto it = registry_.find(capture_name);
  if (it == registry_.end()) {
    return absl::NotFoundError(
        absl::StrFormat("Capture type '%s' was not registered.", capture_name));
  }
  return it->second;
}

namespace internal_registration {

InitOnStartupMarker CaptureDefBuilderWrapper::operator()() {
  CaptureDefRegistry::Global()->Register(
      [builder = std::move(builder_)](CaptureDef* capture_def) -> absl::Status {
        return builder.Finalize(capture_def);
      });
  return {};
}

}  //  namespace internal_registration

}  // namespace visionai
