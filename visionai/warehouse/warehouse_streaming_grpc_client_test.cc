// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/warehouse/warehouse_streaming_grpc_client.h"

#include <memory>
#include <string>
#include <thread>

#include "google/cloud/visionai/v1/warehouse.grpc.pb.h"
#include "google/cloud/visionai/v1/warehouse.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"

namespace visionai {
namespace {

using ::google::cloud::visionai::v1::IngestAssetRequest;
using ::google::cloud::visionai::v1::IngestAssetResponse;
using ::google::cloud::visionai::v1::Warehouse;
using ::testing::_;

class MockWarehouse : public Warehouse::Service {
 public:
  MockWarehouse() = default;
  ~MockWarehouse() override = default;

  MOCK_METHOD(grpc::Status, IngestAsset,
              (grpc::ServerContext * context,
               (grpc::ServerReaderWriter<IngestAssetResponse,
                                         IngestAssetRequest>)*stream),
              (override));

  MockWarehouse(const MockWarehouse &) = delete;
  MockWarehouse &operator=(const MockWarehouse &) = delete;
};

class WarehouseStreamingGrpcClientTest : public testing::Test {
 protected:
  static void SetUpTestSuite() {
    service_ = std::make_unique<MockWarehouse>();
    grpc::ServerBuilder builder;
    int port;
    builder.AddListeningPort("localhost:0", grpc::InsecureServerCredentials(),
                             &port);
    builder.RegisterService(service_.get());
    builder.SetMaxReceiveMessageSize(-1);
    server_ = builder.BuildAndStart();
    server_address_ =
        std::make_unique<std::string>(absl::StrCat("localhost:", port));
    server_worker_ = std::thread([&] { server_->Wait(); });
  }

  void SetUp() override {
    connection_options_ = visionai::DefaultConnectionOptions();
    connection_options_.mutable_ssl_options()->set_use_insecure_channel(true);
  }
  static void TearDownTestSuite() {
    server_->Shutdown();
    server_worker_.join();
  }

  ::visionai::ConnectionOptions
      connection_options_;
  static std::unique_ptr<grpc::Server> server_;
  static std::unique_ptr<MockWarehouse> service_;
  static std::thread server_worker_;
  static std::unique_ptr<std::string> server_address_;
};
std::unique_ptr<grpc::Server> WarehouseStreamingGrpcClientTest::server_ =
    nullptr;
std::unique_ptr<MockWarehouse> WarehouseStreamingGrpcClientTest::service_ =
    nullptr;
std::thread WarehouseStreamingGrpcClientTest::server_worker_;
std::unique_ptr<std::string> WarehouseStreamingGrpcClientTest::server_address_ =
    nullptr;

TEST_F(WarehouseStreamingGrpcClientTest, GetResponseFail) {
  EXPECT_CALL(*service_, IngestAsset(_, _))
      .Times(1)
      .WillOnce([&](grpc::ServerContext *context,
                    grpc::ServerReaderWriter<IngestAssetResponse,
                                             IngestAssetRequest> *stream) {
        IngestAssetRequest request;
        stream->Read(&request);
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
      });

  WarehouseStreamingGrpcClient<Warehouse, IngestAssetRequest,
                               IngestAssetResponse>
      client(connection_options_);
  client.Initialize(
      visionai::CreateChannel(*server_address_, connection_options_));
  IngestAssetRequest request;
  request.mutable_config()->set_asset("fake-asset");
  IngestAssetResponse response;
  EXPECT_TRUE(client.SendRequest(request).ok());
  EXPECT_EQ(client.GetResponse(&response).code(), absl::StatusCode::kInternal);
}

TEST_F(WarehouseStreamingGrpcClientTest, GetResponseSucceed) {
  IngestAssetResponse expected_response;
  expected_response.mutable_successfully_ingested_partition()
      ->mutable_start_time()
      ->set_seconds(10);
  expected_response.mutable_successfully_ingested_partition()
      ->mutable_end_time()
      ->set_seconds(20);
  EXPECT_CALL(*service_, IngestAsset(_, _))
      .Times(1)
      .WillOnce([&](grpc::ServerContext *context,
                    grpc::ServerReaderWriter<IngestAssetResponse,
                                             IngestAssetRequest> *stream) {
        IngestAssetRequest request;
        stream->Read(&request);
        stream->Write(expected_response);
        return grpc::Status::OK;
      });

  WarehouseStreamingGrpcClient<Warehouse, IngestAssetRequest,
                               IngestAssetResponse>
      client(connection_options_);
  client.Initialize(
      visionai::CreateChannel(*server_address_, connection_options_));
  IngestAssetRequest request;
  request.mutable_config()->set_asset("fake-asset");
  IngestAssetResponse response;
  EXPECT_TRUE(client.SendRequest(request).ok());
  EXPECT_TRUE(client.GetResponse(&response).ok());
  EXPECT_EQ(response.successfully_ingested_partition().start_time().seconds(),
            expected_response.successfully_ingested_partition()
                .start_time()
                .seconds());
  EXPECT_EQ(
      response.successfully_ingested_partition().end_time().seconds(),
      expected_response.successfully_ingested_partition().end_time().seconds());
}

}  // namespace
}  // namespace visionai
