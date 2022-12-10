// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_EVENT_UPDATE_RECEIVER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_EVENT_UPDATE_RECEIVER_H_

#include <cstdint>
#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/read_write_channel.h"
#include "visionai/streams/client/streaming_receive_events_grpc_client.h"
#include "visionai/streams/client/worker_v1.h"

namespace visionai {

// -----------------------------------------------------------------------------
// EventUpdateReceiver
// -----------------------------------------------------------------------------
//
// A `EventUpdateReceiver` reads `EventUpdate`s from the Vision AI service.
//
// At present, this is used only to receive newly arrived events on a given
// stream. The user may use this to sense new events and use the
// `PacketReceiver` to then read the data present in them.
//
// You may think of events like files and the packet stream like lines within a
// file. A typical use pattern is to receive events ("files") that you haven't
// read from before, and for each of them, read all the packets ("lines")
// contained within it.
//
// It is helpful to think of the Vision AI service as having a "tape" of events
// that you may read from, and the read head of that tape keeps track of your
// progress. Everytime you have completed the reading of an event, e.g.
// receiving OUT_OF_RANGE using a `PacketReceiver`, you may decide to commit
// your event read progress. If you happen to lose connection, you will be able
// to resume where you last left off, or if that event is already garbage
// collected, resume at a reasonable starting point.
//
//   Example usage:
//
//     EventUpdateReceiver::Options options;
//     options.stream_id = "my-stream";
//     // ... set other options ...
//
//     VAI_ASSIGN_OR_RETURN(auto event_reader,
//                      EventUpdateReceiver::Create(options));
//     // ... check for errors ...
//
//     // Repeatedly read event updates until you decide to cancel or if a
//     // read channel is closed.
//     int current_offset = -1;
//     EventUpdate event_update;
//     for (int i = 1;;) {
//
//       bool did_not_time_out = event_reader->Read(timeout, &event_update,
//                                                  &read_ok);
//
//       // Case 1: Timed out. Just try again.
//       if (!did_not_timeout) {
//         continue;
//       }
//
//       // Case 2: Actually read a new event. Do something interesting,
//       //         Possibly commit an offset, and then resume reading.
//       if (read_ok) {
//         ++i;
//
//         // Do some processing.
//         //
//         // For example, DoSomethingFun could be to run a PacketReceiver
//         // inner loop in controlled mode.
//         current_offset = GetOffset(event_update);
//         absl::Status s = DoSomethingFun(event_update);
//
//         // On success, commit the progress.
//         if (s.ok()) {
//           event_reader->Commit(current_offset);
//           continue;
//         } else {
//           // Suppose you encounter a failure case that requires quiting.
//           // While you could simply return, it is best to be explicit
//           // about your intentions and cancel your reads and then break
//           // to cancel your writes.
//           event_reader->Cancel();
//           break;
//         }
//       }
//
//       // Case 3: You have read all events that will be available in this
//       //         specific RPC session. You must now break to see why the
//       //         event stream ended.
//       break;
//     }
//
//     // You now have a chance to commit your progress for the last time.
//     // Once you do that, you have to explicitly signal you're done.
//     event_reader->Commit(current_offset);
//     event_reader->CommitsDone();
//
//     // Finally, you should find out why the RPC ended.
//     //
//     // IMPORTANT: If OUT_OF_RANGE, then you actually exhausted all events.
//     // by your upstream producer. Otherwise, there WILL be newer events.
//     // that you haven't read and you should reconnect if you care to read
//     // them.
//     return event_reader->Finish();
class EventUpdateReceiver {
 public:
  // Options for configuring the `EventUpdateReceiver`.
  struct Options {
    // The cluster to connect to.
    ClusterSelection cluster_selection;

    // The stream from which event updates are to be read.
    std::string stream_id;

    // A name for the receiver to self identify.
    //
    // This is used to keep track of read progress checkpoints in the server.
    std::string receiver;

    // This is the where the reader will begin its reads.
    // Valid values are:
    //
    // "begin": Start from the earliest available message.
    //
    // "most-recent": Start from the most recently available message.
    //
    // "end": Start from future messages.
    //
    // "stored": This will resume your read progress; i.e. one past the last
    //           committed offset.
    std::string starting_logical_offset = "stored";

    // This is the where the reader will begin its reads if the specified
    // starting logical offset is not avaialble. Valid values are:
    //
    // "begin": Start from the earliest available message.
    // "end": Start from future messages.
    std::string fallback_starting_offset = "begin";

