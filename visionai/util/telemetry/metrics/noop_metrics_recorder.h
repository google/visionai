// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_NOOP_METRICS_RECORDER_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_NOOP_METRICS_RECORDER_H_

#include <initializer_list>
#include <string>

#include "opencensus/stats/stats.h"
#include "visionai/util/telemetry/metrics/metrics_recorder.h"

namespace visionai {

// Explicitly provide a NOOP metrics recorder
class NoopMetricsRecorder : public MetricsRecorder {
 public:
  NoopMetricsRecorder() = default;
  virtual ~NoopMetricsRecorder() override = default;

  // Should not be copyable or movable
  NoopMetricsRecorder(const NoopMetricsRecorder&) = delete;
  NoopMetricsRecorder& operator=(const NoopMetricsRecorder&) = delete;
  NoopMetricsRecorder(NoopMetricsRecorder&&) = delete;
  NoopMetricsRecorder& operator=(NoopMetricsRecorder&&) = delete;

  virtual void Record(
      std::initializer_list<opencensus::stats::Measurement> measurements,
      const opencensus::tags::TagMap& tag_map = {}) override {}
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_NOOP_METRICS_RECORDER_H_
