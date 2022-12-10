// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/warehouse/warehouse_ingester.h"

#include <memory>
#include <thread>

#include "google/cloud/visionai/v1/warehouse.grpc.pb.h"
#include "google/cloud/visionai/v1/warehouse.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/strings/str_cat.h"
#include "absl/types/optional.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/telemetry/metrics/labels.h"

namespace visionai {
namespace {

using ::google::cloud::visionai::v1::IngestAssetRequest;
using ::google::cloud::visionai::v1::IngestAssetResponse;
using ::google::cloud::visionai::v1::Partition;
using ::google::cloud::visionai::v1::Warehouse;
using ::testing::_;

constexpr absl::string_view kAssetName =
    "projects/123345/locations/us-west1/corpora/342533/assets/3432523";
constexpr absl::string_view kTestFile =
    "visionai/testing/testdata/media/pngs/google_logo.png";

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

ConnectionOptions InsecureConnectionOptions() {
  ConnectionOptions connection_options = DefaultConnectionOptions();
  connection_options.mutable_ssl_options()->set_use_insecure_channel(true);
  return connection_options;
}

class WarehouseIngesterTest : public testing::Test {
 protected:
  static void SetUpTestSuite() {
    service_ = std::make_unique<MockWarehouse>();
    grpc::ServerBuilder builder;
    srand(time(NULL));
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
    warehouse_ingester_ = std::make_unique<WarehouseIngester>(
        *server_address_, (struct MetricStreamResourceLabels){},
        InsecureConnectionOptions());
  }

  static void TearDownTestSuite() {
    server_->Shutdown();
    server_worker_.join();
  }

  static std::unique_ptr<grpc::Server> server_;
  static std::unique_ptr<MockWarehouse> service_;
  static std::thread server_worker_;
  static std::unique_ptr<std::string> server_address_;
  std::unique_ptr<WarehouseIngester> warehouse_ingester_;
};

std::unique_ptr<grpc::Server> WarehouseIngesterTest::server_;
std::unique_ptr<MockWarehouse> WarehouseIngesterTest::service_;
std::thread WarehouseIngesterTest::server_worker_;
std::unique_ptr<std::string> WarehouseIngesterTest::server_address_;

TEST_F(WarehouseIngesterTest, GetResponseSucceed) {
  IngestAssetResponse expected_response;
  EXPECT_CALL(*service_, IngestAsset(_, _))
      .Times(1)
      .WillOnce([&](grpc::ServerContext *context,
                    grpc::ServerReaderWriter<IngestAssetResponse,
                                             IngestAssetRequest> *stream) {
        IngestAssetRequest request;
        stream->Read(&request);
        stream->Read(&request);
        stream->Write(expected_response);
        return grpc::Status::OK;
      });

  Partition::TemporalPartition temporal_partition;
  EXPECT_TRUE(
      warehouse_ingester_->Initialize(kAssetName.data(), absl::nullopt).ok());
  EXPECT_TRUE(
      warehouse_ingester_->IngestFile(kTestFile.data(), temporal_partition)
          .ok());
  EXPECT_TRUE(warehouse_ingester_->GetIngestResponse().status().ok());
  EXPECT_TRUE(warehouse_ingester_->Finish().ok());
}

TEST_F(WarehouseIngesterTest, GetResponseFail) {
  EXPECT_CALL(*service_, IngestAsset(_, _))
      .Times(1)
      .WillOnce([&](grpc::ServerContext *context,
                    grpc::ServerReaderWriter<IngestAssetResponse,
                                             IngestAssetRequest> *stream) {
        IngestAssetRequest request;
        stream->Read(&request);
        stream->Read(&request);
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                            "Invalid Argument error");
      });

  Partition::TemporalPartition temporal_partition;

