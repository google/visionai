// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PACKET_RECEIVER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PACKET_RECEIVER_H_

#include <cstdint>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/channel_lease_renewal_task.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/read_write_channel.h"
#include "visionai/streams/client/streaming_receive_packets_grpc_v1_client.h"
#include "visionai/streams/client/worker_v1.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"

namespace visionai {

// -----------------------------------------------------------------------------
// PacketReceiver
// -----------------------------------------------------------------------------
//
// A `PacketReceiver` reads `Packet`s from a `Channel`.
//
// It handles both the control and data protocols, e.g. lease management and
// data streaming, hiding the complexities of the lower level primitives.
//
// Note that deciding which `Channel` to read from is up to the user of
// `PacketReceiver`. That is, event management is considered a separate
// responsibility.
//
// -----------------------------------------------------------------------------
// Read Modes
//
// The `PacketReceiver` is able to offer several different modes of operations
// through the same programming API in order to meet varying usage needs. We
// discuss each of the modes below and demonstrate how they are used.
//
// * Eager Mode
//
//   In this mode, the user eagerly reads `Packet`s as quickly as possible,
//   always trying to stay up to date with the latest available data.
//
//   When the user begins their reads, they await the next available message
//   from its sender. In other words, they do not care about past data, and are
//   only interested in the future and staying up to date with it. This also
//   applies during process restart/failover; i.e. the reader will jump to the
//   next available message, skipping over any messages that might have arrived
//   while it was away.
//
//   Typically, users of this kind is latency sensitive and may even have a hard
//   real-time requirement to process the latest information.
//
//   Example usage:
//
//     PacketReceiver::Options options;
//     options.reader_mode = "eager";
//     // ... set other options ...
//
//     VAI_ASSIGN_OR_RETURN(auto eager_reader,
//                      PacketReceiver::Create(options));
//     // ... check for errors ...
//
//     while (true) {
//       Packet p;
//       auto status = eager_reader->Read(timeout, &p);
//       // ... check for errors and do something interesting ...
//     }
//
// * Controlled Mode
//
//   A `Channel` may be thought of as a long tape of messages, where the
//   sender only appends to the end and the reader iterates forwards towards
//   more recent messages beginning from a starting point.
//
//   In this mode, the user controls their own read progress along the tape.
//   Such a reader typically iterate forwards any number of times and
//   periodically commits their progress explicitly. After a process
//   restart/failover, the reader will resume just after where the last
//   successful commit was to restore their progress. If no checkpoints exist,
//   or if the last checkpoint is no longer present due to the `Channel`'s TTL
//   policy, then the reader will start fresh from a choice of common starting
//   points.
//
//   It is important to point out that this mode allows the reader to read every
//   message *at least once*. The system may deliver the same message more than
//   once so long as that message is more recent than the most recent
//   checkpoint. While they may not prevent the system from delivering the same
//   message to them more than once, they are free to arrange their own
//   mechansims to prevent multiple processing of the same message; e.g. build
//   in idempotence or use transactions in their downstream system.
//
//   Typically, users of this kind is not as latency sensitive and do not have
//   hard real-time requirements. They care about processing every message at
//   least once and require to retain control over their own read progress.
//
//   Example usage:
//
//     PacketReceiver::Options options;
//     options.reader_mode = "controlled";
//     // ... set other options ...
//
//     VAI_ASSIGN_OR_RETURN(auto controlled_reader,
//                      PacketReceiver::Create(options));
//     // ... check for errors ...
//
//     // Repeatedly read packets until you decide to cancel or if a
//     // read channel is closed.
//     constexpr int commit_every_n = 5;
//     int current_offset = -1;
//     Packet p;
//     for (int i = 1;;) {
//
//       bool did_not_time_out = controlled_reader->Read(timeout, &p, &read_ok);
//
//       // Case 1: Timed out. Just try again.
//       if (!did_not_timeout) {
//         continue;
//       }
//
//       // Case 2: Actually read a new packet. Do something interesting,
//       //         Possibly commit an offset, and then resume reading.
//       if (read_ok) {
//         ++i;
//
//         // Do some processing.
//         current_offset = GetOffset(p);
//         absl::Status s = DoSomethingFun(p);
//
//         // On success, commit the progress once in a while
//         if (s.ok() && i % commit_every_n == 0) {
//           controlled_reader->Commit(current_offset);
//           continue;
//         } else {
//           // Suppose you encounter a failure case that requires quiting.
//           // While you could simply return, it is best to be explicit
//           // about your intentions and cancel your reads and then break
//           // to cancel your writes.
//           controlled_reader->Cancel();
//           break;
//         }
//       }
//
//       // Case 3: You have read all packets that will be available in this
//       //         specific RPC session. You must now break to see why there
//       //         the packet stream ended.
//       break;
//     }
//
//     // You now have a chance to commit your progress for the last time.
//     // Once you do that, you have to explicitly signal you're done.
//     controlled_reader->Commit(current_offset);
//     controlled_reader->CommitsDone();
//
//     // Finally, you should find out why the RPC ended.
//     //
//     // IMPORTANT: If OUT_OF_RANGE, then you actually exhausted all packets
//     // by your upstream producer. Otherwise, there WILL be newer packets
//     // that you haven't read and you should reconnect if you care to read
//     // them.
//     return controlled_reader->Finish();
class PacketReceiver {
 public:
  // Options for configuring the `PacketReceiver`.
  struct Options {
    // The cluster to connect to.
    ClusterSelection cluster_selection;

