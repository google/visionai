// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_STORAGE_EXPORTER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_STORAGE_EXPORTER_H_

#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

class StorageExporter {
 public:
  struct Options {
    // Service endpoints.
    //
    // The streaming service address for in-cluster networking.
    std::string streaming_server_addr;
    // The Media Warehouse server address.
    std::string mwh_server_addr;

    // Entity identifications.
    //
    // The information to identify the cluster.
    ClusterSelection cluster_selection;
    // The stream id.
    std::string stream_id;
    // The receiver_id.
    // The lessee of the EventUpdateReceiver will be "{receiver_id}"" and the
    // lessee of the PacketReceiver will be "{receiver_id}-packet-receiver".
    std::string receiver_id;
    // The MWH asset name to ingest into.
    std::string asset_name;

    // Video export configs.
    //
    // Whether to only export h264 encoded streams.
    bool h264_only;
    // Whether to only mux for the h264 encoded streams.
    bool h264_mux_only;
    // The directory to store the temporary local video files.
    std::string temp_video_dir;
  };

  StorageExporter(const Options& options) : options_(options) {}

  // Run the `StorageExporter` to continuously receive events and
  // packkets from the streaming server, aggregate the packets to mp4 video
  // files, and finally ingest the files to MWH server.
  absl::Status Export();

 private:
  Options options_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_STORAGE_EXPORTER_H_
