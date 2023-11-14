/*
 * Copyright (c) 2023 Google LLC All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_LOAD_BALANCER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_LOAD_BALANCER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "google/cloud/visionai/v1/health_service.pb.h"

namespace visionai {

using ::google::cloud::visionai::v1::HealthCheckResponse;

// LoadBalancer is the base class of load balancing between LVA clusters.
class LoadBalancer {
 public:
  explicit LoadBalancer(const std::vector<std::string>& locations_ranking);
  virtual ~LoadBalancer() = default;

  // Add a new Cluster into the candidates.
  virtual absl::Status AddCluster
      (const ClusterSelection& cluster_selection) = 0;

  virtual absl::StatusOr<std::vector<ClusterSelection>>
      ListClusterCandidates(const std::string& location) = 0;

  // Check the healthiness and status of the cluster.
  absl::StatusOr<HealthCheckResponse>
    CheckClusterHealth(const ClusterSelection& cluster_selection);

  // Find an available cluster based on specific load balancing mechanism.
  virtual absl::StatusOr<ClusterSelection> FindAvailableCluster() = 0;

 protected:
  std::unordered_map<std::string, std::vector<ClusterSelection>>
      clusters_by_location_;
  std::vector<std::string> locations_ranking_;
};

class RoundRobinLoadBalancer : public LoadBalancer {
 public:
  explicit RoundRobinLoadBalancer
      (const std::vector<std::string>& locations_ranking) :
    LoadBalancer(locations_ranking){}

  absl::Status AddCluster(const ClusterSelection& cluster_selection) override;
  absl::StatusOr<std::vector<ClusterSelection>> ListClusterCandidates(
      const std::string& location) override;
  absl::StatusOr<ClusterSelection> FindAvailableCluster();
  ~RoundRobinLoadBalancer() override = default;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_LOAD_BALANCER_H_
