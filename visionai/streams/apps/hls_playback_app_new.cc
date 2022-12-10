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

#include "base/init_google.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/hls_playback.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/flags.h"
#include "visionai/util/telemetry/metrics/metrics_recorder.h"
#include "visionai/util/telemetry/metrics/prometheus_metrics_recorder.h"
#include "visionai/util/status/status_macros.h"

ABSL_FLAG(std::string, stream_name, "", "Stream Name");
ABSL_FLAG(std::string, service_endpoint, "",
          "The service endpoint to AIS control plane");
ABSL_FLAG(std::string, output_dir, "storage",
          "The output dir for fragmented video files");
ABSL_FLAG(int, max_files, 5,
          "The max number of files/segments to keep on disk.");
ABSL_FLAG(int, target_duration, 2,
          "The target duration (in seconds) of each file/segment.");
ABSL_FLAG(std::string, k8s_streaming_server_addr, "",
          "The k8s streaming server address");
ABSL_FLAG(std::string, k8s_event_discovery_server_addr, "",
          "The k8s event discovery server address");
ABSL_FLAG(
    std::string, receiver_id, "hls-playback",
    "Event and packet receiver ID to keep track of read progress checkpoints.");
ABSL_FLAG(int, metrics_port, 9090, "The port to expose metrics to Prometheus");

namespace visionai {

absl::StatusOr<HLSPlayback::Options> ConstructHLSPlaybackOptions() {
  // TODO: Update the ParseStreamName to return a struct.
  VAI_ASSIGN_OR_RETURN(auto resource_ids,
                   ParseStreamName(absl::GetFlag(FLAGS_stream_name)));
  std::string project_id = resource_ids[0];
  std::string location_id = resource_ids[1];
  std::string cluster_id = resource_ids[2];
  std::string stream_id = resource_ids[3];

  HLSPlayback::Options options;
  options.cluster_selection.set_project_id(project_id);
  options.cluster_selection.set_location_id(location_id);
  options.cluster_selection.set_cluster_id(cluster_id);
  // TODO(b/250697961): Remove the usage of control plane endpoint.
  // Can connect to data plane server endpoint directly.
  options.cluster_selection.set_service_endpoint(
      absl::GetFlag(FLAGS_service_endpoint));
  options.stream_id = stream_id;
  options.receiver_id = absl::GetFlag(FLAGS_receiver_id);
  options.streaming_server_addr =
      absl::GetFlag(FLAGS_k8s_streaming_server_addr);
  options.local_dir = absl::GetFlag(FLAGS_output_dir);
  options.max_files = absl::GetFlag(FLAGS_max_files);
  options.target_duration_in_sec = absl::GetFlag(FLAGS_target_duration);

  return options;
}

}  // namespace visionai

int main(int argc, char** argv) {
  visionai::VisionAIInit();
  absl::SetFlag(&FLAGS_alsologtostderr, true);
  InitGoogle(nullptr, &argc, &argv, /*remove_flags=*/true);
  // Register gstreamer plugins.
  absl::Status status;
  status = visionai::GstRegisterPlugins();
  if (!status.ok()) {
    LOG(ERROR) << "Failed to register the gstreamer plugins: " << status;
    return EXIT_FAILURE;
  }

  // Start the metrics recorder.
  absl::flat_hash_map<std::string, std::string> global_tags;
  std::string metrics_addr =
      absl::StrFormat("0.0.0.0:%d", absl::GetFlag(FLAGS_metrics_port));
  std::unique_ptr<visionai::MetricsRecorder> metrics_recorder =
      std::make_unique<visionai::PrometheusMetricsRecorder>(metrics_addr,
                                                            global_tags);
  std::unique_ptr<visionai::ScopedGlobalMetricsRecorder> metrics_recorder_ =
      std::make_unique<visionai::ScopedGlobalMetricsRecorder>(
          std::move(metrics_recorder));

  // Launch the HLS Playback server.
  auto options = visionai::ConstructHLSPlaybackOptions();
  if (!options.ok()) {
    LOG(ERROR) << "Failed to parse the command options: " << options.status();
    return EXIT_FAILURE;
  }
  visionai::HLSPlayback hls_playback(options.value());
  status = hls_playback.Run();
  if (!status.ok()) {
    LOG(ERROR) << status;
    return EXIT_FAILURE;
  }
  return 0;
}