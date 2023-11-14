// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/load_balancer.h"

#include <string>
#include <vector>

#include "visionai/proto/cluster_selection.pb.h"
#include "absl/status/statusor.h"
#include "absl/status/status.h"
#include "visionai/util/status/status_macros.h"
#include "visionai/streams/client/control.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/streams/client/cluster_health_check_client.h"

namespace visionai {

LoadBalancer::LoadBalancer(const std::vector<std::string>& locations_ranking)
  : locations_ranking_(locations_ranking) {}

absl::Status RoundRobinLoadBalancer::AddCluster
    (const ClusterSelection& cluster_selection) {
  clusters_by_location_[cluster_selection.location_id()]
      .push_back(cluster_selection);
  return absl::OkStatus();
}

absl::StatusOr<std::vector<ClusterSelection>>
RoundRobinLoadBalancer::ListClusterCandidates(const std::string& location) {
  return clusters_by_location_[location];
}

absl::StatusOr<HealthCheckResponse> LoadBalancer::CheckClusterHealth
    (const ClusterSelection& cluster_selection) {
  VAI_ASSIGN_OR_RETURN(const auto endpoint,
                         GetClusterEndpoint(cluster_selection));
  ClusterHealthCheckClient::Options clOptions;
  clOptions.target_address = endpoint;
  clOptions.connection_options = DefaultConnectionOptions();
  clOptions.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(
      cluster_selection.use_insecure_channel());
  VAI_ASSIGN_OR_RETURN(auto healthClient,
                  ClusterHealthCheckClient::Create(clOptions));
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(cluster_selection));
  VAI_ASSIGN_OR_RETURN(auto response,
              healthClient->CheckClusterHealth(cluster_name));
  return response;
}

// Find the first available cluster from the candidates.
// Queries are based on the location ranking, and the sequence of the cluster
// candidate list of each location.
absl::StatusOr<ClusterSelection>
  RoundRobinLoadBalancer::FindAvailableCluster() {
  for (const auto& location : locations_ranking_) {
    auto iter = clusters_by_location_.find(location);
    if (iter != clusters_by_location_.end()) {
      for (const auto& cluster_selection : iter->second) {
        auto check_status = CheckClusterHealth(cluster_selection);
        if (check_status.ok() && check_status.value().healthy()) {
          return cluster_selection;
        }
      }
    }
  }
  return absl::NotFoundError("no healthy cluster is found");
}

}  // namespace visionai
