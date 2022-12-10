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

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_HLS_EVENT_WRITER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_HLS_EVENT_WRITER_H_

#include "absl/status/status.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/hls/hls_stats_recorder.h"
#include "visionai/util/telemetry/metrics/labels.h"

namespace visionai {

// HLSEventWriter creates video segments and playlists for HLS protocol.
class HLSEventWriter : public EventWriter {
 public:
  struct Options {
    // The local directory to store the HLS playlist and video segments.
    // The directory should exist.
    std::string local_dir;
    // The maximum number of video files at any time.
    int max_files = 5;
    // The target duration of each video segment.
    int target_duration_in_sec = 2;
    // The stream resource labels for recording metrics.
    MetricStreamResourceLabels labels;
  };

  HLSEventWriter() {}

  // No need to call Init() if the class is contructed by the options.
  HLSEventWriter(const Options& options) : options_(options) {}

  ~HLSEventWriter() override {}

  // Initialize the EventWriter with EventWriterInitConext.
  absl::Status Init(EventWriterInitContext* ctx) override;

  // Open the EventWriter for the new event.
  absl::Status Open(absl::string_view event_id) override;

  // Write a packet.
  absl::Status Write(Packet p) override;

  // Close the current event.
  absl::Status Close() override;

 private:
  Options options_;
  std::string event_id_;
  std::unique_ptr<streams_internal::HLSStatsRecorder> stats_reporter_;
  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;

  std::string PipelineString();

  absl::Status CreateMasterPlaylist();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_HLS_EVENT_WRITER_H_
