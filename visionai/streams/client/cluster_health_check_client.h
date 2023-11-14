// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CLUSTER_HEALTH_CHECK_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CLUSTER_HEALTH_CHECK_CLIENT_H_

#include "google/cloud/visionai/v1/health_service.grpc.pb.h"
#include "absl/status/statusor.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"

namespace visionai {

// ClusterHealthCheckClient is the gRPC client talks to the HealthCheckService.
// It provides methods checking healthiness status of Vision AI clusters.
class ClusterHealthCheckClient {
 public:
  struct Options {
    std::string target_address;
    ConnectionOptions connection_options;
  };

  static absl::StatusOr<std::unique_ptr<ClusterHealthCheckClient>> Create(
      const Options& options);

  absl::StatusOr<google::cloud::visionai::v1::HealthCheckResponse>
      CheckClusterHealth(const std::string& cluster_name);

  ClusterHealthCheckClient(const ClusterHealthCheckClient&) = delete;
  ClusterHealthCheckClient& operator=(const ClusterHealthCheckClient&) = delete;
  virtual ~ClusterHealthCheckClient() = default;

 private:
  ClusterHealthCheckClient() = default;
  Options options_;
  std::unique_ptr<google::cloud::visionai::v1::HealthCheckService::Stub>
      health_stub_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CLUSTER_HEALTH_CHECK_CLIENT_H_
