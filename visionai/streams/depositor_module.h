// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_DEPOSITOR_MODULE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_DEPOSITOR_MODULE_H_

#include <memory>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/streams/event_manager.h"
#include "visionai/streams/event_sink.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// The DepositorModule is a facade for the Ingester to run a Depositor.
class DepositorModule {
 public:
  // Construct an instance with the given configurations.
  //
  // Please use the builder methods to configure additional behavior, and run
  // `Initialize` to get a usable instance.
  explicit DepositorModule(const visionai::DepositorConfig&);

  // Attach the input buffer from which to poll for filtered Packets.
  DepositorModule& AttachInput(
      std::shared_ptr<ProducerConsumerQueue<FilteredElement>> input_buffer);

  // Attach an event manager.
  DepositorModule& AttachEventManager(
      std::shared_ptr<EventManager> event_manager);

  // Finalizes the builder configuration and initializes the depositor.
  absl::Status Prepare();

  // Runs the main loop of the depositor.
  //
  // The caller will block until completion or if `Cancel` is called and
  // fulfilled.
  absl::Status Run();

  // Cancels the current depositor `Run`.
  absl::Status Cancel();

  // Copy-control members.
  //
  // Movable, but not Copyable.
  ~DepositorModule() = default;
  DepositorModule(DepositorModule&&) = default;
  DepositorModule& operator=(DepositorModule&&) = default;
  DepositorModule(const DepositorModule&) = delete;
  DepositorModule& operator=(const DepositorModule&) = delete;

 private:
  const DepositorConfig config_;
  absl::Notification is_cancelled_;

  std::shared_ptr<ProducerConsumerQueue<FilteredElement>>
      depositor_input_buffer_ = nullptr;
  std::shared_ptr<EventManager> event_manager_ = nullptr;
  std::shared_ptr<EventSink> event_sink_ = nullptr;

  absl::flat_hash_map<std::string, std::shared_ptr<EventSink>> sinks_;

  absl::Status HandleOpenFilteredElement(FilteredElement f);
  absl::Status HandleCloseFilteredElement(FilteredElement f);
  absl::Status HandlePacketFilteredElement(FilteredElement f);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_DEPOSITOR_MODULE_H_
