// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_METRICS_RECORDER_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_METRICS_RECORDER_H_

#include <initializer_list>
#include <memory>

#include "absl/container/flat_hash_map.h"
#include "opencensus/stats/stats.h"

namespace visionai {

// Interface for a metrics recorder
class MetricsRecorder {
 public:
  MetricsRecorder() = default;
  virtual ~MetricsRecorder() = default;

  // Should not be copyable or movable
  MetricsRecorder(const MetricsRecorder&) = delete;
  MetricsRecorder& operator=(const MetricsRecorder&) = delete;
  MetricsRecorder(MetricsRecorder&&) = delete;
  MetricsRecorder& operator=(MetricsRecorder&&) = delete;

  virtual void Record(
      std::initializer_list<opencensus::stats::Measurement> measurements,
      const opencensus::tags::TagMap& tag_map = {}) {}
};

// Scopes the global singleton metrics recorder (handles initialization and
// destruction). Only one of this object should be constructed per program.
class ScopedGlobalMetricsRecorder {
 public:
  ScopedGlobalMetricsRecorder(std::unique_ptr<MetricsRecorder> recorder);
  ~ScopedGlobalMetricsRecorder();
};

// Leave this as a free function so the global singleton metrics recorder is
// accessible from any scope.
MetricsRecorder& GetMetricsRecorder();

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_METRICS_RECORDER_H_
