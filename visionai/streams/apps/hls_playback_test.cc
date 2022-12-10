// Copyright 2022 Google LLC
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "visionai/streams/apps/hls_playback.h"

#include <thread>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/apps/util/event_loop_runner.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/plugins/event_writers/hls_event_writer.h"
#include "visionai/streams/util/hls/playlist_m3u8.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/file_path.h"

namespace visionai {
namespace testing {

using ::testing::Invoke;

using ::google::cloud::visionai::v1::AcquireLeaseRequest;
using ::google::cloud::visionai::v1::Lease;
using ::google::cloud::visionai::v1::RenewLeaseRequest;

using ::google::cloud::visionai::v1::ReceiveEventsRequest;
using ::google::cloud::visionai::v1::ReceiveEventsResponse;

using ::google::cloud::visionai::v1::ReceivePacketsRequest;
using ::google::cloud::visionai::v1::ReceivePacketsResponse;

constexpr char kTestProjectId[] = "some-project-id";
constexpr char kTestLocationId[] = "some-location-id";
constexpr char kTestClusterId[] = "some-cluster-id";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestReceiverId[] = "some-receiver-id";
constexpr int kHLSTargetVideoDurationSeconds = 1;
constexpr int kHLSMaxFiles = 10;

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class HLSPlaybackForTest : public HLSPlayback {
 public:
  HLSPlaybackForTest(const HLSPlayback::Options& options)
      : HLSPlayback(options) {}

  ~HLSPlaybackForTest() override {}
  std::unique_ptr<EventLoopRunner> CreateEventLoopRunner(
      EventLoopRunner::Options options) override {
    event_loop_runner_options_ = options;
    return std::make_unique<EventLoopRunner>(options);
  }

  std::shared_ptr<HLSEventWriter> CreateEventWriter(
      HLSEventWriter::Options options) override {
    hls_event_writer_options_ = options;
    return std::make_shared<HLSEventWriter>(options);
  }

  EventLoopRunner::Options event_loop_runner_options_;
  HLSEventWriter::Options hls_event_writer_options_;
};

class HLSPlaybackTest : public ::testing::Test {
 protected:
  HLSPlaybackTest()
      : grpc_mocker_(),
        mock_streaming_service_(
            grpc_mocker_.service<MockStreamingService>()),
        local_server_address_(grpc_mocker_.local_credentials_server_address()) {
  }

  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    ASSERT_TRUE(GstRegisterPlugins().ok());
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  }

