// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/control.h"

#include "google/cloud/visionai/v1/common.pb.h"
#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streams_resources.pb.h"
#include "google/cloud/visionai/v1/streams_service.pb.h"
#include "google/longrunning/operations.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/mock_streams_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/util/time_util.h"

namespace visionai {
namespace testing {
namespace {

using ::google::cloud::visionai::v1::AcquireLeaseRequest;
using ::google::cloud::visionai::v1::Cluster;
using ::google::cloud::visionai::v1::CreateEventRequest;
using ::google::cloud::visionai::v1::CreateSeriesRequest;
using ::google::cloud::visionai::v1::CreateStreamRequest;
using ::google::cloud::visionai::v1::GetClusterRequest;
using ::google::cloud::visionai::v1::GetSeriesRequest;
using ::google::cloud::visionai::v1::GetStreamRequest;
using ::google::cloud::visionai::v1::Lease;
using ::google::cloud::visionai::v1::ReleaseLeaseRequest;
using ::google::cloud::visionai::v1::ReleaseLeaseResponse;
using ::google::cloud::visionai::v1::RenewLeaseRequest;
using ::google::cloud::visionai::v1::Series;
using ::google::cloud::visionai::v1::Stream;
using ::google::longrunning::Operation;
using ::testing::EqualsProto;
using ::testing::Invoke;
using ::testing::StrEq;
using ::testing::status::IsOkAndHolds;

const absl::string_view kTestProjectId = "some-project-id";
const absl::string_view kTestLocationId = "some-location-id";
const absl::string_view kTestClusterId = "some-cluster-id";
const absl::string_view kTestEventId = "some-event-id";
const absl::string_view kTestStreamId = "some-stream-id";
const absl::string_view kTestLeaseId = "some-lease-id";
const absl::string_view kTestLessee = "some-lessee";
const absl::string_view kTestAssetName = "some-asset-name";
const absl::Duration kTestLeaseDuration = absl::Minutes(1);

class ControlTest : public ::testing::Test {
 protected:
  ControlTest()
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

  MockMultipleServicesGrpcServer<MockStreamsService, MockStreamingService>
      grpc_mocker_;
  MockStreamsService* mock_streams_service_;
  MockStreamingService* mock_streaming_service_;
  std::string local_server_address_;
};

TEST_F(ControlTest, BasicStreamsControl) {
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_selection, TestClusterSelection());
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_name, MakeClusterName(cluster_selection));
  VAI_ASSERT_OK_AND_ASSIGN(
      auto channel_id,
      MakeChannelId(std::string(kTestEventId), std::string(kTestStreamId)));
  VAI_ASSERT_OK_AND_ASSIGN(
      auto stream_name,
      MakeStreamName(cluster_name, std::string(kTestStreamId)));
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_name,
                       MakeChannelName(cluster_name, channel_id));
  EXPECT_CALL(*mock_streams_service_, CreateStream)
      .WillOnce(
          Invoke([&](::grpc::ServerContext* context,
                     const CreateStreamRequest* request, Operation* operation) {
            EXPECT_EQ(request->parent(), cluster_name);
            EXPECT_EQ(request->stream_id(), kTestStreamId);
            operation->set_done(true);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streams_service_, GetStream)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const GetStreamRequest* request, Stream* stream) {
        EXPECT_EQ(request->name(), stream_name);
        stream->set_name(stream_name);
        return ::grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_streams_service_, CreateEvent)
      .WillOnce(
          Invoke([&](::grpc::ServerContext* context,
                     const CreateEventRequest* request, Operation* operation) {
            EXPECT_EQ(request->parent(), cluster_name);
            EXPECT_EQ(request->event_id(), kTestEventId);
            operation->set_done(true);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streams_service_, GetSeries)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const GetSeriesRequest* request, Series* series) {
        EXPECT_EQ(request->name(), channel_name);
        return ::grpc::Status(::grpc::StatusCode::NOT_FOUND, "no series");
      }));
  EXPECT_CALL(*mock_streams_service_, CreateSeries)
      .WillOnce(
          Invoke([&](::grpc::ServerContext* context,
                     const CreateSeriesRequest* request, Operation* operation) {
            EXPECT_EQ(request->parent(), cluster_name);
            EXPECT_EQ(request->series_id(), channel_id);
            operation->set_done(true);
            return ::grpc::Status::OK;
          }));

