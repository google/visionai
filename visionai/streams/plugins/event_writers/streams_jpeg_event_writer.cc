// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/event_writers/streams_jpeg_event_writer.h"

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "visionai/algorithms/media/gstreamer_async_jpeg_encoder.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/file_path.h"
#include "visionai/util/random_string.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {
constexpr int kRandomSenderNameLength = 8;
}

absl::Status StreamsJPEGEventWriter::Init(EventWriterInitContext* ctx) {
  VAI_ASSIGN_OR_RETURN(cluster_selection_, ctx->GetClusterSelection(),
                   _ << "while getting the ClusterSelection");
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("stream_id", &stream_id_))
      << "while getting the stream id";
  if (stream_id_.empty()) {
    return absl::InvalidArgumentError("Given an empty stream-id.");
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("sender_name", &sender_name_))
      << "while getting the sender name";
  if (sender_name_.empty()) {
    sender_name_ = RandomString(kRandomSenderNameLength);
    LOG(INFO) << absl::StrFormat(
        "An empty sender name was supplied; assigning the generated name "
        "\"%s\".",
        sender_name_);
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("local_dir", &local_dir_))
      << "while getting the local directory";
  return absl::OkStatus();
}

absl::Status StreamsJPEGEventWriter::Open(absl::string_view event_id) {
  event_id_ = std::string(event_id);
  if (!local_dir_.empty()) {
    VAI_RETURN_IF_ERROR(ValidateLocalDirectory(local_dir_))
        << "while checking if the local directory is valid";
  }
  VAI_RETURN_IF_ERROR(CheckStreamExists(cluster_selection_, stream_id_))
      << "while checking if a stream exists in the cluster";
  VAI_RETURN_IF_ERROR(CreateEventIfNotExist(cluster_selection_, event_id_))
      << "while creating an event in the cluster";
  VAI_RETURN_IF_ERROR(Bind(cluster_selection_, event_id_, stream_id_))
      << "while binding a stream to an event";

  // Currently, we just initialize the packet sender one time, upfront.
  // Might consider moving this to `Write` so that a retry is possible.
  //
  // TODO(b/247933460): Make the lease term configurable.
  PacketSender::Options options;
  options.cluster_selection = cluster_selection_;
  options.channel.event_id = event_id_;
  options.channel.stream_id = stream_id_;
  options.sender = sender_name_;

  VAI_ASSIGN_OR_RETURN(sender_, PacketSender::Create(options),
                   _ << "while creating a packet sender");
  LOG(INFO) << absl::StrFormat(
      "Sending data into event \"%s\" through stream \"%s\".", event_id_,
      stream_id_);
  return absl::OkStatus();
}

absl::Status StreamsJPEGEventWriter::Write(Packet pkt) {
  auto gstreamer_buffer = PacketAs<GstreamerBuffer>(pkt);
  if (!gstreamer_jpeg_encoder_) {
    gstreamer_jpeg_encoder_ =
        std::make_unique<GstreamerAsyncJpegEncoder<Packet>>(
          [this](absl::StatusOr<GstreamerBuffer> jpeg_gstreamer_buffer,
                 Packet p) {
            if (!jpeg_gstreamer_buffer.ok()) {
              LOG(ERROR) << "Failed to encode with status: "
                         << jpeg_gstreamer_buffer.status();
              return;
            }
            Pack(std::move(jpeg_gstreamer_buffer.value()), &p).IgnoreError();
            sender_->Send(std::move(p)).IgnoreError();
    },
    /* dimension */ absl::Status(absl::StatusCode::kUnavailable,
                                 "not initialized"),
    GetLocalFilePath());
  }
  VAI_RETURN_IF_ERROR(gstreamer_jpeg_encoder_->Feed(std::move(*gstreamer_buffer),
                                                std::move(pkt)));
  return absl::OkStatus();
}

absl::Status StreamsJPEGEventWriter::Close() {
  gstreamer_jpeg_encoder_.reset();
  sender_.reset();
  return absl::OkStatus();
}

absl::StatusOr<std::string> StreamsJPEGEventWriter::GetLocalFilePath() {
  if (local_dir_.empty()) {
    return absl::Status(absl::StatusCode::kUnavailable, "not initialized");
  }
  // file_path = ${local_dir_}/${event_id_}_%d.jpg
  return file::JoinPath(local_dir_, absl::StrCat(event_id_, "_", "%d.jpg"));
}

REGISTER_EVENT_WRITER_INTERFACE("StreamsJPEGEventWriter")
    .Attr("local_dir", "string")
    .Attr("sender_name", "string")
    .Attr("stream_id", "string")
    .Doc(R"doc(
StreamsJPEGEventWriter sends data into a Vision AI Stream.

sender_name: A name to identifying the sender.

stream_id: The stream to which data shall be sent. It must have been created
           beforehand.
)doc");

REGISTER_EVENT_WRITER_IMPLEMENTATION("StreamsJPEGEventWriter",
                                     StreamsJPEGEventWriter);

}  // namespace visionai
