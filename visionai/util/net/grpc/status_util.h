/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_STATUS_UTIL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_STATUS_UTIL_H_

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "include/grpcpp/grpcpp.h"

namespace visionai {

// Creates an absl::Status from a grpc::Status.
absl::Status ToAbseilStatus(const grpc::Status& status);

// Creates a grpc::Status from an absl::Status.
grpc::Status ToGrpcStatus(const absl::Status& status);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_STATUS_UTIL_H_
