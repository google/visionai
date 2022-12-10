// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/event_update.h"

#include <memory>
#include <string>
#include <type_traits>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "visionai/streams/client/resource_util.h"

namespace visionai {

namespace {

constexpr char kTestClusterName[] =
    "projects/test-project/locations/test-location/clusters/test-cluster";
constexpr char kTestStreamId[] = "my-stream";
constexpr char kTestEventId[] = "my-event";

}  // namespace

TEST(EventUpdateTest, OffsetTest) {
  {
    EventUpdate e;
    int64_t value = 42;
    auto status = SetOffset(42, &e);
    EXPECT_TRUE(status.ok());

    auto result = GetOffset(e);
    EXPECT_EQ(value, result);
  }
  {
    auto status = SetOffset(42, nullptr);
    EXPECT_FALSE(status.ok());
  }
}

TEST(EventUpdateTest, GetStreamIdTest) {
  auto stream_name = MakeStreamName(kTestClusterName, kTestStreamId).value();
  {
    EventUpdate e;
    auto stream_id_statusor = GetStreamId(e);
    EXPECT_FALSE(stream_id_statusor.ok());
  }
  {
    EventUpdate e;
    e.set_stream(stream_name);
    auto stream_id_statusor = GetStreamId(e);
    EXPECT_TRUE(stream_id_statusor.ok());
    EXPECT_EQ(*stream_id_statusor, kTestStreamId);
  }
}

TEST(EventUpdateTest, GetEventIdTest) {
  auto event_name = MakeEventName(kTestClusterName, kTestEventId).value();
  {
    EventUpdate e;
    auto event_id_statusor = GetEventId(e);
    EXPECT_FALSE(event_id_statusor.ok());
  }
  {
    EventUpdate e;
    e.set_event(event_name);
    auto event_id_statusor = GetEventId(e);
    EXPECT_TRUE(event_id_statusor.ok());
    EXPECT_EQ(*event_id_statusor, kTestEventId);
  }
}

}  // namespace visionai
