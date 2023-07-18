// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/packet_receiver.h"

#include "glog/logging.h"
#include "google/cloud/visionai/v1/common.pb.h"
#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streams_service.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/mock_streams_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace testing {
namespace {

using ::google::cloud::visionai::v1::Cluster;
using ::google::cloud::visionai::v1::GetClusterRequest;
using ::google::cloud::visionai::v1::ReceivePacketsRequest;
using ::google::cloud::visionai::v1::ReceivePacketsResponse;
using ::testing::EqualsProto;
using ::testing::HasSubstr;
using ::testing::Invoke;

constexpr char kTestProjectId[] = "some-project-id";
constexpr char kTestLocationId[] = "some-location-id";
constexpr char kTestClusterId[] = "some-cluster-id";
constexpr char kTestEventId[] = "some-event-id";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestLeaseId[] = "some-lease-id";
constexpr char kTestLessee[] = "some-lessee";
constexpr char kTestPacketPayload[] = "hello!";
constexpr absl::Duration kTestLeaseDuration = absl::Minutes(1);
constexpr absl::Duration kTestLeaseTerm = absl::Minutes(1);
constexpr absl::Duration kTestWritesDoneGracePeriod = absl::Seconds(10);

class PacketReceiverTest : public ::testing::Test {
 protected:
  PacketReceiverTest()
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

  absl::Status TestPacketReceiverOptions(PacketReceiver::Options* options) {
    VAI_ASSIGN_OR_RETURN(options->cluster_selection, TestClusterSelection());
    options->channel.event_id = kTestEventId;
    options->channel.stream_id = kTestStreamId;
    options->lessee = kTestLessee;
    options->advanced.grace_period = kTestLeaseTerm;
    return absl::OkStatus();
  }

  std::string TestChannelId() {
    return MakeChannelId(kTestEventId, kTestStreamId).value();
  }

  absl::Status CheckSetupRequest(const ReceivePacketsRequest& request) {
    EXPECT_TRUE(request.has_setup_request());
    EXPECT_EQ(request.setup_request().metadata().event(),
              MakeEventName(cluster_name_, kTestEventId).value());
    EXPECT_EQ(request.setup_request().metadata().stream(),
              MakeStreamName(cluster_name_, kTestStreamId).value());
    EXPECT_EQ(request.setup_request().metadata().series(),
              MakeChannelName(cluster_name_, TestChannelId()).value());
    EXPECT_EQ(request.setup_request().metadata().owner(), kTestLessee);
    EXPECT_EQ(request.setup_request().receiver(), kTestLessee);
    EXPECT_THAT(request.setup_request().metadata().lease_term(),
                EqualsProto(ToProtoDuration(kTestLeaseTerm)));
    return absl::OkStatus();
  }

  absl::Status CheckSetupRequestV1(
      const ::google::cloud::visionai::v1::ReceivePacketsRequest& request) {
    EXPECT_TRUE(request.has_setup_request());
    EXPECT_EQ(request.setup_request().metadata().event(),
              MakeEventName(cluster_name_, kTestEventId).value());
    EXPECT_EQ(request.setup_request().metadata().stream(),
              MakeStreamName(cluster_name_, kTestStreamId).value());
    EXPECT_EQ(request.setup_request().metadata().series(),
              MakeChannelName(cluster_name_, TestChannelId()).value());
    EXPECT_EQ(request.setup_request().metadata().owner(), kTestLessee);
    EXPECT_EQ(request.setup_request().receiver(), kTestLessee);
    EXPECT_THAT(request.setup_request().metadata().lease_term(),
                EqualsProto(ToProtoDuration(kTestLeaseTerm)));
    return absl::OkStatus();
  }

  absl::Status TestPacket(int64_t offset, Packet* p) {
    VAI_ASSIGN_OR_RETURN(*p, MakePacket(std::string(kTestPacketPayload)));
    VAI_RETURN_IF_ERROR(SetOffset(offset, p));
    return absl::OkStatus();
  }


  absl::Status TestPacketResponse(int64_t offset,
                                  ReceivePacketsResponse* response) {
    EXPECT_TRUE(TestPacket(offset, response->mutable_packet()).ok());
    return absl::OkStatus();
  }

  absl::Status TestHeartbeatResponse(ReceivePacketsResponse* response) {
    response->mutable_control()->set_heartbeat(true);
    return absl::OkStatus();
  }

  absl::Status TestWriteDoneRequestResponse(ReceivePacketsResponse* response) {
    response->mutable_control()->set_writes_done_request(true);
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

TEST_F(PacketReceiverTest, BasicInvalidOptions) {
  {
    PacketReceiver::Options options;
    EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
    options.receive_mode = "controlled";
    options.lessee = "";
    auto packet_receiver = PacketReceiver::Create(options);
    EXPECT_FALSE(packet_receiver.ok());
  }
  {
    PacketReceiver::Options options;
    EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
    options.receive_mode = "controlled";
    options.cluster_selection.set_cluster_id("");
    auto packet_receiver = PacketReceiver::Create(options);
    EXPECT_FALSE(packet_receiver.ok());
  }
}

TEST_F(PacketReceiverTest, GetClusterRejection) {
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            return absl::UnknownError("Intentaionl GetCluster fail");
          }));
  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  auto packet_receiver = PacketReceiver::Create(options);
  EXPECT_THAT(packet_receiver.status(),
              StatusIs(util::error::UNKNOWN, HasSubstr("GetCluster fail")));
}

