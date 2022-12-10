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

#include "visionai/streams/plugins/event_writers/streams_event_writer.h"

#include <memory>

#include "google/cloud/visionai/v1/streams_resources.pb.h"
#include "google/cloud/visionai/v1/streams_service.pb.h"
#include "google/cloud/visionai/v1alpha1/streams_resources.pb.h"
#include "google/cloud/visionai/v1alpha1/streams_service.pb.h"
#include "google/protobuf/text_format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/strings/substitute.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/streams/client/mock_packet_sender.h"
#include "visionai/streams/client/mock_streams_service.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/testing/status/status_matchers.h"

namespace visionai {

using ::google::cloud::visionai::v1::CreateEventRequest;
using ::google::cloud::visionai::v1::CreateSeriesRequest;
using ::google::cloud::visionai::v1::Event;
using ::google::cloud::visionai::v1::GetEventRequest;
using ::google::cloud::visionai::v1::GetSeriesRequest;
using ::google::cloud::visionai::v1::GetStreamRequest;
using ::google::cloud::visionai::v1::Series;
using ::google::cloud::visionai::v1::Stream;
using ::google::longrunning::Operation;
using ::google::protobuf::TextFormat;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::Return;

constexpr char kTestSenderName[] = "some-sender-name";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestProjectId[] = "some-project-id";
constexpr char kTestLocationId[] = "some-location-id";
constexpr char kTestClusterId[] = "some-cluster-id";
constexpr char kTestEventId[] = "some-event-id";
constexpr char kTestConfigTemplate[] = R"pb(
  name: "StreamsEventWriter"
  attr { key: "sender_name" value: "$0" }
  attr { key: "stream_id" value: "$1" }
  attr { key: "encoded", value: "$2"}
  cluster_selection {
    service_endpoint: "$3"
    project_id: "$4"
    location_id: "$5"
    cluster_id: "$6"
    use_insecure_channel: true
  }
)pb";
constexpr char kXH264MediaType[] = "video/x-h264";
constexpr char kXRawMediaType[] = "video/x-raw";

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class StreamsEventWriterTest : public ::testing::Test {
 protected:
  StreamsEventWriterTest()
      : mock_streams_server_(),
        mock_streams_service_(mock_streams_server_.service()),
        local_server_address_(
            mock_streams_server_.local_credentials_server_address()) {}

  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    ASSERT_TRUE(GstInit().ok());
    ASSERT_TRUE(GstRegisterPlugins().ok());
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  }

  void SetUp() override {
    mock_packet_sender_ =
        std::make_shared<MockPacketSender>(PacketSender::Options());
  }

  void WritePackets(std::shared_ptr<StreamsEventWriter> writer) {
    GstreamerRunner::Options options;
    options.processing_pipeline_string =
        "videotestsrc num-buffers=10 is-live=true";
    options.receiver_callback =
        [&writer](GstreamerBuffer buffer) -> absl::Status {
      LOG(ERROR) << buffer.caps_cstr();
      VAI_ASSIGN_OR_RETURN(Packet p, MakePacket(buffer));
      return writer->Write(p);
    };
    auto runner = GstreamerRunner::Create(options).value();
    runner->WaitUntilCompleted(absl::Seconds(3));
  }
  testing::MockGrpcServer<MockStreamsService> mock_streams_server_;
  MockStreamsService* mock_streams_service_;
  std::string local_server_address_;

  std::shared_ptr<MockPacketSender> mock_packet_sender_ = nullptr;
};


TEST_F(StreamsEventWriterTest, EmptyStreamId) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>();
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestSenderName,
                      /* stream_id */ "", /* encoded */ false,
                       local_server_address_, kTestProjectId, kTestLocationId,
                       kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_THAT(writer->Init(context.get()),
              absl::InvalidArgumentError("Given an empty stream-id."));
  ASSERT_TRUE(writer->Close().ok());
}

TEST_F(StreamsEventWriterTest, EmptySenderName) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>();
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, /* sender_name */ "",
                       kTestStreamId, /* encoded */ false,
                       local_server_address_, kTestProjectId, kTestLocationId,
                       kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());
  ASSERT_TRUE(writer->Close().ok());
}

TEST_F(StreamsEventWriterTest, StreamNotExist) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>();

  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestSenderName, kTestStreamId,
                       /* encoded */ false, local_server_address_,
                       kTestProjectId, kTestLocationId,
                       kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_name,
                       MakeClusterName(config.cluster_selection()));
  VAI_ASSERT_OK_AND_ASSIGN(
      auto stream_name,
      MakeStreamName(cluster_name, std::string(kTestStreamId)));

  EXPECT_CALL(*mock_streams_service_, GetStream)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const GetStreamRequest* request, Stream* stream) {
        EXPECT_EQ(request->name(), stream_name);
        stream->set_name(kTestStreamId);
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "no stream");
      }));

  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());

  ASSERT_THAT(writer->Open(kTestEventId),
              StatusIs(grpc::StatusCode::NOT_FOUND));
  ASSERT_TRUE(writer->Close().ok());
}

