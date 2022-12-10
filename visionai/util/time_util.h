// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_UTIL_TIME_UTIL_H_
#define VISIONAI_UTIL_TIME_UTIL_H_

#include "google/protobuf/duration.pb.h"
#include "google/protobuf/timestamp.pb.h"
#include "absl/time/time.h"

namespace visionai {

absl::Time ToAbseilTimestamp(
    const ::google::protobuf::Timestamp& proto_timestamp);

absl::Duration ToAbseilDuration(
    const ::google::protobuf::Duration& proto_duration);

::google::protobuf::Timestamp ToProtoTimestamp(const ::absl::Time& absl_time);

::google::protobuf::Duration ToProtoDuration(
    const ::absl::Duration& absl_duration);

}  // namespace visionai

#endif  // VISIONAI_UTIL_TIME_UTIL_H_
