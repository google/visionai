// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_send_packets_grpc_client.h"

#include <string>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/sync_stream.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/net/grpc/client_connect.h"

namespace visionai {
namespace testing {

namespace {

using ::google::cloud::visionai::v1::SendPacketsRequest;
using ::google::cloud::visionai::v1::SendPacketsResponse;
using ::testing::Invoke;

class StreamingSendPacketsTest : public ::testing::Test {
 protected:
  StreamingSendPacketsTest()
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

TEST_F(StreamingSendPacketsTest, Basic) {
  EXPECT_CALL(*mock_streaming_service_, SendPackets)
      .WillOnce(
          Invoke([this](grpc::ServerContext* context,
                        grpc::ServerReaderWriter<SendPacketsResponse,
                                                 SendPacketsRequest>* stream) {
            SendPacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_metadata());
            EXPECT_EQ(req.metadata().event(),
                      MakeEventName(cluster_name_, "some-event").value());
            EXPECT_EQ(req.metadata().stream(),
                      MakeStreamName(cluster_name_, "some-stream").value());
            EXPECT_TRUE(stream->Read(&req));
            EXPECT_TRUE(req.has_packet());
            auto s = PacketAs<std::string>(std::move(*req.mutable_packet()));
            EXPECT_TRUE(s.ok());
            EXPECT_EQ(*s, "hello!");
            return grpc::Status::OK;
          }));

  StreamingSendPacketsGrpcClient::Options options;
  options.target_address = local_credentials_address_;
  options.channel.event_id = "some-event";
  options.channel.stream_id = "some-stream";
  options.sender = "some-sender";
  options.cluster_name = cluster_name_;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      true);
  auto send_client_statusor = StreamingSendPacketsGrpcClient::Create(options);
  EXPECT_TRUE(send_client_statusor.ok());
  auto send_client = *std::move(send_client_statusor);

  auto p = MakePacket(std::string("hello!"));
  EXPECT_TRUE(p.ok());
  auto status = send_client->Send(*std::move(p));
  EXPECT_TRUE(status.ok());
}
}  // namespace

}  // namespace testing
}  // namespace visionai
