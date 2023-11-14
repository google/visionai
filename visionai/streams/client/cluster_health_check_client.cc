// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/cluster_health_check_client.h"

#include <memory>
#include <string>
#include <utility>

#include "google/cloud/visionai/v1/health_service.grpc.pb.h"
#include "google/cloud/visionai/v1/health_service.pb.h"
#include "absl/memory/memory.h"
#include "absl/status/statusor.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

absl::StatusOr<std::unique_ptr<ClusterHealthCheckClient>>
ClusterHealthCheckClient::Create(const Options &options) {
  auto client = absl::WrapUnique(new ClusterHealthCheckClient());
  client->options_ = options;
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      client->options_.target_address, client->options_.connection_options))
      << "while configuring authorization information";
  auto channel = CreateChannel(client->options_.target_address,
                               client->options_.connection_options);

  client->health_stub_ =
      google::cloud::visionai::v1::HealthCheckService::NewStub(channel);
  return std::move(client);
}

absl::StatusOr<google::cloud::visionai::v1::HealthCheckResponse>
    ClusterHealthCheckClient::CheckClusterHealth(
        const std::string& cluster_name) {
  auto context = CreateClientContext(options_.connection_options);
  google::cloud::visionai::v1::HealthCheckRequest request;
  request.set_cluster(cluster_name);
  google::cloud::visionai::v1::HealthCheckResponse response;
  VAI_RETURN_IF_ERROR(health_stub_->HealthCheck(context.get(), request, &response));
  return response;
}

}  // namespace visionai
