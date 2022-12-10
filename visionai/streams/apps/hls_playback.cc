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

#include "visionai/streams/apps/hls_playback.h"

#include <memory>

#include "absl/functional/bind_front.h"
#include "visionai/streams/apps/util/event_loop_runner.h"
#include "visionai/streams/apps/util/packet_loop_runner.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/plugins/event_writers/hls_event_writer.h"

namespace visionai {

absl::Status HLSPlayback::Run() {
  EventUpdateReceiver::Options event_update_receiver_options;
  event_update_receiver_options.cluster_selection = options_.cluster_selection;
  if (!options_.streaming_server_addr.empty()) {
    event_update_receiver_options.cluster_selection.set_use_insecure_channel(
        true);
    event_update_receiver_options.cluster_selection.set_cluster_endpoint(
        options_.streaming_server_addr);
  }
  event_update_receiver_options.stream_id = options_.stream_id;
  event_update_receiver_options.receiver = options_.receiver_id;
  event_update_receiver_options.starting_logical_offset = "most-recent";
  event_update_receiver_options.fallback_starting_offset = "end";

  EventReceiverFactory event_receiver_factory =
      absl::bind_front(EventUpdateReceiver::Create);

  PacketReceiver::Options packet_receiver_options;
  packet_receiver_options.cluster_selection = options_.cluster_selection;
  if (!options_.streaming_server_addr.empty()) {
    packet_receiver_options.cluster_selection.set_use_insecure_channel(true);
    packet_receiver_options.cluster_selection.set_cluster_endpoint(
        options_.streaming_server_addr);
  }
  packet_receiver_options.channel.stream_id = options_.stream_id;
  packet_receiver_options.lessee =
      absl::StrCat(options_.receiver_id, "-packet-receiver");
  packet_receiver_options.receive_mode = "eager";

  PacketReceiverFactory packet_receiver_factory =
      absl::bind_front(PacketReceiver::Create);

  HLSEventWriter::Options event_writer_options;
  event_writer_options.local_dir = options_.local_dir;
  event_writer_options.max_files = options_.max_files;
  event_writer_options.target_duration_in_sec = options_.target_duration_in_sec;
  event_writer_options.labels.project_id =
      options_.cluster_selection.project_id();
  event_writer_options.labels.location_id =
      options_.cluster_selection.location_id();
  event_writer_options.labels.cluster_id =
      options_.cluster_selection.cluster_id();
  event_writer_options.labels.stream_id = options_.stream_id;
  EventWriterFactory event_writer_factory =
      [=](const std::string& event_id, OffsetCommitCallback commit_callback)
      -> absl::StatusOr<std::shared_ptr<EventWriter>> {
    auto event_writer = CreateEventWriter(event_writer_options);
    VAI_RETURN_IF_ERROR(event_writer->Open(event_id));
    return event_writer;
  };

  EventLoopRunner::Options runner_options;
  runner_options.event_writer_factory = event_writer_factory;
  runner_options.event_receiver_factory = event_receiver_factory;
  runner_options.event_receiver_options = event_update_receiver_options;
  runner_options.packet_receiver_factory = packet_receiver_factory;
  runner_options.packet_receiver_options = packet_receiver_options;
  auto runner = CreateEventLoopRunner(runner_options);
  return runner->Run();
}

std::unique_ptr<EventLoopRunner> HLSPlayback::CreateEventLoopRunner(
    EventLoopRunner::Options options) {
  return std::make_unique<EventLoopRunner>(options);
}

std::shared_ptr<HLSEventWriter> HLSPlayback::CreateEventWriter(
    HLSEventWriter::Options options) {
  return std::make_shared<HLSEventWriter>(options);
}

}  // namespace visionai