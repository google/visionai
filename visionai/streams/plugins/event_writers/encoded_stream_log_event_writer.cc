// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// The EncodedStreamLogEventWriter logs the received packet.
class EncodedStreamLogEventWriter : public EventWriter {
 public:
  EncodedStreamLogEventWriter() {}
  ~EncodedStreamLogEventWriter() override {}

  absl::Status Init(EventWriterInitContext* ctx) override {
    return absl::OkStatus();
  }

  absl::Status Open(absl::string_view event_id) override {
    event_id_ = std::string(event_id);
    return absl::OkStatus();
  }

  absl::Status Write(Packet p) override {
    auto gstreamer_buffer = PacketAs<GstreamerBuffer>(std::move(p));
    VAI_RETURN_IF_ERROR(gstreamer_buffer.status());
    if (!gstreamer_runner_) {
      // Create a GstreamerRunner with the x264 encoding pipeline.
      GstreamerRunner::Options gstreamer_runner_options;
      gstreamer_runner_options.appsrc_caps_string =
          gstreamer_buffer->caps_string();
      gstreamer_runner_options.processing_pipeline_string =
          AssembleGstreamerPipeline();
      LOG(INFO) << "Launching the gstreamer pipeline: "
                << gstreamer_runner_options.processing_pipeline_string;
      LOG(INFO) << "Accepting the caps string: "
                << gstreamer_runner_options.appsrc_caps_string;
      gstreamer_runner_options.receiver_callback =
          [this](GstreamerBuffer encoded_gstreamer_buffer) -> absl::Status {
        VAI_ASSIGN_OR_RETURN(auto p,
                         MakePacket(std::move(encoded_gstreamer_buffer)));
        LOG(INFO) << absl::StrFormat("(%s) ", event_id_) << p.DebugString();
        return absl::OkStatus();
      };

      VAI_ASSIGN_OR_RETURN(gstreamer_runner_,
                       GstreamerRunner::Create(gstreamer_runner_options));
    }
    VAI_RETURN_IF_ERROR(gstreamer_runner_->Feed(*gstreamer_buffer));

    return absl::OkStatus();
  }

  absl::Status Close() override { return absl::OkStatus(); }

 private:
  std::string event_id_;

  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;

  std::string AssembleGstreamerPipeline() {
    std::vector<std::string> pipeline_elements;
    pipeline_elements.push_back("rawvideoparse");
    pipeline_elements.push_back("videorate");
    // TODO: make output frame rate configurable.
    pipeline_elements.push_back("video/x-raw,framerate=25/1");
    pipeline_elements.push_back("x264enc");
    return absl::StrJoin(pipeline_elements, " ! ");
  }
};

REGISTER_EVENT_WRITER_INTERFACE("EncodedStreamLogEventWriter").Doc(R"doc(
EncodedStreamLogEventWriter logs the received packet.
It simply prints to stdout.
)doc");

REGISTER_EVENT_WRITER_IMPLEMENTATION("EncodedStreamLogEventWriter",
                                     EncodedStreamLogEventWriter);

}  // namespace visionai
