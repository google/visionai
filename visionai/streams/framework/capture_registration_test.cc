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
#include "visionai/streams/framework/capture_def_registry.h"

namespace visionai {

namespace {

void Register(absl::string_view capture_name, CaptureDefRegistry* registry) {
  registry->Register([capture_name](CaptureDef* capture_def) -> absl::Status {
    capture_def->set_name(std::string(capture_name));
    return absl::OkStatus();
  });
}

}  // namespace

TEST(CaptureDefRegistryTest, TestBasic) {
  std::unique_ptr<CaptureDefRegistry> registry(new CaptureDefRegistry);

  // Basic insertion and lookup.
  Register("Foo", registry.get());
  {
    auto capture_def = registry->LookUp("Foo");
    EXPECT_TRUE(capture_def.ok());
    EXPECT_EQ((*capture_def)->name(), "Foo");
  }

  // Lookup one that doesn't exist.
  {
    auto capture_def = registry->LookUp("Bar");
    EXPECT_FALSE(capture_def.ok());
  }
}

TEST(CaptureDefRegistryTest, TestDuplicate) {
  std::unique_ptr<CaptureDefRegistry> registry(new CaptureDefRegistry);
  Register("Foo", registry.get());
  EXPECT_DEATH({ Register("Foo", registry.get()); },
               "Capture type 'Foo' is already registered.");
}

}  // namespace visionai
