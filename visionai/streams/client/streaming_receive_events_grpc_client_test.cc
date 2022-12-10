// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_receive_events_grpc_client.h"

#include <string>
#include <thread>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/time.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/sync_stream.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/net/grpc/client_connect.h"

namespace visionai {
namespace testing {

namespace {

using ::google::cloud::visionai::v1::ReceiveEventsRequest;
using ::google::cloud::visionai::v1::ReceiveEventsResponse;
using ::testing::Invoke;

constexpr char kTestClusterName[] =
    "projects/test-project/locations/test-location/clusters/test-cluster";
constexpr char kTestEventId[] = "some-event-id";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestReceiver[] = "some-receiver";
constexpr char kTestStartingLogicalOffset[] = "stored";
constexpr char kTestFallbackStartingOffset[] = "begin";

class StreamingReceiveEventsTest : public ::testing::Test {
 protected:
  StreamingReceiveEventsTest()
      : mock_streaming_server_(),
        mock_streaming_service_(mock_streaming_server_.service()),
        local_credentials_address_(
            mock_streaming_server_.local_credentials_server_address()) {}

  absl::Status TestEventUpdate(int64_t offset, EventUpdate* event_update) {
    event_update->set_stream(
        MakeStreamName(kTestClusterName, kTestStreamId).value());
    event_update->set_event(
        MakeEventName(kTestClusterName, kTestEventId).value());
    auto channel_id = MakeChannelId(kTestEventId, kTestStreamId).value();
    event_update->set_series(
        MakeChannelName(kTestClusterName, channel_id).value());
    VAI_RETURN_IF_ERROR(SetOffset(offset, event_update));
    return absl::OkStatus();
  }

  StreamingReceiveEventsGrpcClient::Options
  TestStreamingReceiveEventsOptions() {
    StreamingReceiveEventsGrpcClient::Options options;
    options.target_address = local_credentials_address_;
    options.cluster_name = kTestClusterName;
    options.stream_id = kTestStreamId;
    options.receiver = kTestReceiver;
    options.starting_logical_offset = kTestStartingLogicalOffset;
    options.fallback_starting_offset = kTestFallbackStartingOffset;
    options.advanced.connection_options.mutable_ssl_options()
        ->set_use_insecure_channel(true);
    return options;
  }


  absl::Status CheckSetupRequest(const ReceiveEventsRequest& request) {
    EXPECT_TRUE(request.has_setup_request());
    EXPECT_EQ(request.setup_request().cluster(), kTestClusterName);
    EXPECT_EQ(request.setup_request().stream(),
              MakeStreamName(kTestClusterName, kTestStreamId).value());
    EXPECT_EQ(request.setup_request().receiver(), kTestReceiver);
    EXPECT_EQ(
        request.setup_request().controlled_mode().starting_logical_offset(),
        kTestStartingLogicalOffset);
    EXPECT_EQ(
        request.setup_request().controlled_mode().fallback_starting_offset(),
        kTestFallbackStartingOffset);
    return absl::OkStatus();
  }

  MockGrpcServer<MockStreamingService> mock_streaming_server_;
  MockStreamingService* mock_streaming_service_;
  std::string local_credentials_address_;
};

TEST_F(StreamingReceiveEventsTest, SetupSuccessOnlyTest) {
  ON_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillByDefault(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            ReceiveEventsRequest req;
            stream->Read(&req);
            return grpc::Status(grpc::StatusCode::UNKNOWN,
                                "Sentinel error return.");
          }));
  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);
}

TEST_F(StreamingReceiveEventsTest, SetupRejectionTest) {
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());

            // Simulate rejection.
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                                "Some bogus failed precondition.");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  // Client will detect a rejection. Close all channels and finish.
  ReceiveEventsResponse response;
  EXPECT_FALSE(receive_client->Read(&response));
  EXPECT_TRUE(receive_client->WritesDone());
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsFailedPrecondition(rpc_status));
}

TEST_F(StreamingReceiveEventsTest, ClientDestuctorTest) {
  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());

            // Simulate rejection.
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                                "Some bogus failed precondition.");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  // Client will detect a rejection. Close all channels and finish.
  ReceiveEventsResponse response;
  EXPECT_FALSE(receive_client->Read(&response));
  EXPECT_TRUE(receive_client->WritesDone());
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsFailedPrecondition(rpc_status));
}

TEST_F(StreamingReceiveEventsTest, ReadOnlyNormalTest) {
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

            // Alternate sending event updates and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceiveEventsResponse resp;
              if (i % 2 == 0) {
                EXPECT_TRUE(TestEventUpdate(i, resp.mutable_event_update()).ok());
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Request writes done.
            ReceiveEventsResponse resp;
            resp.mutable_control()->set_writes_done_request(true);
            EXPECT_TRUE(stream->Write(resp));

            // This should unblock if client closes the write channel.
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceiveEventsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_event_update());
      EXPECT_EQ(response.event_update().offset(), i);
    } else {
      ASSERT_TRUE(response.has_control());
      EXPECT_TRUE(response.control().has_heartbeat());
      EXPECT_TRUE(response.control().heartbeat());
    }
  }

  EXPECT_TRUE(receive_client->Read(&response));
  EXPECT_TRUE(response.has_control());
  EXPECT_TRUE(response.control().has_writes_done_request());

  EXPECT_TRUE(receive_client->WritesDone());
  EXPECT_FALSE(receive_client->Read(&response));

  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsOutOfRange(rpc_status));
}

