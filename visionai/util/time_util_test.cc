// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/time_util.h"

#include "gtest/gtest.h"

namespace visionai {

namespace {

TEST(TimeUtilTest, ToAbseilTimestampFromProto) {
  ::google::protobuf::Timestamp proto_timestamp;
  proto_timestamp.set_seconds(1604125796);
  absl::Time time = ToAbseilTimestamp(proto_timestamp);
  absl::Time::Breakdown breakdown = time.In(absl::UTCTimeZone());
  EXPECT_EQ(2020, breakdown.year);
  EXPECT_EQ(10, breakdown.month);
  EXPECT_EQ(31, breakdown.day);
  EXPECT_EQ(6, breakdown.hour);
  EXPECT_EQ(29, breakdown.minute);
  EXPECT_EQ(56, breakdown.second);
}

TEST(TimeUtilTest, ToAbseilDurationFromProto) {
  ::google::protobuf::Duration proto_duration;
  proto_duration.set_seconds(123);
  proto_duration.set_nanos(456);
  absl::Duration duration = ToAbseilDuration(proto_duration);
  EXPECT_EQ(123, absl::IDivDuration(duration, absl::Seconds(1), &duration));
  EXPECT_EQ(456, absl::IDivDuration(duration, absl::Nanoseconds(1), &duration));
}

TEST(TimeUtilTest, ToProtoTimestampFromAbseil) {
  absl::Time absl_time = absl::FromUnixSeconds(123) + absl::Nanoseconds(456);
  ::google::protobuf::Timestamp proto_timestamp = ToProtoTimestamp(absl_time);
  EXPECT_EQ(123, proto_timestamp.seconds());
  EXPECT_EQ(456, proto_timestamp.nanos());
}

TEST(TimeUtilTest, ToProtoDurationFromAbseil) {
  absl::Duration absl_duration = absl::Seconds(123) + absl::Nanoseconds(456789);
  ::google::protobuf::Duration proto_duration = ToProtoDuration(absl_duration);
  EXPECT_EQ(123, proto_duration.seconds());
  EXPECT_EQ(456789, proto_duration.nanos());
}

}  // namespace
}  // namespace visionai
