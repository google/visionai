// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_LVA_RUNTIME_UTIL_H_
#define THIRD_PARTY_VISIONAI_UTIL_LVA_RUNTIME_UTIL_H_

#include "absl/status/statusor.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {

void SetLVARuntimeInputTimstamp(Packet* packet);

absl::StatusOr<absl::Time> GetLVARuntimeInputTimestamp(
    const Packet& packet);

void RemoveLVARuntimeInputTimestamp(Packet* packet);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_LVA_RUNTIME_UTIL_H_