  EXPECT_TRUE(
      warehouse_ingester_->Initialize(kAssetName.data(), absl::nullopt).ok());
  EXPECT_TRUE(
      warehouse_ingester_->IngestFile(kTestFile.data(), temporal_partition)
          .ok());
  EXPECT_EQ(warehouse_ingester_->GetIngestResponse().status().code(),
            absl::StatusCode::kInternal);
  EXPECT_EQ(warehouse_ingester_->Finish().code(),
            absl::StatusCode::kInvalidArgument);
}

TEST_F(WarehouseIngesterTest, GetAllResponse) {
  int start_seconds = 10;
  int end_seconds = 15;
  IngestAssetResponse expected_response;
  expected_response.mutable_successfully_ingested_partition()
      ->mutable_start_time()
      ->set_seconds(start_seconds);
  expected_response.mutable_successfully_ingested_partition()
      ->mutable_end_time()
      ->set_seconds(end_seconds);
  EXPECT_CALL(*service_, IngestAsset(_, _))
      .Times(1)
      .WillOnce([&](grpc::ServerContext *context,
                    grpc::ServerReaderWriter<IngestAssetResponse,
                                             IngestAssetRequest> *stream) {
        IngestAssetRequest request;
        stream->Read(&request);
        stream->Read(&request);
        stream->Read(&request);
        stream->Write(expected_response);
        stream->Write(expected_response);
        return grpc::Status::OK;
      });

  Partition::TemporalPartition temporal_partition;

  EXPECT_TRUE(
      warehouse_ingester_->Initialize(kAssetName.data(), absl::nullopt).ok());
  EXPECT_TRUE(
      warehouse_ingester_->IngestFile(kTestFile.data(), temporal_partition)
          .ok());
  EXPECT_TRUE(
      warehouse_ingester_->IngestFile(kTestFile.data(), temporal_partition)
          .ok());
  auto response = warehouse_ingester_->GetAllIngestResponse();
  EXPECT_TRUE(response[0].ok());
  EXPECT_EQ(response[0].value().start_time().seconds(), start_seconds);
  EXPECT_EQ(response[0].value().end_time().seconds(), end_seconds);
  EXPECT_TRUE(response[1].ok());
  EXPECT_EQ(response[1].value().start_time().seconds(), start_seconds);
  EXPECT_EQ(response[1].value().end_time().seconds(), end_seconds);

  EXPECT_TRUE(warehouse_ingester_->Finish().ok());
}

TEST_F(WarehouseIngesterTest, FinishNotGettingAllResponse) {
  int start_seconds = 10;
  int end_seconds = 15;
  IngestAssetResponse expected_response;
  expected_response.mutable_successfully_ingested_partition()
      ->mutable_start_time()
      ->set_seconds(start_seconds);
  expected_response.mutable_successfully_ingested_partition()
      ->mutable_end_time()
      ->set_seconds(end_seconds);
  EXPECT_CALL(*service_, IngestAsset(_, _))
      .Times(1)
      .WillOnce([&](grpc::ServerContext *context,
                    grpc::ServerReaderWriter<IngestAssetResponse,
                                             IngestAssetRequest> *stream) {
        IngestAssetRequest request;
        stream->Read(&request);
        stream->Read(&request);
        stream->Write(expected_response);
        return grpc::Status::OK;
      });

  Partition::TemporalPartition temporal_partition;

  EXPECT_TRUE(
      warehouse_ingester_->Initialize(kAssetName.data(), absl::nullopt).ok());
  EXPECT_TRUE(
      warehouse_ingester_->IngestFile(kTestFile.data(), temporal_partition)
          .ok());
  EXPECT_EQ(warehouse_ingester_->Finish().code(),
            absl::StatusCode::kFailedPrecondition);
  auto response = warehouse_ingester_->GetAllIngestResponse();
  EXPECT_TRUE(warehouse_ingester_->Finish().ok());
}

TEST_F(WarehouseIngesterTest, InitializeWithSecureChannel) {
  EXPECT_CALL(*service_, IngestAsset(_, _)).Times(0);

  warehouse_ingester_ = std::make_unique<WarehouseIngester>(
      *server_address_, (struct MetricStreamResourceLabels){});

  EXPECT_FALSE(
      warehouse_ingester_->Initialize(kAssetName.data(), absl::nullopt).ok());
}

}  // namespace
}  // namespace visionai
