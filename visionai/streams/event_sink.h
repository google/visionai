// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_EVENT_SINK_H_
#define THIRD_PARTY_VISIONAI_STREAMS_EVENT_SINK_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/notification.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"
#include "visionai/util/producer_consumer_queue.h"

namespace visionai {

// The EventSink enable its user to write a Packet using an EventWriter kind of
// their choosing.
//
// It exposes an aynchronous non-blocking write interface, so that the EventSink
// user can itself sustain a high write request throughput, dispatching
// the actual write to a separate thread contained within the EventSink.
//
// On `Create`, `Write` will immediately become available for the user to buffer
// writes. The actual writes into the destination medium will commence as soon
// as the `EventWriter` has gone through its own Creation and Opening phases,
// which in some cases, can take some time. This interface is to enable
// steady-state writes to have very low latency, with only the initial packets
// possibly experience some initial delay. Even then, the user has the option to
// pre-create the EventSink to mitigate the effects of the initial delay.
class EventSink {
 public:
  // Options for configuring the event sink.
  struct Options {
    // The event id to write to.
    std::string event_id;

    // The capacity of the write buffer.
    //
    // A system default will be chosen if set to 0.
    size_t write_buffer_capacity = 0;

    // The kind of event writer to use for this sink.
    EventWriterConfig event_writer_config;
  };

  // Creates and initializes an instance that is ready for use.
  static absl::StatusOr<std::shared_ptr<EventSink>> Create(const Options &);

  // Write a packet into the sink.
  //
  // This is an asynchronous buffered write and returns immediately.
  absl::Status Write(Packet p);

  // Signal that there will be no more writes.
  void Close();

  // Copy-control. Please use Create to generate new instances of this class.
  explicit EventSink(const Options &);
  ~EventSink();
  EventSink(const EventSink &) = delete;
  EventSink &operator=(const EventSink &) = delete;

 private:
  const Options options_;

  std::shared_ptr<ProducerConsumerQueue<Packet>> write_buffer_ = nullptr;
  std::unique_ptr<streams_internal::Worker> worker_ = nullptr;
  absl::Notification close_signal_;

  absl::Status Initialize();

  struct MainContext {
    std::unique_ptr<EventWriter> writer;
    std::unique_ptr<EventWriterInitContext> init_ctx;
    std::string event_id;
  };
  absl::Status WriterMain(const MainContext &);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_EVENT_SINK_H_
