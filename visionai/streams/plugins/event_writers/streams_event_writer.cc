// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/event_writers/streams_event_writer.h"

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_join.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/util/random_string.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
constexpr int kRandomSenderNameLength = 8;
}

absl::Status StreamsEventWriter::Init(EventWriterInitContext* ctx) {
  VAI_ASSIGN_OR_RETURN(cluster_selection_, ctx->GetClusterSelection(),
                   _ << "while getting the ClusterSelection");
  grace_period_ = ctx->GetGracePeriod();
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("stream_id", &stream_id_))
      << "while getting the stream id";
  if (stream_id_.empty()) {
    return absl::InvalidArgumentError("Given an empty stream-id.");
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("sender_name", &sender_name_))
      << "while getting the stream id";
  if (sender_name_.empty()) {
    sender_name_ = RandomString(kRandomSenderNameLength);
    LOG(INFO) << absl::StrFormat(
        "An empty sender name was supplied; assigning the generated name "
        "\"%s\".",
        sender_name_);
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr<bool>("encoded", &encoded_))
      << "while getting the encoded option";
  return absl::OkStatus();
}

absl::Status StreamsEventWriter::Open(absl::string_view event_id) {
  event_id_ = std::string(event_id);
  VAI_RETURN_IF_ERROR(CheckStreamExists(cluster_selection_, stream_id_))
      << "while checking if a stream exists in the cluster";
  VAI_RETURN_IF_ERROR(CreateEventIfNotExist(cluster_selection_, event_id_))
      << "while creating an event in the cluster";
  VAI_RETURN_IF_ERROR(Bind(cluster_selection_, event_id_, stream_id_))
      << "while binding a stream to an event";

  // Currently, we just initialize the packet sender one time, upfront.
  // Might consider moving this to `Write` so that a retry is possible.
  //
  PacketSender::Options options;
  options.cluster_selection = cluster_selection_;
  options.channel.event_id = event_id_;
  options.channel.stream_id = stream_id_;
  options.sender = sender_name_;
  options.grace_period = grace_period_;

  VAI_ASSIGN_OR_RETURN(sender_, PacketSender::Create(options),
                   _ << "while creating a packet sender");
  LOG(INFO) << absl::StrFormat(
      "Sending data into event \"%s\" through stream \"%s\".", event_id_,
      stream_id_);
  return absl::OkStatus();
}

absl::Status StreamsEventWriter::Write(Packet p) {
  // TODO(b/235906195): refactor out the encoded stream event writer.
  if (!encoded_) {
    return sender_->Send(std::move(p));
  } else {
    auto gstreamer_buffer = PacketAs<GstreamerBuffer>(std::move(p));
    VAI_RETURN_IF_ERROR(gstreamer_buffer.status());
    if (!gstreamer_runner_) {
      // Create a GstreamerRunner with the x264 encoding pipeline.
      GstreamerRunner::Options gstreamer_runner_options;
      gstreamer_runner_options.appsrc_caps_string =
          gstreamer_buffer->caps_string();
      gstreamer_runner_options.processing_pipeline_string =
          AssembleGstreamerPipeline();
      LOG(ERROR) << "Launching the gstreamer pipeline: "
                 << gstreamer_runner_options.processing_pipeline_string;
      LOG(ERROR) << "Accepting the caps string: "
                 << gstreamer_runner_options.appsrc_caps_string;
      gstreamer_runner_options.receiver_callback =
          [this](GstreamerBuffer encoded_gstreamer_buffer) -> absl::Status {
        LOG(ERROR) << encoded_gstreamer_buffer.caps_string();
        VAI_ASSIGN_OR_RETURN(auto p,
                         MakePacket(std::move(encoded_gstreamer_buffer)));
        return sender_->Send(std::move(p));
      };

      VAI_ASSIGN_OR_RETURN(gstreamer_runner_,
                       GstreamerRunner::Create(gstreamer_runner_options));
    }
    VAI_RETURN_IF_ERROR(gstreamer_runner_->Feed(*gstreamer_buffer));

    return absl::OkStatus();
  }
}

absl::Status StreamsEventWriter::Close() {
  sender_.reset();
  return absl::OkStatus();
}

std::string StreamsEventWriter::AssembleGstreamerPipeline() {
  std::vector<std::string> pipeline_elements;
  pipeline_elements.push_back("videoconvert");
  pipeline_elements.push_back("videorate");
  // TODO: make output frame rate configurable.
  pipeline_elements.push_back("video/x-raw,framerate=25/1");
  pipeline_elements.push_back("x264enc");
  return absl::StrJoin(pipeline_elements, " ! ");
}

REGISTER_EVENT_WRITER_INTERFACE("StreamsEventWriter")
    .Attr("sender_name", "string")
    .Attr("stream_id", "string")
    .Attr("encoded", "bool")
    .Doc(R"doc(
StreamsEventWriter sends data into a Vision AI Stream.

sender_name: A name to identifying the sender.

stream_id: The stream to which data shall be sent. It must have been created
           beforehand.
)doc");

REGISTER_EVENT_WRITER_IMPLEMENTATION("StreamsEventWriter", StreamsEventWriter);

}  // namespace visionai
