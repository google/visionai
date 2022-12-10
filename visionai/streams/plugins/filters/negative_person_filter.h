// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_NEGATIVE_PERSON_FILTER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_NEGATIVE_PERSON_FILTER_H_

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/detection/object_detection/object_detector.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"

namespace visionai {
// Runtime phases of negative person filter module:
//
// Init(): Called from the main thread to initialize the module.
// Run(): Called from a worker thread to actually capture data.
// Cancel(): Called from the main thread to stop the worker thread.
class NegativePersonFilter : public Filter {
 public:
  NegativePersonFilter() = default;
  ~NegativePersonFilter() override = default;

  // Initialize the filter module.
  absl::Status Init(FilterInitContext* ctx) override;

  // Main business logic to capture the source data.
  //
  // This runs asynchronously in a background worker thread.
  absl::Status Run(FilterRunContext* ctx) override;

  // Cancels the filter.
  //
  // This will be called from a different thread to request that the currently
  // active `Run` be cancelled.
  absl::Status Cancel() override;

 private:
  absl::Notification is_cancelled_;
  std::unique_ptr<object_detection::ObjectDetector> person_detector_;
  absl::Duration poll_time_out_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_NEGATIVE_PERSON_FILTER_H_
