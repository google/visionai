// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/event_writers/hls_event_writer.h"

#include <fstream>
#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/time/time.h"
#include "re2/re2.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/gstreamer/constants.h"
#include "visionai/util/telemetry/metrics/stats.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {
#define METRIC_HLS_PACKET_DELAY_SEC(labels, capture_time) \
  hls_packet_delay_sec()                                  \
      .Add({{"project_id", labels.project_id},            \
            {"location_id", labels.location_id},          \
            {"cluster_id", labels.cluster_id},            \
            {"stream_id", labels.stream_id}})             \
      .Set(                                               \
          absl::FDivDuration((absl::Now() - capture_time), absl::Seconds(1)));
constexpr inline int KMaxLocalDirLength = 256;
}  // namespace

std::string HLSEventWriter::PipelineString() {
  std::vector<std::string> gst_pipeline;
  // TODO: Remove transcoding if the incoming buffer is already h264 encoded.
  gst_pipeline.push_back("decodebin");
  gst_pipeline.push_back("videoconvert");
  gst_pipeline.push_back("x264enc tune=zerolatency");
  gst_pipeline.push_back("h264parse");
  gst_pipeline.push_back(
      absl::StrFormat("hlssink2 target-duration=%d max-files=%d "
                      "location=%s/segment%%05d.ts "
                      "playlist-location=%s/playlist.m3u8",
                      options_.target_duration_in_sec, options_.max_files,
                      options_.local_dir, options_.local_dir));
  return absl::StrJoin(gst_pipeline, " ! ");
}

absl::Status HLSEventWriter::CreateMasterPlaylist() {
  std::string file_name = absl::StrFormat("%s/master.m3u8", options_.local_dir);
  // TODO(annikaz): Set the resolution and codes according to the caps string.
  return SetFileContents(
      file_name,
      "#EXTM3U\n#EXT-X-STREAM-INF:BANDWIDTH=150000,RESOLUTION=416x234,"
      "CODECS=\"avc1.640028\"\nplaylist.m3u8");
}

absl::Status HLSEventWriter::Init(EventWriterInitContext* ctx) {
  std::string stream_id;
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("stream_id", &stream_id));
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ctx->GetClusterSelection());
  options_.labels = GetMetricStreamResourceLabels(cluster_selection, stream_id);

  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("local_dir", &options_.local_dir));
  VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("max_files", &options_.max_files));
  VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("target_duration_in_sec",
                                    &options_.target_duration_in_sec));
  return absl::OkStatus();
}

absl::Status HLSEventWriter::Open(absl::string_view event_id) {
  event_id_ = std::string(event_id);
  VAI_RETURN_IF_ERROR(ValidateLocalDirectory(options_.local_dir));
  stats_reporter_ = std::make_unique<streams_internal::HLSStatsRecorder>(
      absl::StrFormat("%s/playlist.m3u8", options_.local_dir), options_.labels);
  stats_reporter_->SetEventWriterStartTime(absl::Now());
  return stats_reporter_->StartReport();
}

absl::Status HLSEventWriter::Write(Packet p) {
  absl::Time capture_time = GetCaptureTime(p);
  METRIC_HLS_PACKET_DELAY_SEC(options_.labels, capture_time);
  auto packet_as_gst_buffer = PacketAs<GstreamerBuffer>(std::move(p));
  VAI_RETURN_IF_ERROR(packet_as_gst_buffer.status());
  GstreamerBuffer gstreamer_buffer = packet_as_gst_buffer.value();
  if (!gstreamer_runner_) {
    VAI_RETURN_IF_ERROR(CreateMasterPlaylist());
    stats_reporter_->SetGstPipelineStartTime(absl::Now());
    stats_reporter_->SetFirstPacketCaptureTime(capture_time);

    // Initialize the gstreamer runner.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = PipelineString();
    options.appsrc_caps_string = gstreamer_buffer.caps_string();
    VAI_ASSIGN_OR_RETURN(gstreamer_runner_, GstreamerRunner::Create(options));
    LOG(INFO) << absl::StrFormat(
        "Running the gstreamer pipeline: %s\nAccepting caps:%s",
        options.processing_pipeline_string, options.appsrc_caps_string);
  }
  VAI_RETURN_IF_ERROR(gstreamer_runner_->Feed(gstreamer_buffer));
  return absl::OkStatus();
}

absl::Status HLSEventWriter::Close() {
  gstreamer_runner_->SignalEOS();
  if (!gstreamer_runner_->WaitUntilCompleted(
          kGstreamerRunnerFinalizationDeadline)) {
    return absl::DeadlineExceededError(
        "GstreamerRunner didn't finalize within the deadline");
  }
  VAI_RETURN_IF_ERROR(stats_reporter_->StopReport());
  VAI_RETURN_IF_ERROR(DeleteFilesInDir(options_.local_dir));
  return absl::OkStatus();
}

REGISTER_EVENT_WRITER_INTERFACE("HLSEventWriter")
    .Attr("local_dir", "string")
    .Attr("max_files", "int")
    .Attr("target_duration_in_sec", "int")
    .Attr("stream_id", "string")
    .Doc(R"doc(
HLSEventWriter creates video segments and playlists for HLS protocol.

Attributes:
  local_dir (string, required): the local dir to store the video segments.
  max_files (int, optional):
    the max number of files to keep on disk. Default is 10.
  target_duration_in_sec (int, optional):
    the target duration of each video segment in seconds. Default is 2.

)doc");

REGISTER_EVENT_WRITER_IMPLEMENTATION("HLSEventWriter", HLSEventWriter);

}  // namespace visionai