    // The channel from which data are to be received.
    Channel channel;

    // The name under which resources will be leased.
    // TODO(dschao): Change this to receiver_name.
    std::string lessee;

    // The operation mode to operate under. Valid values are
    // "eager" or "controlled".
    std::string receive_mode = "eager";

    // Advanced options.
    struct Advanced {
      // The amount of time that another PacketReceiver instance is allowed to
      // reconnect under the same `lessee` after an active instance disconnects.
      //
      // A system default will be chosen if not set to a finite positive value.
      absl::Duration grace_period;

      // The duration of silence before which a server heartbeat is expected.
      //
      // A system default will be chosen if not set to a finite positive value.
      absl::Duration heartbeat_interval;

      // A grace period applied on top of the heartbeat interval to accommodate
      // for variations in arrival rates.
      //
      // A system default will be chosen if not set to a finite positive value.
      absl::Duration heartbeat_grace_period;

      // The grace period after which the server will severe the RPC after it
      // issues the final writes done request.
      //
      // The server will choose a default if not set to a finite positive value.
      absl::Duration writes_done_grace_period;

      // The options specific to "controlled" mode.
      struct ControlledModeOptions {
        // This is the where the reader will begin its reads.
        //
        // "begin": Start from the earliest available message.
        // "most-recent": Start from the latest available message.
        // "end": Start from future messages.
        // "stored": Start just after the last commit point, restoring progress.
        std::string starting_logical_offset = "stored";

        // This is the where the reader will begin its reads if no checkpoint is
        // available. Valid values are:
        //
        // "begin": Start from the earliest available message.
        // "end": Start from future messages.
        std::string fallback_starting_offset = "begin";
      };
      ControlledModeOptions controlled_mode_options;
    };

    // Specific advanced settings.
    //
    // This typically does not need to be configured.
    Advanced advanced;
  };

  // Creates and initializes an instance that is ready for use.
  static absl::StatusOr<std::unique_ptr<PacketReceiver>> Create(
      const Options &options);

  // --------------------------------------------------------------------------
  // Eager Mode Methods
  // --------------------------------------------------------------------------

  // Blocks until either:
  //
  // 1. The `timeout` has expired.
  //
  //    - `*packet` left unchanged.
  //    - Returns NOT_FOUND.
  //
  // 2. Successfully received a new `Packet`.
  //
  //    - `*packet` stores the new `Packet`.
  //    - Returns OK.
  //
  // 3. Other return codes indicates that the read stream has ended:
  //
  //    - OUT_OF_RANGE
  //
  //      You exhausted all packets from your upstream producer.
  //      It makes little sense to reconnect.
  //
  //    - CANCELLED
  //
  //      The RPC was cancelled by you or the server.
  //      More packets may still be available. Reconnect to read them.
  //
  //    - DEADLINE_EXCEEDED
  //
  //      This is a regular maintanence severence of a long running RPC.
  //      Reconnect with a new instance to continue.
  //
  //    - Other codes are possible, and it may or may not make sense to
  //      reconnect depending on the specific error detail.
  virtual absl::Status Receive(absl::Duration timeout, Packet *packet);

  // Equivalent to `Receive(absl::Duration, Packet*)` with an
  // `absl::InfiniteDuration` as the timeout.
  virtual absl::Status Receive(Packet *packet);

  // --------------------------------------------------------------------------
  // Controlled Mode Methods
  // --------------------------------------------------------------------------

  // Receive a new `Packet`.
  //
  // The return boolean indicates whether the `timeout` has expired, and `ok`
  // indicates whether the read contains a new `Packet`.
  //
  // Blocks until either:
  //
  // 1. The `timeout` has expired.
  //
  //    - `*packet` left unchanged.
  //    - `*ok` left unchanged.
  //    - Returns `false`.
  //
  // 2. Successfully received a new `Packet`.
  //
  //    - `*packet` stores the new `Packet`.
  //    - `*ok` is set to `true`.
  //    - Returns `true`.
  //
  // 3. The present receive channel has been exhausted.
  //
  //    - `*packet` left unchanged.
  //    - `*ok` is set to `false`.
  //    - Returns `true`.
  //
  // It is the caller's responsibility to ensure that the receive channel is
  // closed before calling `Finish`. This condition has been met if any of the
  // following hold:
  //
  // 1. `Receive` has been called until `ok` stored `false`.
  // 2. `Cancel` has been called.
  //
  // It is important to remember that `*ok` storing `false` only indicates that
  // the current streaming session has been exhausted. It does NOT mean that the
  // all `Packet`s produced by the upstream sender has been exhausted; for this
  // latter condition, `Finish` must return `OUT_OF_RANGE`.
  virtual bool Receive(absl::Duration timeout, Packet *packet, bool *ok);

