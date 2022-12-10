// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/filter_def_registry.h"

namespace visionai {

namespace {

void Register(absl::string_view filter_name, FilterDefRegistry* registry) {
  registry->Register([filter_name](FilterDef* filter_def) -> absl::Status {
    filter_def->set_name(std::string(filter_name));
    return absl::OkStatus();
  });
}

}  // namespace

TEST(FilterDefRegistryTest, TestBasic) {
  std::unique_ptr<FilterDefRegistry> registry(new FilterDefRegistry);

  // Basic insertion and lookup.
  Register("Foo", registry.get());
  {
    auto filter_def = registry->LookUp("Foo");
    EXPECT_TRUE(filter_def.ok());
    EXPECT_EQ((*filter_def)->name(), "Foo");
  }

  // Lookup one that doesn't exist.
  {
    auto filter_def = registry->LookUp("Bar");
    EXPECT_FALSE(filter_def.ok());
  }
}

TEST(FilterDefRegistryTest, TestDuplicate) {
  std::unique_ptr<FilterDefRegistry> registry(new FilterDefRegistry);
  Register("Foo", registry.get());
  EXPECT_DEATH({ Register("Foo", registry.get()); },
               "Filter type 'Foo' is already registered.");
}

}  // namespace visionai
