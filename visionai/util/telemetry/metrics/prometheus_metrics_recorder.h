// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_PROMETHEUS_METRICS_RECORDER_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_PROMETHEUS_METRICS_RECORDER_H_

#include <prometheus/exposer.h>

#include <initializer_list>
#include <string>
#include <utility>

#include "opencensus/exporters/stats/prometheus/prometheus_exporter.h"
#include "opencensus/stats/stats.h"
#include "opencensus/tags/tag_map.h"
#include "visionai/util/telemetry/metrics/metrics_recorder.h"

namespace visionai {

class PrometheusMetricsRecorder : public MetricsRecorder {
 public:
  explicit PrometheusMetricsRecorder(const std::string& address);
  explicit PrometheusMetricsRecorder(
      const std::string& address,
      const absl::flat_hash_map<std::string, std::string>& global_tags);
  ~PrometheusMetricsRecorder() override = default;

  // Should not be copyable or movable
  PrometheusMetricsRecorder(const PrometheusMetricsRecorder&) = delete;
  PrometheusMetricsRecorder& operator=(const PrometheusMetricsRecorder&) =
      delete;
  PrometheusMetricsRecorder(PrometheusMetricsRecorder&&) = delete;
  PrometheusMetricsRecorder& operator=(PrometheusMetricsRecorder&&) = delete;

  virtual void Record(
      std::initializer_list<opencensus::stats::Measurement> measurements,
      const opencensus::tags::TagMap& tag_map = {}) override;

 private:
  std::shared_ptr<opencensus::exporters::stats::PrometheusExporter> exporter_;
  prometheus::Exposer exposer_;
  opencensus::tags::TagMap global_tag_map_ = {};
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_PROMETHEUS_METRICS_RECORDER_H_
