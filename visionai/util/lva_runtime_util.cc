// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/lva_runtime_util.h"

#include "absl/time/time.h"

namespace visionai {

namespace {
constexpr char kLVARuntimeInputTimstamp[] = "LVARuntimeInputTimestamp";
}  // namespace

void SetLVARuntimeInputTimstamp(Packet* p) {
  google::protobuf::Value value;
  std::string input_time =
      absl::FormatTime(absl::RFC1123_full, absl::Now(), absl::UTCTimeZone());
  value.set_string_value(input_time);
  // Update packet metadata field to reflect
  // the timestamp when packets enters the lva runtime.
  auto set_st = SetMetadataField(kLVARuntimeInputTimstamp, value, p);
  if (!set_st.ok()) {
    LOG(WARNING) << "Failed to set the input timestamp: " << set_st;
  }
}

absl::StatusOr<absl::Time> GetLVARuntimeInputTimestamp(
    const Packet& p) {
  auto get_val = GetMetadataField(kLVARuntimeInputTimstamp, p);
  std::string input_time_err;
  absl::Time t;
  return get_val.ok()
             ? (absl::ParseTime(absl::RFC1123_full,
                                get_val.value().string_value(),
                                absl::UTCTimeZone(), &t, &input_time_err),
                absl::StatusOr<absl::Time>(t))
             : absl::InternalError("Failed to get the input timestamp");
}

void RemoveLVARuntimeInputTimestamp(Packet* p) {
  auto remove_st = RemoveMetadataField(kLVARuntimeInputTimstamp, p);
  if (!remove_st.ok()) {
    LOG(WARNING) << "Failed to remove the input timestamp: " << remove_st;
  }
}

}  // namespace visionai