TEST_F(StreamsEventWriterTest,
       EventAndSeriesNotExistThenCreateNewEventAndSeries) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>();

  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestSenderName, kTestStreamId,
                       /* encoded */ false, local_server_address_,
                       kTestProjectId, kTestLocationId,
                       kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_name,
                       MakeClusterName(config.cluster_selection()));
  VAI_ASSERT_OK_AND_ASSIGN(
      auto channel_id,
      MakeChannelId(std::string(kTestEventId), std::string(kTestStreamId)));

  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());

  EXPECT_CALL(*mock_streams_service_, GetStream)
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_CALL(*mock_streams_service_, GetEvent)
      .WillOnce(Return(grpc::Status(grpc::StatusCode::NOT_FOUND, "no event")));
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
      .WillOnce(Return(grpc::Status(grpc::StatusCode::NOT_FOUND, "no series")));
  EXPECT_CALL(*mock_streams_service_, CreateSeries)
      .WillOnce(
          Invoke([&](::grpc::ServerContext* context,
                     const CreateSeriesRequest* request, Operation* operation) {
            EXPECT_EQ(request->parent(), cluster_name);
            EXPECT_EQ(request->series_id(), channel_id);
            operation->set_done(true);
            return ::grpc::Status::OK;
          }));

  ASSERT_TRUE(writer->Open(kTestEventId).ok());
  ASSERT_TRUE(writer->Close().ok());
}

TEST_F(StreamsEventWriterTest, OpenHappyPath) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>();

  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestSenderName, kTestStreamId,
                       /* encoded */ false, local_server_address_,
                       kTestProjectId, kTestLocationId, kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto cluster_name,
                       MakeClusterName(config.cluster_selection()));
  VAI_ASSERT_OK_AND_ASSIGN(
      auto channel_id,
      MakeChannelId(std::string(kTestEventId), std::string(kTestStreamId)));
  VAI_ASSERT_OK_AND_ASSIGN(auto channel_name,
                       MakeChannelName(cluster_name, channel_id));
  VAI_ASSERT_OK_AND_ASSIGN(auto event_name,
                       MakeEventName(cluster_name, std::string(kTestEventId)));

  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());

  EXPECT_CALL(*mock_streams_service_, GetStream)
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_CALL(*mock_streams_service_, GetEvent)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const GetEventRequest* request, Event* event) {
        EXPECT_EQ(request->name(), event_name);
        event->set_name(event_name);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_streams_service_, GetSeries)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const GetSeriesRequest* request, Series* series) {
        EXPECT_EQ(request->name(), channel_name);
        return grpc::Status::OK;
      }));

  ASSERT_TRUE(writer->Open(kTestEventId).ok());
  ASSERT_TRUE(writer->Close().ok());
}

TEST_F(StreamsEventWriterTest, WriteUnencodedPacket) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>(mock_packet_sender_);
  EXPECT_CALL(*mock_packet_sender_, Send(_))
      .Times(10)
      .WillRepeatedly(Invoke([&](Packet p) {
        PacketAs<GstreamerBuffer> packet_as_gbuf(p);
        EXPECT_TRUE(packet_as_gbuf.status().ok());
        // Verify the media type of the sent packet is "video/x-raw"
        EXPECT_EQ(packet_as_gbuf->media_type(), kXRawMediaType);
        return absl::OkStatus();
      }));
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestSenderName, kTestStreamId,
                       /* encoded */ false, local_server_address_,
                       kTestProjectId, kTestLocationId, kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());
  WritePackets(writer);
  ASSERT_TRUE(writer->Close().ok());
}

TEST_F(StreamsEventWriterTest, WriteEncodedPacket) {
  std::shared_ptr<StreamsEventWriter> writer =
      std::make_shared<StreamsEventWriter>(mock_packet_sender_);
  EXPECT_CALL(*mock_packet_sender_, Send(_))
      .Times(AtLeast(1))
      .WillRepeatedly(Invoke([&](Packet p) {
        PacketAs<GstreamerBuffer> packet_as_gbuf(p);
        LOG(ERROR) <<packet_as_gbuf->caps_cstr();
        LOG(ERROR) <<packet_as_gbuf->caps_cstr();
        EXPECT_TRUE(packet_as_gbuf.status().ok());
        // Verify the media type of the sent packet is "video/x-h264"
        EXPECT_EQ(packet_as_gbuf->media_type(), kXH264MediaType);
        return absl::OkStatus();
      }));
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestSenderName, kTestStreamId,
                       /* encoded */ true, local_server_address_,
                       kTestProjectId, kTestLocationId, kTestClusterId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());
  WritePackets(writer);
}

}  // namespace visionai
