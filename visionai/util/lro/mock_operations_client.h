/*
 * Copyright (c) 2023 Google LLC All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_LRO_MOCK_OPERATIONS_CLIENT_H_
#define THIRD_PARTY_VISIONAI_UTIL_LRO_MOCK_OPERATIONS_CLIENT_H_

#include "gmock/gmock.h"
#include "visionai/util/lro/operations_client.h"

namespace visionai {

class MockOperationsClient : public OperationsClient {
 public:
  explicit MockOperationsClient() : OperationsClient(nullptr) {}
  ~MockOperationsClient() override = default;

  MOCK_METHOD(
      absl::StatusOr<google::protobuf::Any>, PollOperation,
      (const std::string& operation_name,
       const ConnectionOptions::ClientContextOptions& client_context_options,
       ExponentialBackoff& exponential_backoff, absl::Duration deadline),
      (override));
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_LRO_MOCK_OPERATIONS_CLIENT_H_
