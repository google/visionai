// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <cstdlib>
#include <memory>

#include "base/commandlineflags.h"
#include "base/init_google.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/storage_exporter.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/flags.h"
#include "visionai/util/telemetry/metrics/metrics_recorder.h"
#include "visionai/util/telemetry/metrics/prometheus_metrics_recorder.h"
#include "visionai/util/status/status_macros.h"

ABSL_FLAG(std::string, stream_name, "", "Stream Name");
ABSL_FLAG(std::string, asset_name, "", "Media Warehouse Asset Name.");
ABSL_FLAG(
    std::string, receiver_id, "storage-exporter",
    "Event and packet receiver ID to keep track of read progress checkpoints.");
ABSL_FLAG(std::string, mwh_server_address, "",
          "Media Warehouse Server Address");
ABSL_FLAG(std::string, ais_service_endpoint, "",
          "AIS Control Plane Service Endpoint");
ABSL_FLAG(std::string, k8s_streaming_server_addr, "",
          "The k8s streaming server address");
ABSL_FLAG(int, metrics_port, 9090, "The port to expose metrics to Prometheus");

ABSL_FLAG(std::string, temp_video_dir, "/tmp/storage-exporter",
          "The output dir to temporarily store the fragmented video files");

ABSL_FLAG(bool, h264_only, false, "Whether to reject non-h264 streams");
ABSL_FLAG(bool, h264_mux_only, true,
          "Whether to remove the transcoding for h264 streams");

namespace visionai {

absl::StatusOr<StorageExporter::Options> ConstructStorageExporterOptions() {
  VAI_ASSIGN_OR_RETURN(auto resource_ids,
                   ParseStreamName(absl::GetFlag(FLAGS_stream_name)));
  std::string project_id = resource_ids[0];
  std::string location_id = resource_ids[1];
  std::string cluster_id = resource_ids[2];
  std::string stream_id = resource_ids[3];

  StorageExporter::Options options;
  options.cluster_selection.set_project_id(project_id);
  options.cluster_selection.set_location_id(location_id);
  options.cluster_selection.set_cluster_id(cluster_id);
  options.cluster_selection.set_service_endpoint(
      absl::GetFlag(FLAGS_ais_service_endpoint));
  options.stream_id = stream_id;
  options.receiver_id = absl::GetFlag(FLAGS_receiver_id);
  options.asset_name = absl::GetFlag(FLAGS_asset_name);
  options.mwh_server_addr = absl::GetFlag(FLAGS_mwh_server_address);
  options.streaming_server_addr =
      absl::GetFlag(FLAGS_k8s_streaming_server_addr);
  options.temp_video_dir = absl::GetFlag(FLAGS_temp_video_dir);
  options.h264_only = absl::GetFlag(FLAGS_h264_only);
  options.h264_mux_only = absl::GetFlag(FLAGS_h264_mux_only);
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

  // Launch the storage exporter.
  auto options = visionai::ConstructStorageExporterOptions();
  if (!options.ok()) {
    LOG(ERROR) << "Failed to parse the command options: " << options.status();
    return EXIT_FAILURE;
  }
  visionai::StorageExporter exporter(options.value());
  status = exporter.Export();
  if (!status.ok()) {
    LOG(ERROR) << status;
    return EXIT_FAILURE;
  }
  return 0;
}
