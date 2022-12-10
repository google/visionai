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

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_HLS_PLAYBACK_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_HLS_PLAYBACK_H_

#include <string>

#include "absl/status/status.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/util/event_loop_runner.h"
#include "visionai/streams/plugins/event_writers/hls_event_writer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

class HLSPlayback {
 public:
  struct Options {
    // Service endpoints.
    //
    // The streaming service address for in-cluster networking.
    std::string streaming_server_addr;

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

    // HLS Configs.
    //
    // The local directory to store the HLS playlist and video segments.
    std::string local_dir;
    // The maximum number of video files at any time.
    int max_files;
    // The target duration of each video segment.
    int target_duration_in_sec;
  };

  HLSPlayback(const Options& options) : options_(options) {}
  virtual ~HLSPlayback() = default;

  // Run the HLS Playback application.
  absl::Status Run();

 private:
  Options options_;

  virtual std::unique_ptr<EventLoopRunner> CreateEventLoopRunner(
      EventLoopRunner::Options);

  virtual std::shared_ptr<HLSEventWriter> CreateEventWriter(
      HLSEventWriter::Options);
};
}  // namespace visionai
#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_HLS_PLAYBACK_H_