    // Advanced options.
    struct Advanced {
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
    };

    // Specific advanced settings.
    //
    // This typically does not need to be configured.
    Advanced advanced;
  };

  // Creates and initializes an instance that is ready for use.
  static absl::StatusOr<std::unique_ptr<EventUpdateReceiver>> Create(
      const Options &options);

  // Receive a new `EventUpdate`.
  //
  // The return boolean indicates whether the `timeout` has expired, and `ok`
  // indicates whether the read contains a new `EventUpdate`.
  //
  // Blocks until either:
  //
  // 1. The `timeout` has expired.
  //
  //    - `*event_update` left unchanged.
  //    - `*ok` left unchanged.
  //    - Returns `false`.
  //
  // 2. Successfully received a new `EventUpdate`.
  //
  //    - `*event_update` stores the new `EventUpdate`.
  //    - `*ok` is set to `true`.
  //    - Returns `true`.
  //
  // 3. The present receive channel has been exhausted.
  //
  //    - `*event_update` left unchanged.
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
  // all `EventUpdate`s produced by the upstream sender has been exhausted; for
  // this latter condition, `Finish` must return `OUT_OF_RANGE`.
  virtual bool Receive(absl::Duration timeout, EventUpdate *event_update,
                       bool *ok);

  // Commit a `EventUpdate`'s `offset`, checkpointing the read progress.
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
  // The caller must be prepared to handle `EventUpdate`s that have been
  // received should a commit be lost.
  virtual bool Commit(absl::Duration timeout, int64_t offset, bool *ok);

  // Explicitly signals that all `Commit`s have been issued, and closes the
  // commit channel cleanly.
  virtual void CommitsDone();

  // Unilaterally terminates both the read and commit channels. This is a client
  // side RPC cancellation, and may be used as a last resort.
  virtual void Cancel();

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
  //   `EventUpdate`s and truly exhausted all messages that the upstream sender
  //   has produced. If the last `EventUpdate`'s offset is committed, then there
  //   is no reason to reconnect to the stream.
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

  virtual ~EventUpdateReceiver();
  EventUpdateReceiver(const EventUpdateReceiver &) = delete;
  EventUpdateReceiver &operator=(const EventUpdateReceiver &) = delete;

 protected:
  explicit EventUpdateReceiver(const Options &);

 private:
  Options options_;
  absl::Status Initialize();

  // --------------------------------------------------------------------------
  // Runtime workers, channels, and their coordination.
  // --------------------------------------------------------------------------

  std::shared_ptr<streams_internal::ReadWriteChannel<absl::Status>>
      main_status_channel_ = nullptr;
  std::shared_ptr<
      streams_internal::ReadWriteChannel<std::shared_ptr<EventUpdate>>>
      receive_channel_ = nullptr;
  std::shared_ptr<streams_internal::ReadWriteChannel<int64_t>> commit_channel_ =
      nullptr;

  std::shared_ptr<absl::Notification> offset_committer_cancelled_ = nullptr;
  std::shared_ptr<absl::Notification> event_update_transferrer_cancelled_ =
      nullptr;

  std::unique_ptr<streams_internal::WorkerV1> main_;

  absl::Status MainTask(streams_internal::Writer<absl::Status> *status_writer,
                        streams_internal::Writer<std::shared_ptr<EventUpdate>>
                            *event_update_writer,
                        streams_internal::Reader<int64_t> *offset_reader);
  absl::Status CancelMainTask();

  absl::Status OffsetCommitterTask(
      streams_internal::Writer<absl::Status> *status_writer,
      streams_internal::Reader<int64_t> *offset_reader);
  absl::Status CancelOffsetCommitterTask();

  absl::Status EventUpdateTransferrerTask(
      streams_internal::Writer<absl::Status> *status_writer,
      streams_internal::Writer<std::shared_ptr<EventUpdate>>
          *event_update_writer);
  absl::Status CancelEventUpdateTransferrerTask();

  // --------------------------------------------------------------------------
  // Application protocol data structures and helpers
  // --------------------------------------------------------------------------

  // General
  absl::Status CompleteOptionsWithDefaults();

  // Streaming grpc client data structures and helpers.
  std::unique_ptr<StreamingReceiveEventsGrpcClient> grpc_client_ = nullptr;
  absl::Status CreateGrpcClient();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_EVENT_UPDATE_RECEIVER_H_
