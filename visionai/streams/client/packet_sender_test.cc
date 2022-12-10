// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/packet_sender.h"

#include "glog/logging.h"
#include "google/cloud/visionai/v1/common.pb.h"
#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streams_service.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/mock_streams_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace testing {
namespace {

using ::google::cloud::visionai::v1::Cluster;
using ::google::cloud::visionai::v1::GetClusterRequest;
using ::google::cloud::visionai::v1::SendPacketsRequest;
using ::google::cloud::visionai::v1::SendPacketsResponse;
using ::testing::Invoke;

constexpr char kTestProjectId[] = "some-project-id";
constexpr char kTestLocationId[] = "some-location-id";
constexpr char kTestClusterId[] = "some-cluster-id";
constexpr char kTestEventId[] = "some-event-id";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestSender[] = "some-sender";

class PacketSenderTest : public ::testing::Test {
 protected:
  PacketSenderTest()
      : grpc_mocker_(),
        mock_streams_service_(grpc_mocker_.service<MockStreamsService>()),
        mock_streaming_service_(grpc_mocker_.service<MockStreamingService>()),
        local_server_address_(grpc_mocker_.local_credentials_server_address()) {
  }

  absl::StatusOr<ClusterSelection> TestClusterSelection() {
    ClusterSelection selection;
    selection.set_service_endpoint(local_server_address_);
    selection.set_project_id(kTestProjectId);
    selection.set_location_id(kTestLocationId);
    selection.set_cluster_id(kTestClusterId);
    selection.set_use_insecure_channel(true);
    return selection;
  }

  absl::Status TestPacketSenderOptions(PacketSender::Options* options) {
    VAI_ASSIGN_OR_RETURN(options->cluster_selection, TestClusterSelection());
    options->channel.event_id = kTestEventId;
    options->channel.stream_id = kTestStreamId;
    options->sender = kTestSender;
    return absl::OkStatus();
  }


  MockMultipleServicesGrpcServer<MockStreamsService, MockStreamingService>
      grpc_mocker_;
  MockStreamsService* mock_streams_service_;
  MockStreamingService* mock_streaming_service_;
  std::string local_server_address_;

  const std::string cluster_name_ =
      absl::StrFormat("projects/%s/locations/%s/clusters/%s", kTestProjectId,
                      kTestLocationId, kTestClusterId);
};

TEST_F(PacketSenderTest, Basic) {
  int iterations = 5;

  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId(kTestEventId, kTestStreamId));
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, SendPackets)
      .WillOnce(Invoke(
          [&](grpc::ServerContext* context,
              grpc::ServerReaderWriter<SendPacketsResponse, SendPacketsRequest>*
                  stream) {
            SendPacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_metadata());
            EXPECT_EQ(req.metadata().event(),
                      MakeEventName(cluster_name_, kTestEventId).value());
            EXPECT_EQ(req.metadata().stream(),
                      MakeStreamName(cluster_name_, kTestStreamId).value());
            EXPECT_EQ(req.metadata().series(),
                      MakeChannelName(cluster_name_, channel_id).value());
            EXPECT_EQ(req.metadata().owner(), kTestSender);
            for (int i = 0; i < iterations; ++i) {
              EXPECT_TRUE(stream->Read(&req));
              EXPECT_TRUE(req.has_packet());
              auto s = PacketAs<std::string>(std::move(*req.mutable_packet()));
              EXPECT_TRUE(s.ok());
              EXPECT_EQ(*s, "hello!");
            }
            return grpc::Status::OK;
          }));

  PacketSender::Options options;
  EXPECT_TRUE(TestPacketSenderOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_sender, PacketSender::Create(options));
  auto p = MakePacket(std::string("hello!"));
  EXPECT_TRUE(p.ok());
  for (int i = 0; i < iterations; ++i) {
    auto status = packet_sender->Send(*p, absl::Seconds(5));
    EXPECT_TRUE(status.ok());
  }
}

}  // namespace
}  // namespace testing
}  // namespace visionai
