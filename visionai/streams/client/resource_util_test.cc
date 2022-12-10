// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/resource_util.h"

#include "glog/logging.h"
#include "gtest/gtest.h"

namespace visionai {

TEST(ResourceUtilTest, MakeProjectLocationName) {
  {
    std::string project_id = "p-1";
    std::string location_id = "l-1";
    auto pl_name = MakeProjectLocationName(project_id, location_id);
    EXPECT_TRUE(pl_name.ok());
    EXPECT_EQ(*pl_name, "projects/p-1/locations/l-1");
  }
}

TEST(ResourceUtilTest, MakeClusterName) {
  {
    std::string project_id = "p-1";
    std::string location_id = "l-1";
    std::string cluster_id = "c1";
    auto cluster_name = MakeClusterName(project_id, location_id, cluster_id);
    EXPECT_TRUE(cluster_name.ok());
    EXPECT_EQ(*cluster_name, "projects/p-1/locations/l-1/clusters/c1");
  }
  {
    std::string project_id = "1";
    std::string location_id = "l-1";
    std::string cluster_id = "c1";
    auto cluster_name = MakeClusterName(project_id, location_id, cluster_id);
    EXPECT_FALSE(cluster_name.ok());
  }
  {
    std::string project_id = "1";
    std::string location_id = "l-1";
    std::string cluster_id = "";
    auto cluster_name = MakeClusterName(project_id, location_id, cluster_id);
    EXPECT_FALSE(cluster_name.ok());
  }
}

TEST(ResourceUtilTest, MakeEventName) {
  {
    std::string cluster_name = "projects/p-1/locations/l-1/clusters/c-1";
    std::string event_id = "e-1";
    auto name = MakeEventName(cluster_name, event_id);
    EXPECT_TRUE(name.ok());
    EXPECT_EQ(*name, "projects/p-1/locations/l-1/clusters/c-1/events/e-1");
  }
}

TEST(ResourceUtilTest, MakeStreamName) {
  {
    std::string cluster_name = "projects/p-1/locations/l-1/clusters/c-1";
    std::string stream_id = "s-1";
    auto name = MakeStreamName(cluster_name, stream_id);
    EXPECT_TRUE(name.ok());
    EXPECT_EQ(*name, "projects/p-1/locations/l-1/clusters/c-1/streams/s-1");
  }
}

TEST(ResourceUtilTest, MakeChannelName) {
  {
    std::string cluster_name = "projects/p-1/locations/l-1/clusters/c-1";
    std::string channel_id = "s-1";
    auto name = MakeChannelName(cluster_name, channel_id);
    EXPECT_TRUE(name.ok());
    EXPECT_EQ(*name, "projects/p-1/locations/l-1/clusters/c-1/series/s-1");
  }
}

TEST(ResourceUtilTest, ParseClusterName) {
  {
    std::string cluster_name = "projects/p-1/locations/l-1/clusters/c-1";
    auto resource_ids = ParseClusterName(cluster_name);
    EXPECT_TRUE(resource_ids.ok());
    EXPECT_EQ(resource_ids->size(), 3);
    EXPECT_EQ((*resource_ids)[0], "p-1");
    EXPECT_EQ((*resource_ids)[1], "l-1");
    EXPECT_EQ((*resource_ids)[2], "c-1");
  }
  {
    std::string cluster_name =
        "projects/p-1/locations/l-1/clusters/c-1/streams/s-1";
    auto resource_ids = ParseClusterName(cluster_name);
    EXPECT_FALSE(resource_ids.ok());
  }
  {
    std::string cluster_name = "c-1";
    auto resource_ids = ParseClusterName(cluster_name);
    EXPECT_FALSE(resource_ids.ok());
  }
}

TEST(ResourceUtilTest, ParseStreamName) {
  {
    std::string stream_name =
        "projects/p-1/locations/l-1/clusters/c-1/streams/s-1";
    auto resource_ids = ParseStreamName(stream_name);
    EXPECT_TRUE(resource_ids.ok());
    EXPECT_EQ(resource_ids->size(), 4);
    EXPECT_EQ((*resource_ids)[0], "p-1");
    EXPECT_EQ((*resource_ids)[1], "l-1");
    EXPECT_EQ((*resource_ids)[2], "c-1");
    EXPECT_EQ((*resource_ids)[3], "s-1");
  }
  {
    std::string stream_name = "projects/p-1/locations/l-1/clusters/c-1";
    auto resource_ids = ParseStreamName(stream_name);
    EXPECT_FALSE(resource_ids.ok());
  }
  {
    std::string stream_name = "c-1";
    auto resource_ids = ParseStreamName(stream_name);
    EXPECT_FALSE(resource_ids.ok());
  }
}

TEST(ResourceUtilTest, GetClusterName) {
  {
    std::string resource_name =
        "projects/test-project/locations/test-location/clusters/test-cluster/"
        "streams/test-stream";
    auto cluster_name = GetClusterName(resource_name);
    EXPECT_TRUE(cluster_name.ok());
    EXPECT_EQ(
        *cluster_name,
        "projects/test-project/locations/test-location/clusters/test-cluster");
  }
  {
    std::string resource_name =
        "projects/test-project/locations/test-location/clusters/test-cluster/"
        "events/test-event";
    auto cluster_name = GetClusterName(resource_name);
    EXPECT_TRUE(cluster_name.ok());
    EXPECT_EQ(
        *cluster_name,
        "projects/test-project/locations/test-location/clusters/test-cluster");
  }
  {
    std::string resource_name =
        "projects/test-project/locations/test-location/clusters/test-cluster/"
        "series/test-series";
    auto cluster_name = GetClusterName(resource_name);
    EXPECT_TRUE(cluster_name.ok());
    EXPECT_EQ(
        *cluster_name,
        "projects/test-project/locations/test-location/clusters/test-cluster");
  }
  {
    std::string resource_name =
        "projects/test-project/locations/test-location/clusters/test-cluster";
    auto cluster_name = GetClusterName(resource_name);
    LOG(INFO) << cluster_name.status();
    EXPECT_FALSE(cluster_name.ok());
  }
}

TEST(ResourceUtilTest, InSameCluster) {
  {
    std::string name1 =
        "projects/test-project/locations/test-location/clusters/test-cluster/"
        "streams/test-stream";
    std::string name2 =
        "projects/test-project/locations/test-location/clusters/test-cluster/"
        "events/test-event";
    auto is_same = InSameCluster(name1, name2);
    EXPECT_TRUE(is_same.ok());
    EXPECT_TRUE(*is_same);
  }
  {
    std::string name1 =
        "projects/test-project/locations/test-location/clusters/test-cluster1/"
        "streams/test-stream";
    std::string name2 =
        "projects/test-project/locations/test-location/clusters/test-cluster2/"
        "events/test-event";
    auto is_same = InSameCluster(name1, name2);
    EXPECT_TRUE(is_same.ok());
    EXPECT_FALSE(*is_same);
  }
}

TEST(ResourceUtilTest, MakeChannelId) {
  {
    std::string event_id = "test-event";
    std::string stream_id = "test-stream";
    auto channel_id = MakeChannelId(event_id, stream_id);
    EXPECT_TRUE(channel_id.ok());
    EXPECT_EQ(*channel_id, "test-event--test-stream");
  }
  LOG(INFO) << NewEventId();
}

TEST(ResourceUtilTest, ResourceParendAndId) {
  {
    std::string resource_name =
        "projects/test-project/locations/test-location/clusters/test-cluster/"
        "streams/test-stream";

    auto resource_id = ResourceId(resource_name);
    EXPECT_TRUE(resource_id.ok());
    EXPECT_EQ(*resource_id, "test-stream");

    auto resource_parent = ResourceParentName(resource_name);
    EXPECT_TRUE(resource_parent.ok());
    EXPECT_EQ(
        *resource_parent,
        "projects/test-project/locations/test-location/clusters/test-cluster");
  }

  {
    std::string resource_name =
        "projects/test-project/locations/test-location/clusters/test-cluster";

    auto resource_id = ResourceId(resource_name);
    EXPECT_TRUE(resource_id.ok());
    EXPECT_EQ(*resource_id, "test-cluster");

    auto resource_parent = ResourceParentName(resource_name);
    EXPECT_TRUE(resource_parent.ok());
    EXPECT_EQ(*resource_parent,
              "projects/test-project/locations/test-location");
  }

  {
    std::string resource_name = "test-stream";

    auto resource_id = ResourceId(resource_name);
    EXPECT_TRUE(resource_id.ok());
    EXPECT_EQ(*resource_id, "test-stream");

    auto resource_parent = ResourceParentName(resource_name);
    EXPECT_FALSE(resource_parent.ok());
  }

  {
    std::string resource_name = "//// /test-stream";

    auto resource_id = ResourceId(resource_name);
    EXPECT_TRUE(resource_id.ok());
    EXPECT_EQ(*resource_id, "test-stream");

    auto resource_parent = ResourceParentName(resource_name);
    EXPECT_FALSE(resource_parent.ok());
  }
}

}  // namespace visionai