TEST_F(PacketReceiverTest, ConstructionSuccess) {
  ON_CALL(*mock_streams_service_, GetCluster)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  ON_CALL(*mock_streaming_service_, ReceivePackets)
      .WillByDefault(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            stream->Read(&req);

            return grpc::Status(grpc::StatusCode::UNKNOWN,
                                "Sentinel error return");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));
}

TEST_F(PacketReceiverTest, SetupRequestRejection) {
  ON_CALL(*mock_streams_service_, GetCluster)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            return absl::UnknownError("Intentional Setup rejection");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Close read channel.
  Packet p;
  bool ok;
  EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
  EXPECT_FALSE(ok);

  // Close write channel.
  packet_receiver->CommitsDone();

  // Get result.
  EXPECT_THAT(
      packet_receiver->Finish(),
      StatusIs(grpc::StatusCode::UNKNOWN, HasSubstr("Setup rejection")));
}

TEST_F(PacketReceiverTest, ControlledReadOnlyCommonCase) {
  int num_packets = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            EXPECT_TRUE(TestWriteDoneRequestResponse(&resp).ok());
            EXPECT_TRUE(stream->Write(resp));

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read a few packets.
  Packet p;
  bool ok;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
    EXPECT_TRUE(ok);
  }

  // Close read channel.
  EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
  EXPECT_FALSE(ok);

  // Close write channel.
  packet_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsOutOfRange(packet_receiver->Finish()));
}

TEST_F(PacketReceiverTest, ServerCancelTest) {
  ON_CALL(*mock_streams_service_, GetCluster)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  absl::Notification server_cancelled;
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Server cancels.
            context->TryCancel();
            server_cancelled.Notify();

            return absl::UnknownError("Sentinel return");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // This test server does not write.
  // Read should timeout before cancellation.
  Packet packet;
  bool read_ok = true;
  EXPECT_FALSE(
      packet_receiver->Receive(absl::ZeroDuration(), &packet, &read_ok));

  server_cancelled.WaitForNotification();

  // Read should fail after cancelled.
  EXPECT_TRUE(
      packet_receiver->Receive(absl::InfiniteDuration(), &packet, &read_ok));
  EXPECT_FALSE(read_ok);

  // Attempt to commit. This eventually fails as the committer will realize the
  // server has closed.
  bool write_ok = true;
  while (write_ok) {
    EXPECT_TRUE(
        packet_receiver->Commit(absl::InfiniteDuration(), 42, &write_ok));
    absl::SleepFor(absl::Milliseconds(100));
  }

  // Close write channel.
  packet_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsCancelled(packet_receiver->Finish()));
}

TEST_F(PacketReceiverTest, ClientCancelTest) {
  absl::Notification server_done;
  ON_CALL(*mock_streams_service_, GetCluster)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
            });

            // Try to send. Client will eventually cancel.
            // No point requesting a writes done afterwards.
            ReceivePacketsResponse resp;
            while (stream->Write(resp)) {
              absl::SleepFor(absl::Milliseconds(200));
            }

            // This should unblock.
            reader.join();

            server_done.Notify();

            return absl::UnknownError("Sentinel return");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Client side cancel.
  packet_receiver->Cancel();
  EXPECT_TRUE(absl::IsCancelled(packet_receiver->Finish()));
  server_done.WaitForNotification();
}

TEST_F(PacketReceiverTest, ControlledReadOnlyMissedHeartbeat) {
  int num_packets = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Just stop writing anything at all.
            // Emulate failed heartbeat.

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  options.advanced.heartbeat_interval = absl::Milliseconds(100);
  options.advanced.heartbeat_grace_period = absl::Milliseconds(100);
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read a few packets.
  Packet p;
  bool ok;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
    EXPECT_TRUE(ok);
  }

  // Close read channel.
  EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
  EXPECT_FALSE(ok);

  // Close write channel.
  packet_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsCancelled(packet_receiver->Finish()));
}

TEST_F(PacketReceiverTest, ControlledReadOnlyLateCommitsDone) {
  int num_packets = 5;

  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());
            EXPECT_TRUE(req.setup_request().has_writes_done_grace_period());
            auto writes_done_grace_period = ToAbseilDuration(
                req.setup_request().writes_done_grace_period());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            EXPECT_TRUE(TestWriteDoneRequestResponse(&resp).ok());
            EXPECT_TRUE(stream->Write(resp));

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    writes_done_grace_period)) {
              context->TryCancel();
            }
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  // Intentionally set a small writes done grace period.
  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  options.advanced.writes_done_grace_period = absl::Milliseconds(100);
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read a few packets.
  Packet p;
  bool ok;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
    EXPECT_TRUE(ok);
  }

  // Close read channel.
  EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &ok));
  EXPECT_FALSE(ok);

  // Intentionally close the write channel late.
  absl::SleepFor(absl::Seconds(1));
  packet_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsCancelled(packet_receiver->Finish()));
}

