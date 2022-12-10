// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_STREAMS_INGESTER_H_
#define VISIONAI_STREAMS_INGESTER_H_

#include <functional>
#include <memory>
#include <thread>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "visionai/streams/capture_module.h"
#include "visionai/streams/depositor_module.h"
#include "visionai/streams/event_manager.h"
#include "visionai/streams/filter_module.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// An Ingester is the top level object that manages the ingestion dataflow.
//
// Its behavior can be configured through the top level configuration
// message `IngesterConfig`. Please see its schema for more detailed
// descriptions.
class Ingester {
 public:
  // Construct an instance with the given configurations.
  //
  // This only constructs the object. `Initialize` must be called to get a
  // usable instance.
  explicit Ingester(const IngesterConfig&);

  // Prepare the ingester.
  //
  // This must be called before calling `Start`.
  absl::Status Prepare();

  // Run the the main ingester dataflow.
  //
  // Blocks the calling thread and returns either when the dataflow has
  // completed or after the request for cancellation takes effect.
  //
  // Returns the reason (within the status) for why the dataflow halted. Either:
  // + OK: Normal termination, or if a user requested cancellation took effect.
  // + Other failures: Abnormal halting; worth further examination.
  absl::Status Run();

  // Request the running dataflow to be cancelled for early stopping.
  absl::Status Cancel();

  // Copy-control members.
  //
  // Movable, but not Copyable.
  ~Ingester();
  Ingester(Ingester&&) = default;
  Ingester& operator=(Ingester&&) = default;
  Ingester(const Ingester&) = delete;
  Ingester& operator=(const Ingester&) = delete;

 private:
  struct StopContext {
    std::string name;
    streams_internal::Worker* worker;
    absl::Duration finalize_timeout;
    std::function<absl::Status(void)> task_canceller;
    absl::Status worker_return_status = absl::OkStatus();
  };

  const IngesterConfig config_;
  absl::Notification is_cancelled_;

  std::shared_ptr<RingBuffer<Packet>> capture_output_buffer_ = nullptr;
  std::shared_ptr<CaptureModule> capture_module_ = nullptr;
  std::unique_ptr<streams_internal::Worker> capture_worker_;

  std::shared_ptr<ProducerConsumerQueue<FilteredElement>>
      filter_output_buffer_ = nullptr;
  std::shared_ptr<FilterModule> filter_module_ = nullptr;
  std::unique_ptr<streams_internal::Worker> filter_worker_;

  std::shared_ptr<DepositorModule> depositor_module_ = nullptr;
  std::unique_ptr<streams_internal::Worker> depositor_worker_;

  std::shared_ptr<EventManager> event_manager_ = nullptr;

  absl::Status CreateModules();
  absl::Status CreateAndAttachInterModuleBuffers();
  absl::Status CreateAndAttachEventManager();
  absl::Status PrepareModules();
  absl::Status StartModules();
  absl::Status StopModules();
};

}  // namespace visionai

#endif  // VISIONAI_STREAMS_INGESTER_H_