  EXPECT_CALL(*mock_streams_service_, UpdateStream)
      .WillOnce([](grpc::ServerContext* context,
                   const google::cloud::visionai::v1::UpdateStreamRequest* req,
                   Operation* operation) {
        EXPECT_THAT(
            req,
            EqualsProto(
                R"pb(
                  update_mask { paths: "enable_hls_playback" }
                  stream {
                    name: "projects/some-project-id/locations/some-location-id/clusters/some-cluster-id/streams/some-stream-id"
                    enable_hls_playback: true
                  }
                )pb"));
        operation->set_done(true);
        return ::grpc::Status::OK;
      })
      .WillOnce([](grpc::ServerContext* context,
                   const google::cloud::visionai::v1::UpdateStreamRequest* req,
                   Operation* operation) {
        EXPECT_THAT(
            req,
            EqualsProto(
                R"pb(
                  update_mask { paths: "enable_hls_playback" }
                  stream {
                    name: "projects/some-project-id/locations/some-location-id/clusters/some-cluster-id/streams/some-stream-id"
                    enable_hls_playback: false
                  }
                )pb"));
        operation->set_done(true);
        return ::grpc::Status::OK;
      })
      .WillOnce([](grpc::ServerContext* context,
                   const google::cloud::visionai::v1::UpdateStreamRequest* req,
                   Operation* operation) {
        EXPECT_THAT(
            req,
            EqualsProto(
                R"pb(
                  update_mask { paths: "media_warehouse_asset" }
                  stream {
                    name: "projects/some-project-id/locations/some-location-id/clusters/some-cluster-id/streams/some-stream-id"
                    media_warehouse_asset: "some-asset-name"
                  }
                )pb"));
        operation->set_done(true);
        return ::grpc::Status::OK;
      })
      .WillOnce([](grpc::ServerContext* context,
                   const google::cloud::visionai::v1::UpdateStreamRequest* req,
                   Operation* operation) {
        EXPECT_THAT(
            req,
            EqualsProto(
                R"pb(
                  update_mask { paths: "media_warehouse_asset" }
                  stream {
                    name: "projects/some-project-id/locations/some-location-id/clusters/some-cluster-id/streams/some-stream-id"
                    media_warehouse_asset: ""
                  }
                )pb"));
        operation->set_done(true);
        return ::grpc::Status::OK;
      });

  EXPECT_TRUE(CreateStream(cluster_selection, std::string(kTestStreamId)).ok());
  EXPECT_TRUE(CheckStreamExists(cluster_selection, std::string(kTestStreamId)).ok());
  EXPECT_TRUE(CreateEvent(cluster_selection, std::string(kTestEventId)).ok());
  EXPECT_OK(Bind(cluster_selection, std::string(kTestEventId),
                 std::string(kTestStreamId)));
  EXPECT_TRUE(EnableHlsPlayback(cluster_selection, std::string(kTestStreamId)).ok());
  EXPECT_TRUE(DisableHlsPlayback(cluster_selection, std::string(kTestStreamId)).ok());
  EXPECT_OK(EnableMwhExporter(cluster_selection, std::string(kTestStreamId),
                              std::string(kTestAssetName)));
  EXPECT_TRUE(DisableMwhExporter(cluster_selection, std::string(kTestStreamId)).ok());
}

TEST_F(ControlTest, BasicClusterControl) {
  {
    VAI_ASSERT_OK_AND_ASSIGN(auto cluster_selection, TestClusterSelection());
    VAI_ASSERT_OK_AND_ASSIGN(auto cluster_name, MakeClusterName(cluster_selection));
    EXPECT_CALL(*mock_streams_service_, GetCluster)
        .WillOnce(
            Invoke([&](::grpc::ServerContext* context,
                       const GetClusterRequest* request, Cluster* cluster) {
              EXPECT_EQ(request->name(), cluster_name);
              cluster->set_dataplane_service_endpoint(local_server_address_);
              return ::grpc::Status::OK;
            }));

    auto endpoint_statusor = GetClusterEndpoint(cluster_selection);
    EXPECT_TRUE(endpoint_statusor.ok());
    EXPECT_EQ(*endpoint_statusor, local_server_address_);
  }
  {
    ClusterSelection cluster_selection;
    cluster_selection.set_cluster_endpoint(local_server_address_);
    EXPECT_THAT(GetClusterEndpoint(cluster_selection),
                IsOkAndHolds(StrEq(local_server_address_)));
  }
}

TEST_F(ControlTest, BasicLeaseControl) {
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_selection, TestClusterSelection());
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_name, MakeClusterName(cluster_selection));
  VAI_ASSERT_OK_AND_ASSIGN(
      auto channel_id,
      MakeChannelId(std::string(kTestEventId), std::string(kTestStreamId)));

  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, AcquireLease)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const AcquireLeaseRequest* request, Lease* lease) {
        lease->set_id(kTestLeaseId);
        lease->set_series(channel_id);
        lease->set_owner(kTestLessee);
        *lease->mutable_expire_time() =
            ToProtoTimestamp(absl::Now() + kTestLeaseDuration);
        return ::grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_streaming_service_, RenewLease)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const RenewLeaseRequest* request, Lease* lease) {
        lease->set_id(request->id());
        lease->set_series(request->series());
        lease->set_owner(request->owner());
        *lease->mutable_expire_time() =
            ToProtoTimestamp(absl::Now() + ToAbseilDuration(request->term()));
        return ::grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_streaming_service_, ReleaseLease)
      .WillOnce(Invoke(
          [&](::grpc::ServerContext* context,
              const ReleaseLeaseRequest* request,
              ReleaseLeaseResponse* response) { return ::grpc::Status::OK; }));

  LeaseOptions options;
  options.lessee = kTestLessee;
  options.duration = kTestLeaseDuration;
  Channel channel;
  channel.event_id = kTestEventId;
  channel.stream_id = kTestStreamId;
  options.channel = channel;
  options.lease_type = ChannelLeaseType::kWriters;

  auto lease_statusor = AcquireChannelLease(cluster_selection, options);
  EXPECT_TRUE(lease_statusor.ok());

  auto lease = std::move(*lease_statusor);
  EXPECT_EQ(lease.id, kTestLeaseId);
  EXPECT_EQ(lease.channel.event_id, kTestEventId);
  EXPECT_EQ(lease.channel.stream_id, kTestStreamId);
  EXPECT_EQ(lease.lessee, kTestLessee);
  EXPECT_EQ(lease.duration, kTestLeaseDuration);
  EXPECT_EQ(lease.lease_type, ChannelLeaseType::kWriters);

  EXPECT_TRUE(RenewLease(cluster_selection, &lease).ok());
  EXPECT_EQ(lease.id, kTestLeaseId);
  EXPECT_EQ(lease.channel.event_id, kTestEventId);
  EXPECT_EQ(lease.channel.stream_id, kTestStreamId);
  EXPECT_EQ(lease.lessee, kTestLessee);
  EXPECT_EQ(lease.duration, kTestLeaseDuration);
  EXPECT_EQ(lease.lease_type, ChannelLeaseType::kWriters);

  EXPECT_TRUE(ReleaseLease(cluster_selection, &lease).ok());
  EXPECT_EQ(lease.duration, absl::ZeroDuration());
}

}  // namespace

}  // namespace testing
}  // namespace visionai