  // Commit a `Packet`'s `offset`, checkpointing the read progress.
  //
  // The return boolean indicates whether the `timeout` has expired, and `ok`
  // indicates whether the commit channel is open.
  //
  // Blocks until either:
  //
  // 1. The `timeout` has expired.
  //
  //    - `*ok` left unchanged.
  //    - Returns `false`.
  //
  // 2. Successfully submitted a commit request.
  //
  //    - `*ok` is set to `true`.
  //    - Returns `true`.
  //
  // 3. The commit channel is closed.
  //
  //    - `*ok` is set to `false`.
  //    - Returns `true`.
  //
  // It is the caller's responsibility to ensure that the commit channel is
  // closed before calling `Finish`. This condition has been met if any of the
  // following hold:
  //
  // 1. `CommitsDone` has been called.
  // 2. `Commit` has been called until `*ok` is false.
  // 3. `Cancel` has been called.
  //
  // Note that this is merely a request to commit. The actual checkpoint is
  // tracked on the server, and may only be eventually be updated, if at all.
  // The caller must be prepared to handle `Packet`s that have been received
  // should a commit be lost.
  virtual bool Commit(absl::Duration timeout, int64_t offset, bool *ok);

  // Explicitly signals that all `Commit`s have been issued, and closes the
  // commit channel cleanly.
  virtual void CommitsDone();

  // Unilaterally terminates both the read and commit channels. This is a client
  // side RPC cancellation, and may be used as a last resort.
  void Cancel();

  // `Finish` the streaming session, returning a status code indicating the
  // reason of termination.
  //
  // This may be called only when both the `Receive` and `Commit` channels are
  // closed. Calling this prematurely will cause the caller to block until the
  // condition holds. See `Receive` and `Commit` for more information about how
  // to close their channels.
  //
  // Notable return codes:
  //
  // * OUT_OF_RANGE: This indicates that the caller has `Receive`'d all of the
  //   `Packet`s and truly exhausted all messages that the upstream sender has
  //   produced. If the last `Packet`'s offset is committed, then there is no
  //   reason to reconnect to the stream.
  //
  // * DEADLINE_EXCEEDED: This indicates that the present streaming session is
  //   intentionally terminated due to either a client or server enforced
  //   maximum streaming session time. Unlike unary RPCs, this is also
  //   considered normal, as this is a routine maintenance operation to keep the
  //   system healthy.
  //
  // * CANCELLED: This indicates that either the client or the server has
  //   explicitly cancelled the present session for any reason; e.g. network
  //   partition, unreachable party, local application error, or just about any
  //   reason whatsoever.
  //
  // * Other codes are also possible; their meanings are better understood and
  //   the specific message will provide more diagnostics.
  virtual absl::Status Finish();

  virtual ~PacketReceiver();
  PacketReceiver(const PacketReceiver &) = delete;
  PacketReceiver &operator=(const PacketReceiver &) = delete;

 protected:
  explicit PacketReceiver(const Options &);

 private:
  Options options_;

  absl::Status Initialize();

  // --------------------------------------------------------------------------
  // Runtime workers, channels, and their coordination.
  // --------------------------------------------------------------------------

  std::shared_ptr<streams_internal::ReadWriteChannel<absl::Status>>
      main_status_channel_ = nullptr;
  std::shared_ptr<streams_internal::ReadWriteChannel<std::shared_ptr<Packet>>>
      receive_channel_ = nullptr;
  std::shared_ptr<streams_internal::ReadWriteChannel<int64_t>> commit_channel_ =
      nullptr;

  std::shared_ptr<absl::Notification> offset_committer_cancelled_ = nullptr;
  std::shared_ptr<absl::Notification> packet_transferrer_cancelled_ = nullptr;

  std::unique_ptr<streams_internal::WorkerV1> main_;

  absl::Status MainTask(
      streams_internal::Writer<absl::Status> *status_writer,
      streams_internal::Writer<std::shared_ptr<Packet>> *packet_writer,
      streams_internal::Reader<int64_t> *offset_reader);
  absl::Status CancelMainTask();

  absl::Status OffsetCommitterTask(
      streams_internal::Writer<absl::Status> *status_writer,
      streams_internal::Reader<int64_t> *offset_reader);
  absl::Status CancelOffsetCommitterTask();

  absl::Status PacketTransferrerTask(
      streams_internal::Writer<absl::Status> *status_writer,
      streams_internal::Writer<std::shared_ptr<Packet>> *packet_writer);
  absl::Status CancelPacketTransferrerTask();

  // --------------------------------------------------------------------------
  // Application protocol data structures and helpers
  // --------------------------------------------------------------------------

  // General
  absl::Status CompleteOptionsWithDefaults();
  absl::Status ValidateChannelOptions();

  // Streaming grpc client data structures and helpers.
  std::unique_ptr<StreamingReceivePacketsGrpcV1Client> grpc_client_ = nullptr;
  absl::Status CreateGrpcClient();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PACKET_RECEIVER_H_