TEST_F(PacketReceiverTest, ControlledCommitAll) {
  int num_packets = 5;

  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            //
            // The client will commit twice, once during the middle of the
            // stream, and another after it gets the last one.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_TRUE(stream->Read(&req));
              EXPECT_TRUE(req.has_commit_request());
              EXPECT_EQ(req.commit_request().offset(), 2);
              EXPECT_TRUE(stream->Read(&req));
              EXPECT_TRUE(req.has_commit_request());
              EXPECT_EQ(req.commit_request().offset(), 4);
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            EXPECT_TRUE(TestWriteDoneRequestResponse(&resp).ok());
            EXPECT_TRUE(stream->Write(resp));

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "controlled";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read a few packets.
  Packet p;
  bool read_ok;
  bool write_ok;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(
        packet_receiver->Receive(absl::InfiniteDuration(), &p, &read_ok));
    EXPECT_TRUE(read_ok);
    if (i == 2) {
      EXPECT_TRUE(packet_receiver->Commit(absl::InfiniteDuration(),
                                          GetOffset(p), &write_ok));
      EXPECT_TRUE(write_ok);
    }
  }

  // Close read channel.
  EXPECT_TRUE(packet_receiver->Receive(absl::InfiniteDuration(), &p, &read_ok));
  EXPECT_FALSE(read_ok);

  // Commit last offset and close write channel.
  EXPECT_TRUE(packet_receiver->Commit(absl::InfiniteDuration(), GetOffset(p),
                                      &write_ok));
  EXPECT_TRUE(write_ok);
  packet_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsOutOfRange(packet_receiver->Finish()));
}

TEST_F(PacketReceiverTest, EagerReadCommonCase) {
  int num_packets = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            EXPECT_TRUE(TestWriteDoneRequestResponse(&resp).ok());
            EXPECT_TRUE(stream->Write(resp));

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "eager";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read all the packets.
  Packet p;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(&p).ok());
  }

  // Should reach the end.
  auto status = packet_receiver->Receive(&p);
  EXPECT_TRUE(absl::IsOutOfRange(status));
}

TEST_F(PacketReceiverTest, EagerReadServerSeverance) {
  int num_packets = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            EXPECT_TRUE(TestWriteDoneRequestResponse(&resp).ok());
            EXPECT_TRUE(stream->Write(resp));

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();

            return absl::DeadlineExceededError("Fake severance");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "eager";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read all the packets.
  Packet p;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(&p).ok());
  }

  // Should get a server severance.
  auto status = packet_receiver->Receive(&p);
  EXPECT_THAT(status, StatusIs(util::error::DEADLINE_EXCEEDED,
                               HasSubstr("Fake severance")));
}

TEST_F(PacketReceiverTest, EagerReadMissedHeartbeat) {
  int num_packets = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Just stop writing anything at all.
            // Emulate failed heartbeat.

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "eager";
  options.advanced.heartbeat_interval = absl::Milliseconds(100);
  options.advanced.heartbeat_grace_period = absl::Milliseconds(100);
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read all the packets.
  Packet p;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(&p).ok());
  }

  // Should get a client side cancelled.
  auto status = packet_receiver->Receive(&p);
  EXPECT_TRUE(absl::IsCancelled(status));
}

TEST_F(PacketReceiverTest, EagerReadClientEarlyTermination) {
  int num_packets = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < num_packets; ++i) {
              ReceivePacketsResponse resp;
              EXPECT_TRUE(TestPacketResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Just stop writing anything at all.
            // Emulate upstream having nothing to produce.

            // Wait for reader to complete.
            if (!reader_done.WaitForNotificationWithTimeout(
                    kTestWritesDoneGracePeriod)) {
              context->TryCancel();
            }
            reader.join();
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  PacketReceiver::Options options;
  EXPECT_TRUE(TestPacketReceiverOptions(&options).ok());
  options.receive_mode = "eager";
  VAI_ASSERT_OK_AND_ASSIGN(auto packet_receiver, PacketReceiver::Create(options));

  // Read all the packets.
  Packet p;
  for (int i = 0; i < num_packets; ++i) {
    EXPECT_TRUE(packet_receiver->Receive(absl::Milliseconds(100), &p).ok());
  }

  // Should get a client side not found.
  auto status = packet_receiver->Receive(absl::Milliseconds(100), &p);
  EXPECT_TRUE(absl::IsNotFound(status));
}
}  // namespace

}  // namespace testing
}  // namespace visionai
