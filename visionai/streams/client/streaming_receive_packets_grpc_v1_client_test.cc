// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_receive_packets_grpc_v1_client.h"

#include <string>
#include <thread>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/time.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/sync_stream.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {
namespace testing {

namespace {

using ::google::cloud::visionai::v1::ReceivePacketsRequest;
using ::google::cloud::visionai::v1::ReceivePacketsResponse;
using ::testing::Invoke;

class StreamingReceivePacketsV1Test : public ::testing::Test {
 protected:
  StreamingReceivePacketsV1Test()
      : mock_streaming_server_(),
        mock_streaming_service_(mock_streaming_server_.service()),
        local_credentials_address_(
            mock_streaming_server_.local_credentials_server_address()) {}

  MockGrpcServer<MockStreamingService> mock_streaming_server_;
  MockStreamingService* mock_streaming_service_;

  std::string local_credentials_address_;
  const std::string cluster_name_ =
      "projects/test-project/locations/test-location/clusters/test-cluster";
};

TEST_F(StreamingReceivePacketsV1Test, SetupSuccessOnlyTest) {
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  ON_CALL(*mock_streaming_service_, ReceivePackets)
      .WillByDefault(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            ReceivePacketsRequest req;
            stream->Read(&req);
            return grpc::Status(grpc::StatusCode::UNKNOWN,
                                "Sentinel error return.");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);
}

TEST_F(StreamingReceivePacketsV1Test, SetupRejectionTest) {
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());

            // Simulate rejection.
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                                "Some bogus failed precondition.");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  // Client will detect a rejection. Close all channels and finish.
  ReceivePacketsResponse response;
  EXPECT_FALSE(receive_client->Read(&response));
  EXPECT_TRUE(receive_client->WritesDone());
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsFailedPrecondition(rpc_status));
}

TEST_F(StreamingReceivePacketsV1Test, ClientDestuctorTest) {
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());

            // Simulate rejection.
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                                "Some bogus failed precondition.");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  // Client will detect a rejection. Close all channels and finish.
  ReceivePacketsResponse response;
  EXPECT_FALSE(receive_client->Read(&response));
  EXPECT_TRUE(receive_client->WritesDone());
  absl::Status rpc_status;
  EXPECT_TRUE(receive_client->Finish(&rpc_status).ok());
  EXPECT_TRUE(absl::IsFailedPrecondition(rpc_status));
}

TEST_F(StreamingReceivePacketsV1Test, EagerNormalTest) {
  auto test_packet = MakePacket(std::string("hello!")).value();
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());
            EXPECT_EQ(req.setup_request().metadata().series(),
                      MakeChannelName(cluster_name_, channel_id).value());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
            });

            // Alternate sending packets and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceivePacketsResponse resp;
              if (i % 2 == 0) {
                *resp.mutable_packet() = test_packet;
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            resp.mutable_control()->set_writes_done_request(true);
            EXPECT_TRUE(stream->Write(resp));

            // This should unblock if client closes the write channel.
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceivePacketsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_packet());
      EXPECT_EQ(test_packet.ShortDebugString(),
                response.packet().ShortDebugString());
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

TEST_F(StreamingReceivePacketsV1Test, EagerEarlyWritesDoneTest) {
  auto test_packet = MakePacket(std::string("hello!")).value();
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());
            EXPECT_EQ(req.setup_request().metadata().series(),
                      MakeChannelName(cluster_name_, channel_id).value());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
            });

            // Send a few packets and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceivePacketsResponse resp;
              if (i % 2 == 0) {
                *resp.mutable_packet() = test_packet;
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            resp.mutable_control()->set_writes_done_request(true);
            EXPECT_TRUE(stream->Write(resp));

            // This should unblock if client closes the write channel.
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  // Writes done early.
  EXPECT_TRUE(receive_client->WritesDone());

  ReceivePacketsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_packet());
      EXPECT_EQ(test_packet.ShortDebugString(),
                response.packet().ShortDebugString());
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

