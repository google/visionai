// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_FILTER_MODULE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_FILTER_MODULE_H_

#include <memory>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "visionai/streams/event_manager.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// The FilterModule is a facade for the Ingester to run a Filter. It contains
// the specific Filter requested, as well as all the environment information
// necessary for it to run.
class FilterModule {
 public:
  // Construct an instance with the given configurations.
  //
  // Please use the builder methods to configure additional behavior, and run
  // `Prepare` to get a usable instance.
  explicit FilterModule(const visionai::FilterConfig&);

  // Attach the input buffer from which to poll for filter inputs.
  FilterModule& AttachInput(std::shared_ptr<RingBuffer<Packet>> input_buffer);

  // Attach the output buffer to which to push filtered outputs.
  FilterModule& AttachOutput(
      std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer);

  // Attach an event manager.
  FilterModule& AttachEventManager(std::shared_ptr<EventManager> event_manager);

  // Finalizes the builder configuration and initializes the filter module.
  //
  // This method initializes the FilterModule itself. It must be called before
  // executing the Filter life-cycle routines.
  absl::Status Prepare();

  // Copy-control members.
  //
  // Movable, but not Copyable.
  ~FilterModule() = default;
  FilterModule(FilterModule&&) = default;
  FilterModule& operator=(FilterModule&&) = default;
  FilterModule(const FilterModule&) = delete;
  FilterModule& operator=(const FilterModule&) = delete;

  // Filter life-cycle routines
  // ---------------------------

  // Phase 1: Inits the Filter.
  absl::Status Init();

  // Phase 2: Runs the Filter.
  absl::Status Run();

  // Phase 3: Cancels the Filter.
  absl::Status Cancel();

 private:
  const FilterConfig config_;

  std::shared_ptr<RingBuffer<Packet>> filter_input_buffer_ = nullptr;
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>>
      filter_output_buffer_ = nullptr;
  std::shared_ptr<EventManager> event_manager_ = nullptr;

  std::unique_ptr<FilterInitContext> filter_init_ctx_ = nullptr;
  std::unique_ptr<FilterRunContext> filter_run_ctx_ = nullptr;
  std::unique_ptr<Filter> filter_ = nullptr;

  absl::StatusOr<std::unique_ptr<FilterInitContext>> CreateFilterInitContext();
  absl::StatusOr<std::unique_ptr<FilterRunContext>> CreateFilterRunContext();
  absl::StatusOr<std::unique_ptr<Filter>> CreateFilter();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_FILTER_MODULE_H_
