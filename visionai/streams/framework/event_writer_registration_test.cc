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
#include "visionai/streams/framework/event_writer_def_registry.h"

namespace visionai {

namespace {

void Register(absl::string_view event_writer_name,
              EventWriterDefRegistry* registry) {
  registry->Register(
      [event_writer_name](EventWriterDef* event_writer_def) -> absl::Status {
        event_writer_def->set_name(std::string(event_writer_name));
        return absl::OkStatus();
      });
}

}  // namespace

TEST(EventWriterDefRegistryTest, TestBasic) {
  std::unique_ptr<EventWriterDefRegistry> registry(new EventWriterDefRegistry);

  // Basic insertion and lookup.
  Register("Foo", registry.get());
  {
    auto event_writer_def = registry->LookUp("Foo");
    EXPECT_TRUE(event_writer_def.ok());
    EXPECT_EQ((*event_writer_def)->name(), "Foo");
  }

  // Lookup one that doesn't exist.
  {
    auto event_writer_def = registry->LookUp("Bar");
    EXPECT_FALSE(event_writer_def.ok());
  }
}

TEST(EventWriterDefRegistryTest, TestDuplicate) {
  std::unique_ptr<EventWriterDefRegistry> registry(new EventWriterDefRegistry);
  Register("Foo", registry.get());
  EXPECT_DEATH({ Register("Foo", registry.get()); },
               "EventWriter type 'Foo' is already registered.");
}

}  // namespace visionai
