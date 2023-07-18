// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/event_update_receiver.h"

#include <cstdint>

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
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/mock_streams_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace testing {
namespace {

using ::google::cloud::visionai::v1::Cluster;
using ::google::cloud::visionai::v1::GetClusterRequest;
using ::google::cloud::visionai::v1::ReceiveEventsRequest;
using ::google::cloud::visionai::v1::ReceiveEventsResponse;
using ::testing::HasSubstr;
using ::testing::Invoke;

constexpr char kTestProjectId[] = "some-project-id";
constexpr char kTestLocationId[] = "some-location-id";
constexpr char kTestClusterId[] = "some-cluster-id";
constexpr char kTestEventId[] = "some-event-id";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestReceiver[] = "some-receiver";
constexpr char kTestStartingLogicalOffset[] = "stored";
constexpr char kTestFallbackStartingOffset[] = "begin";
constexpr absl::Duration kTestWritesDoneGracePeriod = absl::Seconds(10);

class EventUpdateReceiverTest : public ::testing::Test {
 protected:
  EventUpdateReceiverTest()
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


  absl::Status TestEventUpdateReceiverOptions(
      EventUpdateReceiver::Options* options) {
    VAI_ASSIGN_OR_RETURN(options->cluster_selection, TestClusterSelection());
    options->stream_id = kTestStreamId;
    options->receiver = kTestReceiver;
    options->starting_logical_offset = kTestStartingLogicalOffset;
    options->fallback_starting_offset = kTestFallbackStartingOffset;
    return absl::OkStatus();
  }

  absl::Status CheckSetupRequest(const ReceiveEventsRequest& request) {
    EXPECT_TRUE(request.has_setup_request());
    EXPECT_EQ(request.setup_request().cluster(), cluster_name_);
    EXPECT_EQ(request.setup_request().stream(),
              MakeStreamName(cluster_name_, kTestStreamId).value());
    EXPECT_EQ(request.setup_request().receiver(), kTestReceiver);
    EXPECT_EQ(
        request.setup_request().controlled_mode().starting_logical_offset(),
        kTestStartingLogicalOffset);
    EXPECT_EQ(
        request.setup_request().controlled_mode().fallback_starting_offset(),
        kTestFallbackStartingOffset);
    return absl::OkStatus();
  }

  absl::Status TestEventUpdate(int64_t offset, EventUpdate* event_update) {
    event_update->set_stream(
        MakeStreamName(cluster_name_, kTestStreamId).value());
    event_update->set_event(MakeEventName(cluster_name_, kTestEventId).value());
    auto channel_id = MakeChannelId(kTestEventId, kTestStreamId).value();
    event_update->set_series(
        MakeChannelName(cluster_name_, channel_id).value());
    VAI_RETURN_IF_ERROR(SetOffset(offset, event_update));
    return absl::OkStatus();
  }

  absl::Status TestEventUpdateResponse(int64_t offset,
                                       ReceiveEventsResponse* response) {
    EXPECT_TRUE(TestEventUpdate(offset, response->mutable_event_update()).ok());
    return absl::OkStatus();
  }

  absl::Status TestHeartbeatResponse(ReceiveEventsResponse* response) {
    response->mutable_control()->set_heartbeat(true);
    return absl::OkStatus();
  }

  absl::Status TestWriteDoneRequestResponse(ReceiveEventsResponse* response) {
    response->mutable_control()->set_writes_done_request(true);
    return absl::OkStatus();
  }

  absl::Status TestWriteDoneRequestResponseV1(
      google::cloud::visionai::v1::ReceiveEventsResponse* response) {
    response->mutable_control()->set_writes_done_request(true);
    return absl::OkStatus();
  }

  MockMultipleServicesGrpcServer<MockStreamsService, MockStreamingService>
      grpc_mocker_;
  MockStreamsService* mock_streams_service_;
  MockStreamingService* mock_streaming_service_;
  std::string local_server_address_;
  const std::string cluster_name_ =
      MakeClusterName(kTestProjectId, kTestLocationId, kTestClusterId).value();
};

TEST_F(EventUpdateReceiverTest, BasicInvalidOptions) {
  {
    EventUpdateReceiver::Options options;
    EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
    options.stream_id = "";
    auto event_update_receiver = EventUpdateReceiver::Create(options);
    EXPECT_FALSE(event_update_receiver.ok());
  }
  {
    EventUpdateReceiver::Options options;
    EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
    options.cluster_selection.set_cluster_id("");
    auto event_update_receiver = EventUpdateReceiver::Create(options);
    EXPECT_FALSE(event_update_receiver.ok());
  }
}

TEST_F(EventUpdateReceiverTest, GetClusterRejection) {
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            return grpc::Status(grpc::StatusCode::UNKNOWN,
                                "Intentaionl GetCluster fail");
          }));
  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  auto event_update_receiver = EventUpdateReceiver::Create(options);
  EXPECT_THAT(
      event_update_receiver.status(),
      StatusIs(grpc::StatusCode::UNKNOWN, HasSubstr("GetCluster fail")));
}

