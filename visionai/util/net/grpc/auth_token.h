/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_AUTH_TOKEN_H_
#define THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_AUTH_TOKEN_H_

#include <string>

#include "absl/status/statusor.h"

namespace visionai {

// GetJwt generates the JWT from json secret with audience.
absl::StatusOr<std::string> GetJwt(const std::string& json_secret,
                                   const std::string& audience);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_AUTH_TOKEN_H_
