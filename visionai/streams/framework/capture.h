/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_H_

#include <memory>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/registration.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// CaptureInitContext is the object passed to the `Init` routine to help
// the captures initialize with the user's runtime data.
class CaptureInitContext {
 public:
  struct InitData {
    std::vector<std::string> input_urls;
    absl::flat_hash_map<std::string, AttrValue> attrs;
  };
  explicit CaptureInitContext(InitData d);

  // Gets the input source url from the user.
  absl::Status GetInputUrl(std::string* url) const;

  // Gets the input source urls from the user when there are multiple.
  absl::Status GetInputUrls(std::vector<std::string>* urls) const;

  // Get the value of a specific attribute.
  //
  // If the user supplied a value, then `out` will be overwritten. Otherwise,
  // leave the value of `out` as is. That is, you may initialize `out` to a
  // default value before calling this function.
  template <typename T>
  absl::Status GetAttr(absl::string_view name, T* out) const;

 private:
  std::vector<std::string> input_urls_;
  absl::flat_hash_map<std::string, AttrValue> attrs_;
};

// CaptureRunContext is the object passed to the `Run` routine to supply
// the capture with runtime environment information and data structures.
class CaptureRunContext {
 public:
  struct RunData {
    std::shared_ptr<RingBuffer<Packet>> output_buffer;
  };
  explicit CaptureRunContext(RunData d);

  absl::Status Push(Packet v) {
    output_buffer_->EmplaceFront(std::move(v));
    return absl::OkStatus();
  }

 private:
  std::shared_ptr<RingBuffer<Packet>> output_buffer_;
};

// Capture is the base class for all source captures.
// Derive from it to write your specific source capture.
//
// Example:
// class MyCapture : public Capture {
//   // fill in the blanks.
// };
class Capture {
 public:
  Capture() = default;
  virtual ~Capture() = default;

  // Initializes the Capture.
  //
  // You may perform expensive initializations as this is done upfront.
  virtual absl::Status Init(CaptureInitContext* ctx) = 0;

  // Runs the main loop for data capture.
  //
  // The main custom business logic to capture source data goes here.
  //
  // The caller will block and perform the custom logic until the capture has
  // completed or if `Cancel` is called and fulfilled.
  virtual absl::Status Run(CaptureRunContext* ctx) = 0;

  // Cancels the data capture.
  //
  // This will be called from a different thread to request that the currently
  // active `Run` be cancelled.
  //
  // All Capture developers should have a way to stop a currently in flight
  // `Run`. For example, it is often enough to simply have `Run` periodically
  // check an absl::Notification object that `Cancel` will `Notify`.
  virtual absl::Status Cancel() = 0;
};

// CaptureRegistry is a static global registry that contains functors for
// instantiating instance of specific source captures.
//
// It is currently NOT thread safe.
class CaptureRegistry {
 public:
  typedef std::function<Capture*(void)> CaptureFactory;

  CaptureRegistry() = default;

  // Get an instance of the global registry.
  static CaptureRegistry* Global();

  // Register the CaptureFactory to the given `capture_name`.
  //
  // Note that we are registering the factory itself. The factory will be called
  // to instantiate the actual Capture if it is actually requested at
  // runtime.
  void Register(absl::string_view capture_name, CaptureFactory capture_factory);

  // Create a Capture registered against the given `capture_name`.
  absl::StatusOr<std::unique_ptr<Capture>> CreateCapture(
      absl::string_view capture_name);

 private:
  absl::Status RegisterHelper(absl::string_view capture_name,
                              CaptureFactory capture_factory);

  absl::flat_hash_map<std::string, CaptureFactory> registry_;
};

// To register a new Capture definition:
//
// // Assuming you implemented MyCapture:
// class MyCapture : public Capture {
//   // ...
// };
//
// // Register the class against the corresponding CaptureDef name.
// REGISTER_CAPTURE_IMPLEMENTATION("MyCapture", MyCapture);
#define REGISTER_CAPTURE_IMPLEMENTATION_IMPL(ctr, name, ...)                 \
  static ::visionai::InitOnStartupMarker const register_capture##ctr =       \
      ::visionai::InitOnStartupMarker{} << ([]() {                           \
        ::visionai::CaptureRegistry::Global()->Register(                     \
            name, []() -> ::visionai::Capture* { return new __VA_ARGS__; }); \
        return ::visionai::InitOnStartupMarker{};                            \
      })

#define REGISTER_CAPTURE_IMPLEMENTATION(name, ...) \
  VAI_NEW_ID_FOR_INIT(REGISTER_CAPTURE_IMPLEMENTATION_IMPL, name, __VA_ARGS__)

// -----------------------------------------------------------------------------
// Implementation below; please ignore.

template <typename T>
absl::Status CaptureInitContext::GetAttr(absl::string_view name, T* out) const {
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

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_H_