TEST_F(EventUpdateReceiverTest, ConstructionSuccess) {
  ON_CALL(*mock_streams_service_, GetCluster)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceiveEventsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Request writes done.
            ReceiveEventsResponse resp;
            EXPECT_TRUE(TestWriteDoneRequestResponse(&resp).ok());
            stream->Write(resp);

            // Wait for reader to complete.
            reader_done.WaitForNotification();
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));
}

TEST_F(EventUpdateReceiverTest, SetupRequestRejection) {
  ON_CALL(*mock_streams_service_, GetCluster)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            return grpc::Status(grpc::StatusCode::UNKNOWN,
                                "Intentional Setup rejection");
          }));

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // Close read channel.
  EventUpdate event_update;
  bool ok;
  EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                             &event_update, &ok));
  EXPECT_FALSE(ok);

  // Close write channel.
  event_update_receiver->CommitsDone();

  // Get result.
  EXPECT_THAT(
      event_update_receiver->Finish(),
      StatusIs(grpc::StatusCode::UNKNOWN, HasSubstr("Setup rejection")));
}

TEST_F(EventUpdateReceiverTest, ReadOnlyCommonCase) {
  int num_event_updates = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceiveEventsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few event updates interspersed with heartbeats.
            for (int i = 0; i < num_event_updates; ++i) {
              ReceiveEventsResponse resp;
              EXPECT_TRUE(TestEventUpdateResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceiveEventsResponse resp;
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

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // Read a few updates.
  EventUpdate event_update;
  bool ok;
  for (int i = 0; i < num_event_updates; ++i) {
    EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                               &event_update, &ok));
    EXPECT_TRUE(ok);
  }

  // Close read channel.
  EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                             &event_update, &ok));
  EXPECT_FALSE(ok);

  // Close write channel.
  event_update_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsOutOfRange(event_update_receiver->Finish()));
}

TEST_F(EventUpdateReceiverTest, ServerCancelTest) {
  absl::Notification server_cancelled;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Server cancels.
            context->TryCancel();
            server_cancelled.Notify();

            return grpc::Status(grpc::StatusCode::UNKNOWN, "Sentinel return");
          }));

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // This test server does not write.
  // Read should timeout before cancellation.
  EventUpdate event_update;
  bool read_ok = true;
  EXPECT_FALSE(event_update_receiver->Receive(absl::ZeroDuration(),
                                              &event_update, &read_ok));

  server_cancelled.WaitForNotification();

  // Read should fail after cancelled.
  EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                             &event_update, &read_ok));
  EXPECT_FALSE(read_ok);

  // Attempt to commit. This eventually fails as the committer will realize the
  // server has closed.
  bool write_ok = true;
  while (write_ok) {
    EXPECT_TRUE(
        event_update_receiver->Commit(absl::InfiniteDuration(), 42, &write_ok));
    absl::SleepFor(absl::Milliseconds(100));
  }

  // Close write channel.
  event_update_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsCancelled(event_update_receiver->Finish()));
}

TEST_F(EventUpdateReceiverTest, ClientCancelTest) {
  int num_event_updates = 5;
  absl::Notification server_done;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceiveEventsRequest req;
              EXPECT_FALSE(stream->Read(&req));
            });

            // Write a few event updates interspersed with heartbeats.
            for (int i = 0; i < num_event_updates; ++i) {
              ReceiveEventsResponse resp;
              EXPECT_TRUE(TestEventUpdateResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Try to send event updates. Client will eventually cancel.
            // No point requesting a writes done afterwards.
            ReceiveEventsResponse resp;
            while (stream->Write(resp)) {
              absl::SleepFor(absl::Milliseconds(200));
            }

            // This should unblock.
            reader.join();

            server_done.Notify();
            return grpc::Status(grpc::StatusCode::CANCELLED,
                                "Client cancelled.");
          }));

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // Read a few event updates.
  EventUpdate event_update;
  bool ok;
  for (int i = 0; i < num_event_updates; ++i) {
    EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                               &event_update, &ok));
    EXPECT_TRUE(ok);
  }

  // Client side cancel.
  event_update_receiver->Cancel();
  EXPECT_TRUE(absl::IsCancelled(event_update_receiver->Finish()));
  server_done.WaitForNotification();
}

