// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "base/init_google.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
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

namespace visionai {
absl::StatusOr<HLSPlayback::Options> ConstructHLSPlaybackOptions() {
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
  std::unique_ptr<visionai::MetricsRecorder> pms_metrics_recorder =
      std::make_unique<visionai::PrometheusMetricsRecorder>("0.0.0.0:9090",
                                                            global_tags);
  std::unique_ptr<visionai::ScopedGlobalMetricsRecorder> metrics_recorder_ =
      std::make_unique<visionai::ScopedGlobalMetricsRecorder>(
          std::move(pms_metrics_recorder));

  // Launch the storage exporter.
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