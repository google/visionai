// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/event_writers/local_video_event_writer.h"

#include <memory>
#include <string>

#include "visionai/util/file_path.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "visionai/algorithms/media/gstreamer_video_writer.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
constexpr char kDefaultTmpVideoPath[] = "/tmp";
}

absl::Status LocalVideoEventWriter::Init(EventWriterInitContext* ctx) {
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("output", &output_));
  if (output_.empty()) {
    output_ = kDefaultTmpVideoPath;
    LOG(INFO) << "output attribute not set. Writing to default "
                  "temp location: "
              << output_;
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr<bool>("skip_until_first_key_frame",
                                     &skip_until_first_key_frame_));
  return absl::OkStatus();
}

absl::Status LocalVideoEventWriter::Open(absl::string_view event_id) {
  event_id_ = std::string(event_id);
  return absl::OkStatus();
}

absl::Status LocalVideoEventWriter::Write(Packet p) {
  auto gstreamer_buffer = PacketAs<GstreamerBuffer>(std::move(p));
  VAI_RETURN_IF_ERROR(gstreamer_buffer.status());
  got_key_frame_ = got_key_frame_ || gstreamer_buffer.value().is_key_frame();
  if (skip_until_first_key_frame_ && !got_key_frame_) {
    return absl::OkStatus();
  }
  if (!video_writer_) {
    GstreamerVideoWriter::Options options;
    options.file_path = file::JoinPath(output_,
                                       absl::StrCat(event_id_, ".mp4"));
    options.caps_string = gstreamer_buffer->caps_string();
    options.h264_mux_only = true;
    options.h264_only = true;
    LOG(ERROR) << "Launching the gstreamer video writer";
    VAI_ASSIGN_OR_RETURN(video_writer_, GstreamerVideoWriter::Create(options));
  }
  LOG_EVERY_T(INFO, 10) << "Writing...";
  return video_writer_->Put(gstreamer_buffer.value());
}

absl::Status LocalVideoEventWriter::Close() {
  video_writer_.reset();
  return absl::OkStatus();
}

REGISTER_EVENT_WRITER_INTERFACE("LocalVideoEventWriter")
    .Attr("output", "string")
    .Attr("skip_until_first_key_frame", "bool")
    .Doc(R"doc(
LocalVideoEventWriter saves the incoming packets to local mp4 file.

Attributes:
  output (string, required): the output dir for segmented mp4 files.
  skip_until_first_key_frame (bool, optional): if set true, the writer drops
    all the frames until a key frame arrives.
)doc");

REGISTER_EVENT_WRITER_IMPLEMENTATION("LocalVideoEventWriter",
                                     LocalVideoEventWriter);

}  // namespace visionai
