// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/storage_exporter.h"

#include "absl/functional/bind_front.h"
#include "absl/synchronization/notification.h"
#include "visionai/streams/apps/util/event_loop_runner.h"
#include "visionai/streams/apps/util/packet_loop_runner.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/plugins/event_writers/warehouse/warehouse_event_writer.h"

namespace visionai {

absl::Status StorageExporter::Export() {
  EventUpdateReceiver::Options event_receiver_opts;
  event_receiver_opts.cluster_selection = options_.cluster_selection;
  if (!options_.streaming_server_addr.empty()) {
    event_receiver_opts.cluster_selection.set_use_insecure_channel(true);
    event_receiver_opts.cluster_selection.set_cluster_endpoint(
        options_.streaming_server_addr);
  }
  event_receiver_opts.stream_id = options_.stream_id;
  event_receiver_opts.receiver = options_.receiver_id;
  event_receiver_opts.starting_logical_offset = "most-recent";
  event_receiver_opts.fallback_starting_offset = "end";

  EventReceiverFactory event_receiver_factory =
      absl::bind_front(EventUpdateReceiver::Create);
  PacketReceiver::Options packet_receiver_opts;
  packet_receiver_opts.cluster_selection = options_.cluster_selection;
  if (!options_.streaming_server_addr.empty()) {
    packet_receiver_opts.cluster_selection.set_use_insecure_channel(true);
    packet_receiver_opts.cluster_selection.set_cluster_endpoint(
        options_.streaming_server_addr);
  }
  packet_receiver_opts.channel.stream_id = options_.stream_id;
  packet_receiver_opts.lessee = options_.receiver_id + "-packet-receiver";
  packet_receiver_opts.receive_mode = "controlled";

  PacketReceiverFactory packet_receiver_factory =
      absl::bind_front(PacketReceiver::Create);

  EventWriterFactory event_writer_factory =
      [=](const std::string& event_id, OffsetCommitCallback commit_callback)
      -> absl::StatusOr<std::shared_ptr<EventWriter>> {
    WarehouseEventWriter::Options event_writer_opts;
    event_writer_opts.warehouse_server_address = options_.mwh_server_addr;
    event_writer_opts.asset_name = options_.asset_name;
    event_writer_opts.stream_id = options_.stream_id;
    event_writer_opts.temp_video_dir = options_.temp_video_dir;
    event_writer_opts.labels.project_id =
        options_.cluster_selection.project_id();
    event_writer_opts.labels.location_id =
        options_.cluster_selection.location_id();
    event_writer_opts.labels.cluster_id =
        options_.cluster_selection.cluster_id();
    event_writer_opts.labels.stream_id = options_.stream_id;
    event_writer_opts.h264_only = options_.h264_only;
    event_writer_opts.h264_mux_only = options_.h264_mux_only;
    event_writer_opts.submission_callback = [=](const VideoPartition& file) {
      commit_callback(file.end_packet_offset);
    };
    std::shared_ptr<EventWriter> event_writer =
        std::make_shared<WarehouseEventWriter>(event_writer_opts);
    VAI_RETURN_IF_ERROR(event_writer->Open(event_id));
    return event_writer;
  };

  EventLoopRunner::Options runner_options;
  runner_options.event_writer_factory = event_writer_factory;
  runner_options.event_receiver_factory = event_receiver_factory;
  runner_options.event_receiver_options = event_receiver_opts;
  runner_options.packet_receiver_factory = packet_receiver_factory;
  runner_options.packet_receiver_options = packet_receiver_opts;
  EventLoopRunner runner(runner_options);
  return runner.Run();
}
}  // namespace visionai