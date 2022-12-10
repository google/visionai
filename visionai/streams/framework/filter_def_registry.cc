// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/filter_def_registry.h"

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
#include "visionai/streams/framework/filter_def.pb.h"
#include "visionai/streams/framework/filter_def_builder.h"
#include "visionai/streams/framework/registration.h"

namespace visionai {

FilterDefRegistry::FilterDefRegistry() {}

FilterDefRegistry* FilterDefRegistry::Global() {
  static FilterDefRegistry* global_filter_registry = new FilterDefRegistry;
  return global_filter_registry;
}

void FilterDefRegistry::Register(const FilterDefFactory& filter_def_factory) {
  auto s = RegisterHelper(filter_def_factory);
  CHECK(s.ok()) << s;
}

absl::Status FilterDefRegistry::RegisterHelper(
    const FilterDefFactory& filter_def_factory) {
  std::unique_ptr<FilterDef> filter_def(new FilterDef);

  // This stays in the global static registry and cleaned up
  // during program termination. We only want it cleaned up if the registration
  // is unsuccessful.
  absl::IgnoreLeak(filter_def.get());

  auto s = filter_def_factory(filter_def.get());
  if (s.ok()) {
    if (!registry_.insert({filter_def->name(), filter_def.get()}).second) {
      s = absl::AlreadyExistsError(absl::StrFormat(
          "Filter type '%s' is already registered.", filter_def->name()));
    }
  }
  if (s.ok()) {
    filter_def.release();
  } else {
    filter_def.reset();
  }
  return s;
}

absl::StatusOr<const FilterDef*> FilterDefRegistry::LookUp(
    absl::string_view filter_name) {
  auto it = registry_.find(filter_name);
  if (it == registry_.end()) {
    return absl::NotFoundError(
        absl::StrFormat("Filter type '%s' was not registered.", filter_name));
  }
  return it->second;
}

namespace internal_registration {

InitOnStartupMarker FilterDefBuilderWrapper::operator()() {
  FilterDefRegistry::Global()->Register(
      [builder = std::move(builder_)](FilterDef* filter_def) -> absl::Status {
        return builder.Finalize(filter_def);
      });
  return {};
}

}  //  namespace internal_registration

}  // namespace visionai
