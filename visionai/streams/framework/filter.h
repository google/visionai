// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_FILTER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_FILTER_H_

#include <memory>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/streams/event_manager.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/registration.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// FilterInitContext is the object passed to the `Init` routine to give
// the filters context from which to initialize itself.
class FilterInitContext {
 public:
  struct InitData {
    absl::flat_hash_map<std::string, AttrValue> attrs;
  };
  explicit FilterInitContext(InitData d);

  // Get the value of a specific attribute.
  //
  // If the user supplied a value, then `out` will be overwritten. Otherwise,
  // leave the value of `out` as is. That is, you may initialize `out` to a
  // default value before calling this function.
  template <typename T>
  absl::Status GetAttr(absl::string_view name, T* out) const;

 private:
  absl::flat_hash_map<std::string, AttrValue> attrs_;
};

// FilterRunContext is the object passed to the `Run` routine to supply the
// filter with specific runtime environment information and data structures.
class FilterRunContext {
 public:
  struct RunData {
    std::shared_ptr<RingBuffer<Packet>> input_buffer;
    std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer;
    std::shared_ptr<EventManager> event_manager;
  };
  explicit FilterRunContext(RunData d);

  // Start a (new) event.
  //
  // Returns the id that new event corresponds to.
  // It may then be used as an argument to `Push`.
  absl::StatusOr<std::string> StartEvent();

  // End an event.
  //
  // Further calls to `Push`'s using the given event id will be rejected.
  absl::Status EndEvent(absl::string_view event_id);

  // Poll for a new Packet coming from upstream for up to `timeout`;
  //
  // Returns OK on success, with `p` containing the new Packet.
  // Returns UNAVAILABLE if `timeout` expires.
  // Other codes indicate failure.
  absl::Status Poll(Packet* p, absl::Duration timeout);

  // Push a Packet downstream, into the given `event_id`.
  //
  // Returns OK on success.
  absl::Status Push(absl::string_view event_id, Packet p);

 private:
  std::shared_ptr<RingBuffer<Packet>> input_buffer_;
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer_;
  std::shared_ptr<EventManager> event_manager_;
};

// Filter is the base class for all source filters.
// Derive from it to write your specific source filter.
//
// Example:
// class MyFilter : public Filter {
//   // fill in the blanks.
// };
class Filter {
 public:
  Filter() = default;
  virtual ~Filter() = default;

  // Initializes the Filter.
  //
  // You may perform expensive initializations as this is done upfront.
  virtual absl::Status Init(FilterInitContext* ctx) = 0;

  // Runs the main loop for data filtering.
  //
  // The main custom business logic to filter/transform source data goes here.
  //
  // The caller will block and perform the custom logic until the filter has
  // completed or if `Cancel` is called and fulfilled.
  virtual absl::Status Run(FilterRunContext* ctx) = 0;

  // Cancels the data filter.
  //
  // This will be called from a different thread to request that the currently
  // active `Run` be cancelled.
  //
  // All Filter developers should have a way to stop a currently in flight
  // `Run`. For example, it is often enough to simply have `Run` periodically
  // check an absl::Notification object that `Cancel` will `Notify`.
  virtual absl::Status Cancel() = 0;
};

// FilterRegistry is a static global registry that contains functors for
// instantiating instances of specific source filters.
//
// It is currently NOT thread safe.
class FilterRegistry {
 public:
  typedef std::function<Filter*(void)> FilterFactory;

  FilterRegistry() = default;

  // Get an instance of the global registry.
  static FilterRegistry* Global();

  // Register the FilterFactory to the given `filter_name`.
  //
  // Note that we are registering the factory itself. The factory will be called
  // to instantiate the actual Filter if it is actually requested at
  // runtime.
  void Register(absl::string_view filter_name, FilterFactory filter_factory);

  // Create a Filter registered against the given `filter_name`.
  absl::StatusOr<std::unique_ptr<Filter>> CreateFilter(
      absl::string_view filter_name);

 private:
  absl::Status RegisterHelper(absl::string_view filter_name,
                              FilterFactory filter_factory);

  absl::flat_hash_map<std::string, FilterFactory> registry_;
};

// To register a new Filter definition:
//
// // Assuming you implemented MyFilter:
// class MyFilter : public Filter {
//   // ...
// };
//
// // Register the class against the corresponding FilterDef name.
// REGISTER_FILTER_IMPLEMENTATION("MyFilter", MyFilter);
#define REGISTER_FILTER_IMPLEMENTATION_IMPL(ctr, name, ...)                 \
  static ::visionai::InitOnStartupMarker const register_filter##ctr =       \
      ::visionai::InitOnStartupMarker{} << ([]() {                          \
        ::visionai::FilterRegistry::Global()->Register(                     \
            name, []() -> ::visionai::Filter* { return new __VA_ARGS__; }); \
        return ::visionai::InitOnStartupMarker{};                           \
      })

#define REGISTER_FILTER_IMPLEMENTATION(name, ...) \
  VAI_NEW_ID_FOR_INIT(REGISTER_FILTER_IMPLEMENTATION_IMPL, name, __VA_ARGS__)

// -----------------------------------------------------------------------------
// Implementation below; please ignore.

template <typename T>
absl::Status FilterInitContext::GetAttr(absl::string_view name, T* out) const {
  auto it = attrs_.find(name);
  if (it == attrs_.end()) {
    return absl::OkStatus();
  }
  auto value = GetAttrValue<T>(it->second);
  if (!value.ok()) {
    return value.status();
  }
  *out = *value;
  return absl::OkStatus();
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_FILTER_H_
