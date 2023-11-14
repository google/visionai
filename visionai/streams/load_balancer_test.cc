// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/load_balancer.h"

#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "visionai/proto/cluster_selection.pb.h"

namespace visionai {

namespace testing {

namespace {

class LoadBalancerTest : public ::testing::Test {
 protected:
  LoadBalancerTest() {}
};

TEST_F(LoadBalancerTest, AddClusterRoundRobin) {
  std::vector<std::string> locations;
  locations.push_back("location-1");
  locations.push_back("location-2");

  auto lb = RoundRobinLoadBalancer(locations);
  ClusterSelection cluster_selection;
  cluster_selection.set_cluster_id("cluster-1");
  cluster_selection.set_location_id("location-1");
  cluster_selection.set_project_id("project-1");

  auto status = lb.AddCluster(cluster_selection);
  EXPECT_TRUE(status.ok());

  auto candidates = lb.ListClusterCandidates("location-1");
  EXPECT_TRUE(candidates.ok());
  EXPECT_EQ(candidates->size(), 1);
  EXPECT_EQ(candidates->at(0).cluster_id(), cluster_selection.cluster_id());
  EXPECT_EQ(candidates->at(0).location_id(), cluster_selection.location_id());
  EXPECT_EQ(candidates->at(0).project_id(), cluster_selection.project_id());
}

}  // namespace

}  // namespace testing

}  // namespace visionai
