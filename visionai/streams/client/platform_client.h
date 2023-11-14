/*
 * Copyright (c) 2023 Google LLC All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PLATFORM_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PLATFORM_CLIENT_H_

#include <memory>
#include <vector>

#include "google/cloud/visionai/v1/platform.grpc.pb.h"
#include "google/cloud/visionai/v1/platform.pb.h"
#include "google/longrunning/operations.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/lro/operations_client.h"
#include "visionai/util/net/exponential_backoff.h"

namespace visionai {

// Client for visionai platform service to manage the application resources.
class PlatformClient {
 public:
  // Options to control the platform client.
  struct Options {
    // The deadline for LRO to complete.
    absl::Duration deadline = absl::Minutes(5);

    // Advanced config for polling LRO with exponential backoff.
    absl::Duration initial_wait_time = absl::Seconds(1);
    absl::Duration max_wait_time = absl::Seconds(30);
    float wait_time_multiplier = 2.0f;

    // Whether to use insecure channel.
    // Only flip it for unit tests.
    bool use_insecure_channel = false;
  };

  // Create an instance of PlatformClient with default settings.
  static absl::StatusOr<std::unique_ptr<PlatformClient>> Create(
      const std::string &service_address);

  // Create an instance of PlatformClient with customized options.
  static absl::StatusOr<std::unique_ptr<PlatformClient>> Create(
      const std::string &service_address, const Options &options,
      const std::shared_ptr<OperationsClient> &operations_client);

  // Synchronous call to add stream input to an application.
  absl::Status AddStreamToApplication(
      const resource_ids::Stream &stream,
      const resource_ids::Application &application);

  // Synchronous call to remove stream input from an application.
  absl::Status RemoveStreamFromApplication(
      const resource_ids::Stream &stream,
      const resource_ids::Application &application);

 private:
  explicit PlatformClient(const Options &options) : options_(options){};

  Options options_;
  ConnectionOptions connection_options_;
  std::shared_ptr<::grpc::Channel> channel_;
  std::shared_ptr<OperationsClient> operations_client_;
};
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PLATFORM_CLIENT_H_