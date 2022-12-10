// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/telemetry/metrics/prometheus_metrics_recorder.h"

#include <prometheus/exposer.h>

#include <initializer_list>
#include <string>
#include <utility>

#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "opencensus/exporters/stats/prometheus/prometheus_exporter.h"
#include "visionai/util/telemetry/metrics/stats.h"

namespace visionai {

PrometheusMetricsRecorder::PrometheusMetricsRecorder(const std::string& address)
    : PrometheusMetricsRecorder(address, {}) {}

PrometheusMetricsRecorder::PrometheusMetricsRecorder(
    const std::string& address,
    const absl::flat_hash_map<std::string, std::string>& global_tags)
    : exposer_(address) {
  exporter_ =
      std::make_shared<opencensus::exporters::stats::PrometheusExporter>();
  exposer_.RegisterCollectable(exporter_);
  exposer_.RegisterCollectable(GlobalRegistry());

  MetricsTagKey::RegisterTagKeys(global_tags);
  std::vector<std::pair<opencensus::tags::TagKey, std::string>> tags;
  for (auto& global_tag : global_tags) {
    opencensus::tags::TagKey tag_key =
        MetricsTagKey::tag_keys_.find(global_tag.first)->second;
    tags.emplace_back(tag_key, global_tag.second);
  }
  global_tag_map_ = opencensus::tags::TagMap(tags);

  MetricsMeasure::RegisterMeasures();
  MetricsView::RegisterViews(global_tags);

  // System metrics.
  opencensus::stats::Record(
      {{MetricsMeasure::ProcessStartTime(),
        (absl::Now() - absl::UnixEpoch()) / absl::Seconds(1)}},
      tags);
}

void PrometheusMetricsRecorder::Record(
    std::initializer_list<opencensus::stats::Measurement> measurements,
    const opencensus::tags::TagMap& tag_map) {
  std::vector<std::pair<opencensus::tags::TagKey, std::string>> tags;
  tags.insert(tags.end(), global_tag_map_.tags().begin(),
              global_tag_map_.tags().end());
  tags.insert(tags.end(), tag_map.tags().begin(), tag_map.tags().end());
  opencensus::stats::Record(measurements, tags);
}

}  // namespace visionai