  void SetUp() override {
    local_dir_ = file::JoinPath(::testing::TempDir(), "test_hls_dir");
    ASSERT_TRUE(CreateDir(local_dir_).ok());
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

  std::vector<streams_internal::HLSSegment> GetHLSSegments(
      const std::string& path) {
    std::string playlist_content;
    GetFileContents(path, &playlist_content).IgnoreError();
    return streams_internal::ParsePlaylistSegments(playlist_content);
  }

  void AssertClusterSelection(const ClusterSelection& cluster_selection) {
    ASSERT_EQ(cluster_selection.service_endpoint(), local_server_address_);
    ASSERT_EQ(cluster_selection.project_id(), kTestProjectId);
    ASSERT_EQ(cluster_selection.location_id(), kTestLocationId);
    ASSERT_EQ(cluster_selection.cluster_id(), kTestClusterId);
    ASSERT_EQ(cluster_selection.cluster_endpoint(), local_server_address_);
  }

  void AssertEventUpdateReceiverOptions(
      const EventUpdateReceiver::Options& event_update_receiver_options) {
    AssertClusterSelection(event_update_receiver_options.cluster_selection);
    ASSERT_EQ(event_update_receiver_options.stream_id, kTestStreamId);
    ASSERT_EQ(event_update_receiver_options.receiver, kTestReceiverId);
    ASSERT_EQ(event_update_receiver_options.starting_logical_offset,
              "most-recent");
    ASSERT_EQ(event_update_receiver_options.fallback_starting_offset, "end");
  }

  void AssertPacketReceiverOptions(
      const PacketReceiver::Options& packet_receiver_options) {
    AssertClusterSelection(packet_receiver_options.cluster_selection);
    ASSERT_EQ(packet_receiver_options.receive_mode, "eager");
    ASSERT_EQ(packet_receiver_options.channel.stream_id, kTestStreamId);
    ASSERT_EQ(packet_receiver_options.lessee,
              absl::StrCat(kTestReceiverId, "-packet-receiver"));
  }

  void AssertEventWriterOptions(
      const HLSEventWriter::Options& event_writer_options) {
    ASSERT_EQ(event_writer_options.target_duration_in_sec,
              kHLSTargetVideoDurationSeconds);
    ASSERT_EQ(event_writer_options.max_files, kHLSMaxFiles);
    ASSERT_EQ(event_writer_options.local_dir, local_dir_);
    ASSERT_EQ(event_writer_options.labels.stream_id, kTestStreamId);
    ASSERT_EQ(event_writer_options.labels.cluster_id, kTestClusterId);
    ASSERT_EQ(event_writer_options.labels.location_id, kTestLocationId);
    ASSERT_EQ(event_writer_options.labels.project_id, kTestProjectId);
  }

  MockMultipleServicesGrpcServer<MockStreamingService> grpc_mocker_;
  MockStreamingService* mock_streaming_service_;
  std::string local_server_address_;
  std::string local_dir_;
};

TEST_F(HLSPlaybackTest, SuccessfulExecution) {
  ON_CALL(*mock_streaming_service_, AcquireLease)
      .WillByDefault(
          Invoke([&](::grpc::ServerContext* context,
                     const AcquireLeaseRequest* request, Lease* lease) {
            lease->set_id("lease-123");
            return ::grpc::Status::OK;
          }));
  EXPECT_CALL(*mock_streaming_service_, RenewLease)
      .WillRepeatedly(Invoke([&](::grpc::ServerContext* context,
                                 const RenewLeaseRequest* request,
                                 Lease* lease) { return ::grpc::Status::OK; }));

  EXPECT_CALL(*mock_streaming_service_, ReceivePackets)
      .WillRepeatedly(
          Invoke([&](grpc::ServerContext* context,
                     grpc::ServerReaderWriter<ReceivePacketsResponse,
                                              ReceivePacketsRequest>* stream) {
            ReceivePacketsRequest req;
            EXPECT_TRUE(stream->Read(&req));

            std::vector<GstreamerBuffer> buffers;
            GstreamerRunner::Options options;
            options.processing_pipeline_string =
                "videotestsrc num-buffers = 200 is-live=true";
            options.receiver_callback =
                [&](GstreamerBuffer buffer) -> absl::Status {
              ReceivePacketsResponse resp;
              Packet p = MakePacket(buffer).value();
              buffers.push_back(buffer);
              *resp.mutable_packet() = p;
              stream->Write(resp);
              return absl::OkStatus();
            };
            auto runner = GstreamerRunner::Create(options).value();
            runner->WaitUntilCompleted(absl::Seconds(5));
            LOG(INFO) << "ReceivePacket finished.";
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
              EXPECT_EQ(req.commit_request().offset(), 43);
              reader_done.Notify();
            });

            ReceiveEventsResponse resp;
            *resp.mutable_event_update()->mutable_event() = "ev-0";
            resp.mutable_event_update()->set_offset(42);
            EXPECT_TRUE(stream->Write(resp));

            *resp.mutable_event_update()->mutable_event() = "ev-1";
            resp.mutable_event_update()->set_offset(43);
            EXPECT_TRUE(stream->Write(resp));
            // Wait for reader to complete.
            reader_done.WaitForNotification();
            reader.join();
            LOG(INFO) << "ReceiveEvent finished.";
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE,
                                "End of message stream");
          }));

  HLSPlayback::Options options;
  options.streaming_server_addr = local_server_address_;
  options.cluster_selection = TestClusterSelection();
  options.receiver_id = kTestReceiverId;
  options.stream_id = kTestStreamId;
  options.local_dir = local_dir_;
  options.target_duration_in_sec = kHLSTargetVideoDurationSeconds;
  options.max_files = kHLSMaxFiles;

  HLSPlaybackForTest playback(options);
  ASSERT_TRUE(playback.Run().ok());

  AssertEventUpdateReceiverOptions(
      playback.event_loop_runner_options_.event_receiver_options);
  AssertPacketReceiverOptions(
      playback.event_loop_runner_options_.packet_receiver_options);
  AssertEventWriterOptions(playback.hls_event_writer_options_);
}
}  // namespace testing
}  // namespace visionai
