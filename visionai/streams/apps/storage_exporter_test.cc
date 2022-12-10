// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
// "visionai/streams/apps/storage_exporter.h"

#include "visionai/streams/apps/storage_exporter.h"

#include <thread>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/warehouse.grpc.pb.h"
#include "google/cloud/visionai/v1/warehouse.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/apps/storage_exporter.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_path.h"
#include "visionai/util/net/grpc/client_connect.h"

namespace visionai {

namespace testing {

using ::testing::Invoke;

using ::google::cloud::visionai::v1::AcquireLeaseRequest;
using ::google::cloud::visionai::v1::Lease;

using ::google::cloud::visionai::v1::ReceiveEventsRequest;
using ::google::cloud::visionai::v1::ReceiveEventsResponse;

using ::google::cloud::visionai::v1::ReceivePacketsRequest;
using ::google::cloud::visionai::v1::ReceivePacketsResponse;

using ::google::cloud::visionai::v1::IngestAssetRequest;
using ::google::cloud::visionai::v1::IngestAssetResponse;
using ::google::cloud::visionai::v1::Warehouse;

constexpr char kTestProjectId[] = "some-project-id";
constexpr char kTestLocationId[] = "some-location-id";
constexpr char kTestClusterId[] = "some-cluster-id";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestAssetName[] = "some-asset-name";
constexpr char kTestReceiverId[] = "some-receiver-id";

class MockWarehouse : public Warehouse::Service {
 public:
  using ServiceType = ::google::cloud::visionai::v1::Warehouse;
  MockWarehouse() = default;
  ~MockWarehouse() override = default;
  MOCK_METHOD(grpc::Status, IngestAsset,
              (grpc::ServerContext * context,
               (grpc::ServerReaderWriter<IngestAssetResponse,
                                         IngestAssetRequest>)*stream),
              (override));

  MockWarehouse(const MockWarehouse&) = delete;
  MockWarehouse& operator=(const MockWarehouse&) = delete;
};

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class StorageExporterTest : public ::testing::Test {
 protected:
  StorageExporterTest()
      : grpc_mocker_(),
        mock_streaming_service_(
            grpc_mocker_.service<MockStreamingService>()),
        mock_warehouse_(grpc_mocker_.service<MockWarehouse>()),
        local_server_address_(grpc_mocker_.local_credentials_server_address()) {
  }

  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    auto status = GstInit();
    ASSERT_TRUE(status.ok());
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
    ASSERT_TRUE(GstRegisterPlugins().ok());
  }

  ClusterSelection TestClusterSelection() {
    ClusterSelection cluster_selection;
    *cluster_selection.mutable_service_endpoint() = local_server_address_;
    *cluster_selection.mutable_project_id() = kTestProjectId;
    *cluster_selection.mutable_location_id() = kTestLocationId;
    *cluster_selection.mutable_cluster_id() = kTestClusterId;
    *cluster_selection.mutable_cluster_endpoint() = local_server_address_;
    return cluster_selection;
  }
  MockMultipleServicesGrpcServer<MockStreamingService, MockWarehouse>
      grpc_mocker_;
  MockStreamingService* mock_streaming_service_;
  MockWarehouse* mock_warehouse_;
  std::string local_server_address_;
};

TEST_F(StorageExporterTest, SuccessfulExport) {
  ON_CALL(*mock_streaming_service_, AcquireLease)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const AcquireLeaseRequest* request, Lease* lease) {
            lease->set_id("lease-123");
            return ::grpc::Status::OK;
          }));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillRepeatedly(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));

            std::vector<GstreamerBuffer> buffers;
            GstreamerRunner::Options options;
            options.processing_pipeline_string = "videotestsrc num-buffers = 5";
            options.receiver_callback =
                [&](GstreamerBuffer buffer) -> absl::Status {
              buffers.push_back(buffer);
              return absl::OkStatus();
            };
            auto runner = GstreamerRunner::Create(options).value();
            runner->WaitUntilCompleted(absl::InfiniteDuration());

            // Write a few packets interspersed with heartbeats.
            for (int i = 0; i < buffers.size(); ++i) {
              ReceivePacketsResponse resp;
              Packet p = MakePacket(buffers[i]).value();
              EXPECT_TRUE(SetOffset(i, &p).ok());
              EXPECT_TRUE(stream->Write(resp));
              absl::SleepFor(absl::Milliseconds(30));
            }

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  EXPECT_CALL(*mock_streaming_service_, ReceiveEvents)
      .WillOnce(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceiveEventsResponse,
                                              ReceiveEventsRequest>* stream) {
            // Expect setup message for handshake.
            ReceiveEventsRequest req;
            EXPECT_TRUE(stream->Read(&req));

            // Simulate the server reader.
            absl::Notification reader_done;
            std::thread reader([stream, &reader_done]() {
              ReceiveEventsRequest req;
              // Expect the commit messages.
              EXPECT_TRUE(stream->Read(&req));
              EXPECT_EQ(req.commit_request().offset(), 42);
              reader_done.Notify();
            });

            ReceiveEventsResponse resp;
            *resp.mutable_event_update()->mutable_event() = "ev-0";
            resp.mutable_event_update()->set_offset(42);
            EXPECT_TRUE(stream->Write(resp));
            // Wait for reader to complete.
            reader_done.WaitForNotification();
            reader.join();

            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  StorageExporter::Options options;
  options.streaming_server_addr = local_server_address_;
  options.mwh_server_addr = local_server_address_;
  options.cluster_selection = TestClusterSelection();
  options.receiver_id = kTestReceiverId;
  options.stream_id = kTestStreamId;
  options.asset_name = kTestAssetName;
  options.h264_only = false;
  options.h264_mux_only = false;
  options.temp_video_dir = file::JoinPath(::testing::TempDir(), "test_dir");

  StorageExporter exporter(options);
  ASSERT_TRUE(exporter.Export().ok());
}

}  // namespace testing

}  // namespace visionai