TEST_F(StreamingReceivePacketsV1Test, EagerClientCancelTest) {
  auto test_packet = MakePacket(std::string("hello!")).value();
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());
            EXPECT_EQ(req.setup_request().metadata().series(),
                      MakeChannelName(cluster_name_, channel_id).value());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
            });

            // Send a few packets and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceivePacketsResponse resp;
              if (i % 2 == 0) {
                *resp.mutable_packet() = test_packet;
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Try to send packets packet. Client will eventually cancel.
            // No point requesting a writes done afterwards.
            ReceivePacketsResponse resp;
            *resp.mutable_packet() = test_packet;
            while (stream->Write(resp)) {
              absl::SleepFor(absl::Milliseconds(200));
            }

            // This should unblock.
            reader.join();

            return grpc::Status(grpc::StatusCode::CANCELLED,
                                "Client cancelled.");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceivePacketsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_packet());
      EXPECT_EQ(test_packet.ShortDebugString(),
                response.packet().ShortDebugString());
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

TEST_F(StreamingReceivePacketsV1Test, EagerServerCancelTest) {
  auto test_packet = MakePacket(std::string("hello!")).value();
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());
            EXPECT_EQ(req.setup_request().metadata().series(),
                      MakeChannelName(cluster_name_, channel_id).value());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceivePacketsRequest req;
              EXPECT_FALSE(stream->Read(&req));
            });

            // Send a few packets and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceivePacketsResponse resp;
              if (i % 2 == 0) {
                *resp.mutable_packet() = test_packet;
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
                                "Server cancelled.");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceivePacketsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_packet());
      EXPECT_EQ(test_packet.ShortDebugString(),
                response.packet().ShortDebugString());
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

TEST_F(StreamingReceivePacketsV1Test, ControlledCommitAllTest) {
  auto test_packet = MakePacket(std::string("hello!")).value();
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_id,
                       MakeChannelId("some-event", "some-stream"));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            // Expect setup message for handshake.
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_setup_request());
            EXPECT_EQ(req.setup_request().metadata().series(),
                      MakeChannelName(cluster_name_, channel_id).value());

            // Simulate the server reader.
            std::thread reader([stream]() {
              ReceivePacketsRequest req;
              for (int i = 0; i < 5; ++i) {
                EXPECT_TRUE(stream->Read(&req));
                EXPECT_TRUE(req.has_commit_request());
                EXPECT_EQ(req.commit_request().offset(), i);
              }
              EXPECT_FALSE(stream->Read(&req));
            });

            // Alternate sending packets and heartbeats.
            for (int i = 0; i < 10; ++i) {
              ReceivePacketsResponse resp;
              if (i % 2 == 0) {
                *resp.mutable_packet() = test_packet;
                EXPECT_TRUE(SetOffset(i / 2, resp.mutable_packet()).ok());
                EXPECT_TRUE(stream->Write(resp));
              } else {
                resp.mutable_control()->set_heartbeat(true);
                EXPECT_TRUE(stream->Write(resp));
              }
            }

            // Request writes done.
            ReceivePacketsResponse resp;
            resp.mutable_control()->set_writes_done_request(true);
            EXPECT_TRUE(stream->Write(resp));

            // This should unblock if client closes the write channel.
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  StreamingReceivePacketsGrpcV1Client::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.receiver = "some-receiver";
  options.receive_mode = "controlled";
  options.cluster_name = cluster_name_;
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(true);
  auto receive_client_statusor =
      StreamingReceivePacketsGrpcV1Client::Create(options);
  ASSERT_TRUE(receive_client_statusor.ok());
  auto receive_client = *std::move(receive_client_statusor);

  ReceivePacketsResponse response;
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(receive_client->Read(&response));
    if (i % 2 == 0) {
      ASSERT_TRUE(response.has_packet());
      EXPECT_EQ(i / 2, GetOffset(response.packet()));

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
