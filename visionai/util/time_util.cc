// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/time_util.h"

#include "visionai/util/time_constants.h"

namespace visionai {
namespace {
namespace protobuf = ::google::protobuf;
}  // namespace

absl::Time ToAbseilTimestamp(const protobuf::Timestamp& proto_timestamp) {
  return absl::FromUnixNanos(proto_timestamp.seconds() * kNanoSecondsInSecond +
                             proto_timestamp.nanos());
}

absl::Duration ToAbseilDuration(const protobuf::Duration& proto_duration) {
  return absl::Seconds(proto_duration.seconds()) +
         absl::Nanoseconds(proto_duration.nanos());
}

protobuf::Timestamp ToProtoTimestamp(const absl::Time& absl_time) {
  protobuf::Timestamp proto_timestamp;
  proto_timestamp.set_seconds(absl::ToUnixSeconds(absl_time));
  proto_timestamp.set_nanos(absl::ToInt64Nanoseconds(
      absl_time - absl::FromUnixSeconds(proto_timestamp.seconds())));
  return proto_timestamp;
}

protobuf::Duration ToProtoDuration(const absl::Duration& absl_duration) {
  protobuf::Duration proto_duration;
  proto_duration.set_seconds(absl_duration / absl::Seconds(1));
  proto_duration.set_nanos(
      (absl_duration - absl::Seconds(proto_duration.seconds())) /
      absl::Nanoseconds(1));
  return proto_duration;
}

}  // namespace visionai
