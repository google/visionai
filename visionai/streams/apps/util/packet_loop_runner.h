// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_UTIL_PACKET_LOOP_RUNNER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_UTIL_PACKET_LOOP_RUNNER_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/notification.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

using OffsetCommitCallback = std::function<void(int64_t)>;

using PacketReceiverFactory =
    std::function<absl::StatusOr<std::shared_ptr<PacketReceiver>>(
        const PacketReceiver::Options& options)>;

using EventWriterFactory =
    std::function<absl::StatusOr<std::shared_ptr<EventWriter>>(
        const std::string& event_id, OffsetCommitCallback callback)>;

// `PacketLoopRunner` repetitively reads packets from `PacketReceiver` and feed
// the packets to `EventWriter` for further processing.
//
// --------------------------
// Receiving Packets
// --------------------------
// The `PacketLoopRunner` creates an instance of `PacketReceiver` with the given
// factory method and the option. Some possible behaviours and outcomes while
// receiving the packets:
//
// (1) Success:
//      Feed the packet to the `EventWriter` pipeline.
// (2) Timeout:
//      Continue to poll the packets with the same `PacketReceiver` instance.
// (3) Finish with `absl::OutOfRange status`:
//      All packets have been received. Commit the event.
//      `PacketLoopRunner::Run()` returns OK.
// (4) Finish with other error status:
//      Create another `PacketReceiver` instance and retry.
//
// --------------------------
// Packet Receiving Mode
// --------------------------
// The PacketReceiver can operate under two distinct modes, the Eager Mode and
// the Controlled Mode.
//
// In the Eager Mode, the application always receive the latest packet when it
// starts to receive packets in a gRPC stream.
//
// In the Controlled Mode, the application can receive the packet from the last
// commit offset. The application has to commit the packet offset it has
// successfully processed, otherwise the uncommitted packets will be redelivered
// upon gRPC restart.
//
// See visionai/streams/client/packet_receiver.h for a more thorough
// description of the receiver modes.
//
// --------------------------
// Committing Packets
// --------------------------
// The function `PacketLoopRunner::CommitPacketOffset()` is passed to the
// EventWriter, and the EventWriter can decide when to invoke this callback
// to commit a particular packet offset.
//
// The packet commit is conducted as a fire-and-forget operation. We only
// log the error encountered in the commit process.
//
// If the packet commit failed because the underlying gRPC stream terminated,
// the main runner can identify the issue with `PacketReceiver::Receive()` and
// retry.
//
// The `PacketLoopRunner` also keeps track of the `local_commit_offset_` to
// deduplicate the packets in the case of commit failures.
//
class PacketLoopRunner {
 public:
  struct Options {
    // The current event to process.
    EventUpdate current_event;

    // The factory to create the `PacketReceiver` instances.
    PacketReceiverFactory packet_receiver_factory;

    // The options to create the `PacketReceiver` instances.
    PacketReceiver::Options packet_receiver_options;

    // The factory to create the `EventWriter` instance.
    EventWriterFactory event_writer_factory;

    // The event commit callback to be evoked at the end of the processing.
    OffsetCommitCallback event_commit_callback;
  };

  PacketLoopRunner(const Options& options)
      : options_(options), is_canceled_(false) {}

  // Run the loop to process the packets of the event until `Cancel` is called
  // or all the packets from the event have been processed.
  absl::Status Run();

  // Cancel the runner.
  void Cancel() { is_canceled_.Notify(); }

 private:
  Options options_;
  absl::Notification is_canceled_;

  std::shared_ptr<PacketReceiver> packet_receiver_ = nullptr;
  std::shared_ptr<EventWriter> event_writer_ = nullptr;
  absl::Mutex local_commit_offset_mu_;
  int64_t local_commit_offset_ = -1;

  // Operate the PacketReceiver loop under the controlled mode.
  absl::Status RunControlledMode();

  // Operate the PacketReceiver loop under the eager mode.
  absl::Status RunEagerMode();

  void CommitPacketOffset(int64_t offset);

  void OnReceivePacket(Packet p);
  void Finalize();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_UTIL_PACKET_LOOP_RUNNER_H_