TEST_F(EventUpdateReceiverTest, ReadOnlyMissedHeartbeat) {
  int num_event_updates = 5;
  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceiveEventsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few event updates interspersed with heartbeats.
            for (int i = 0; i < num_event_updates; ++i) {
              ReceiveEventsResponse resp;
              EXPECT_TRUE(TestEventUpdateResponse(i, &resp).ok());
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

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  options.advanced.heartbeat_interval = absl::Milliseconds(100);
  options.advanced.heartbeat_grace_period = absl::Milliseconds(100);
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // Read a few event updates.
  EventUpdate event_update;
  bool ok;
  for (int i = 0; i < num_event_updates; ++i) {
    EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                               &event_update, &ok));
    EXPECT_TRUE(ok);
  }

  // Close read channel.
  EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                             &event_update, &ok));
  EXPECT_FALSE(ok);

  // Close write channel.
  event_update_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsCancelled(event_update_receiver->Finish()));
}

TEST_F(EventUpdateReceiverTest, ReadOnlyLateCommitsDone) {
  int num_event_updates = 5;

  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());
            EXPECT_TRUE(req.setup_request().has_writes_done_grace_period());
            auto writes_done_grace_period = ToAbseilDuration(
                req.setup_request().writes_done_grace_period());

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceiveEventsRequest req;
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few event updates interspersed with heartbeats.
            for (int i = 0; i < num_event_updates; ++i) {
              ReceiveEventsResponse resp;
              EXPECT_TRUE(TestEventUpdateResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceiveEventsResponse resp;
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
  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  options.advanced.writes_done_grace_period = absl::Milliseconds(100);
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // Read a few event updates.
  EventUpdate event_update;
  bool ok;
  for (int i = 0; i < num_event_updates; ++i) {
    EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                               &event_update, &ok));
    EXPECT_TRUE(ok);
  }

  // Close read channel.
  EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                             &event_update, &ok));
  EXPECT_FALSE(ok);

  // Intentionally close the write channel late.
  absl::SleepFor(absl::Seconds(1));
  event_update_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsCancelled(event_update_receiver->Finish()));
}

TEST_F(EventUpdateReceiverTest, ControlledCommitAll) {
  int num_event_updates = 5;

  EXPECT_CALL(*mock_streams_service_, GetCluster)
      .WillRepeatedly(
          Invoke([&](::grpc::ServerContext* context,
                     const GetClusterRequest* request, Cluster* cluster) {
            cluster->set_dataplane_service_endpoint(local_server_address_);
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(CheckSetupRequest(req).ok());

            // Simulate the server reader.
            //
            // The client will commit twice, once during the middle of the
            // stream, and another after it gets the last one.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceiveEventsRequest req;
              EXPECT_TRUE(stream->Read(&req));
              EXPECT_TRUE(req.has_commit_request());
              EXPECT_EQ(req.commit_request().offset(), 2);
              EXPECT_TRUE(stream->Read(&req));
              EXPECT_TRUE(req.has_commit_request());
              EXPECT_EQ(req.commit_request().offset(), 4);
              EXPECT_FALSE(stream->Read(&req));
              reader_done.Notify();
            });

            // Write a few event updates interspersed with heartbeats.
            for (int i = 0; i < num_event_updates; ++i) {
              ReceiveEventsResponse resp;
              EXPECT_TRUE(TestEventUpdateResponse(i, &resp).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));

              EXPECT_TRUE(TestHeartbeatResponse(&resp).ok());
              resp.mutable_control()->set_heartbeat(true);
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            // Request writes done.
            ReceiveEventsResponse resp;
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

  EventUpdateReceiver::Options options;
  EXPECT_TRUE(TestEventUpdateReceiverOptions(&options).ok());
  VAI_ASSERT_OK_AND_ASSIGN(auto event_update_receiver,
                       EventUpdateReceiver::Create(options));

  // Read a few event updates.
  EventUpdate event_update;
  bool read_ok;
  bool write_ok;
  for (int i = 0; i < num_event_updates; ++i) {
    EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                               &event_update, &read_ok));
    EXPECT_TRUE(read_ok);
    if (i == 2) {
      EXPECT_TRUE(event_update_receiver->Commit(
          absl::InfiniteDuration(), GetOffset(event_update), &write_ok));
      EXPECT_TRUE(write_ok);
    }
  }

  // Close read channel.
  EXPECT_TRUE(event_update_receiver->Receive(absl::InfiniteDuration(),
                                             &event_update, &read_ok));
  EXPECT_FALSE(read_ok);

  // Commit last offset and close write channel.
  EXPECT_TRUE(event_update_receiver->Commit(
      absl::InfiniteDuration(), GetOffset(event_update), &write_ok));
  EXPECT_TRUE(write_ok);
  event_update_receiver->CommitsDone();

  // Get result.
  EXPECT_TRUE(absl::IsOutOfRange(event_update_receiver->Finish()));
}

}  // namespace

}  // namespace testing
}  // namespace visionai