TEST_F(StreamingReceiveEventsTest, WritesDoneEarlyTest) {
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

            // Alternate sending event updates and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceiveEventsResponse resp;
              if (i % 2 == 0) {
                EXPECT_TRUE(TestEventUpdate(i, resp.mutable_event_update()).ok());
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Request writes done.
            ReceiveEventsResponse resp;
            resp.mutable_control()->set_writes_done_request(true);
            EXPECT_TRUE(stream->Write(resp));

            // This should unblock if client closes the write channel.
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  // Close write channel early.
  EXPECT_TRUE(receive_client->WritesDone());

  ReceiveEventsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_event_update());
      EXPECT_EQ(response.event_update().offset(), i);
    } else {
      ASSERT_TRUE(response.has_control());
      EXPECT_TRUE(response.control().has_heartbeat());
      EXPECT_TRUE(response.control().heartbeat());
    }
  }

  EXPECT_TRUE(receive_client->Read(&response));
  EXPECT_TRUE(response.has_control());
  EXPECT_TRUE(response.control().has_writes_done_request());

  EXPECT_FALSE(receive_client->Read(&response));

  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsOutOfRange(rpc_status));
}

TEST_F(StreamingReceiveEventsTest, ServerCancelTest) {
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

            // Alternate sending event updates and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceiveEventsResponse resp;
              if (i % 2 == 0) {
                EXPECT_TRUE(TestEventUpdate(i, resp.mutable_event_update()).ok());
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Server cancel. No further actions needed.
            context->TryCancel();

            // This should unblock.
            reader.join();

            return grpc::Status(grpc::StatusCode::CANCELLED,
                                "Client cancelled.");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceiveEventsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_event_update());
      EXPECT_EQ(response.event_update().offset(), i);
    } else {
      ASSERT_TRUE(response.has_control());
      EXPECT_TRUE(response.control().has_heartbeat());
      EXPECT_TRUE(response.control().heartbeat());
    }
  }

  // Encounters an early read closure. Should still `WritesDone`.
  EXPECT_FALSE(receive_client->Read(&response));
  EXPECT_TRUE(receive_client->WritesDone());

  // Get RPC status.
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsCancelled(rpc_status));
}

TEST_F(StreamingReceiveEventsTest, ClientCancelTest) {
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

            // Alternate sending event updates and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceiveEventsResponse resp;
              if (i % 2 == 0) {
                EXPECT_TRUE(TestEventUpdate(i, resp.mutable_event_update()).ok());
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Try to send event updates. Client will eventually cancel.
            // No point requesting a writes done afterwards.
            ReceiveEventsResponse resp;
            while (stream->Write(resp)) {
              absl::SleepFor(absl::Milliseconds(200));
            }

            // This should unblock.
            reader.join();

            return grpc::Status(grpc::StatusCode::CANCELLED,
                                "Client cancelled.");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceiveEventsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_event_update());
      EXPECT_EQ(response.event_update().offset(), i);
    } else {
      ASSERT_TRUE(response.has_control());
      EXPECT_TRUE(response.control().has_heartbeat());
      EXPECT_TRUE(response.control().heartbeat());
    }
  }

  // Decide to cancel. No further action necessary.
  receive_client->Cancel();
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsCancelled(rpc_status));
}

TEST_F(StreamingReceiveEventsTest, ControlledCommitAllTest) {
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
              for (int i = 0; i < 5; ++i) {
                EXPECT_TRUE(stream->Read(&req));
                EXPECT_TRUE(req.has_commit_request());
                EXPECT_EQ(req.commit_request().offset(), i);
              }
              EXPECT_FALSE(stream->Read(&req));
            });

            // Alternate sending event updates and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceiveEventsResponse resp;
              if (i % 2 == 0) {
                EXPECT_TRUE(TestEventUpdate(i / 2, resp.mutable_event_update()).ok());
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Request writes done.
            ReceiveEventsResponse resp;
            resp.mutable_control()->set_writes_done_request(true);
            EXPECT_TRUE(stream->Write(resp));

            // This should unblock if client closes the write channel.
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  auto options = TestStreamingReceiveEventsOptions();
  auto receive_client_statusor =
      StreamingReceiveEventsGrpcClient::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceiveEventsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_event_update());
      EXPECT_EQ(i / 2, GetOffset(response.event_update()));

      // Commit, but intentionally defer the last one.
      if (i != 8) {
        EXPECT_TRUE(receive_client->WriteCommit(i / 2));
      }
    } else {
      ASSERT_TRUE(response.has_control());
      EXPECT_TRUE(response.control().has_heartbeat());
      EXPECT_TRUE(response.control().heartbeat());
    }
  }

  // When the server requests a writes done, Commit the last packet and close
  // the write channel.
  EXPECT_TRUE(receive_client->Read(&response));
  EXPECT_TRUE(response.has_control());
  EXPECT_TRUE(response.control().has_writes_done_request());
  EXPECT_TRUE(receive_client->WriteCommit(4));
  EXPECT_TRUE(receive_client->WritesDone());

  // Also close the read channel and get the final status.
  EXPECT_FALSE(receive_client->Read(&response));
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsOutOfRange(rpc_status));
}

}  // namespace

}  // namespace testing
}  // namespace visionai
