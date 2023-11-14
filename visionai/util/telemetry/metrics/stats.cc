// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/telemetry/metrics/stats.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

#include "prometheus/registry.h"
#include "absl/container/flat_hash_map.h"
#include "opencensus/stats/stats.h"
#include "visionai/util/telemetry/metrics/constants.h"

namespace visionai {
namespace {
using opencensus::stats::Aggregation;
using opencensus::stats::MeasureDouble;
using opencensus::stats::MeasureInt64;
}  // namespace

absl::flat_hash_map<std::string, opencensus::tags::TagKey>
    MetricsTagKey::tag_keys_ = {};

#define REGISTER_MEASURE(type, name, description, unit)                \
  opencensus::stats::Measure##type MetricsMeasure::name() {            \
    static const opencensus::stats::Measure##type __measure =          \
        opencensus::stats::Measure##type::Register(kMeasure##name,     \
                                                   description, unit); \
    return __measure;                                                  \
  }

#define REGISTER_TAGKEY(name)                                                \
  opencensus::tags::TagKey MetricsTagKey::name() {                           \
    if (!MetricsTagKey::tag_keys_.contains(kTagKey##name)) {                 \
      MetricsTagKey::tag_keys_.emplace(                                      \
          kTagKey##name, opencensus::tags::TagKey::Register(kTagKey##name)); \
    }                                                                        \
    return MetricsTagKey::tag_keys_.find(kTagKey##name)->second;             \
  }

// Register measures.
REGISTER_MEASURE(Double, OperatorRequestLatency, "request latency measure",
                 kMs);
REGISTER_MEASURE(Int64, OperatorRequestCount, "request count measure", kCount);
REGISTER_MEASURE(Double, CustomModelPredictLatency,
                 "custom model prediction latency measure", kMs);
REGISTER_MEASURE(Int64, CustomModelPredictCount,
                 "custom model prediction count measure", kCount);
REGISTER_MEASURE(Double, InstanceUptime, "instance uptime measure", kMs);

REGISTER_MEASURE(Int64, StreamsSentBytesCount, "sent bytes count measure", kBy);
REGISTER_MEASURE(Int64, StreamsReceivedBytesCount,
                 "received bytes count measure", kBy);
REGISTER_MEASURE(Int64, StreamsSentPacketsCount, "sent packets count measure",
                 kCount);
REGISTER_MEASURE(Int64, StreamsReceivedPacketsCount,
                 "received packets count measure", kCount);
REGISTER_MEASURE(Int64, ProcessStartTime,
                 "process start time in seconds since unix epoch", kSecond);

// Register tags.
REGISTER_TAGKEY(AnalysisName);
REGISTER_TAGKEY(AnalyzerName);
REGISTER_TAGKEY(StreamName);
REGISTER_TAGKEY(EventName);
REGISTER_TAGKEY(ChannelName);
REGISTER_TAGKEY(ConsumerName);
REGISTER_TAGKEY(ServiceName);
REGISTER_TAGKEY(ResponseCode);
REGISTER_TAGKEY(ProjectID);
REGISTER_TAGKEY(LocationID);
REGISTER_TAGKEY(ClusterID);
REGISTER_TAGKEY(StreamID);
REGISTER_TAGKEY(EventID);

std::pair<opencensus::tags::TagKey, std::string>
MetricsTagKey::AnalyzerNameTag() {
  return {MetricsTagKey::AnalyzerName(),
          std::string(absl::NullSafeStringView(getenv(kAnalyzerNameEnv)))};
}

std::pair<opencensus::tags::TagKey, std::string>
MetricsTagKey::AnalysisNameTag() {
  return {MetricsTagKey::AnalysisName(),
          std::string(absl::NullSafeStringView(getenv(kAnalysisNameEnv)))};
}

opencensus::stats::ViewDescriptor MetricsView::OperatorRequestLatency() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewOperatorRequestLatency)
      .set_measure(kMeasureOperatorRequestLatency)
      .set_aggregation(Aggregation::LastValue())
      .add_column(MetricsTagKey::AnalysisName())
      .add_column(MetricsTagKey::AnalyzerName())
      .add_column(MetricsTagKey::ServiceName())
      .add_column(MetricsTagKey::ResponseCode());
}

opencensus::stats::ViewDescriptor MetricsView::OperatorRequestCount() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewOperatorRequestCount)
      .set_measure(kMeasureOperatorRequestCount)
      .set_aggregation(Aggregation::Sum())
      .add_column(MetricsTagKey::AnalysisName())
      .add_column(MetricsTagKey::AnalyzerName())
      .add_column(MetricsTagKey::ServiceName())
      .add_column(MetricsTagKey::ResponseCode());
}

opencensus::stats::ViewDescriptor MetricsView::CustomModelPredictLatency() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewCustomModelPredictLatency)
      .set_measure(kMeasureCustomModelPredictLatency)
      .set_aggregation(Aggregation::LastValue())
      .add_column(MetricsTagKey::AnalysisName())
      .add_column(MetricsTagKey::AnalyzerName())
      .add_column(MetricsTagKey::ResponseCode());
}

opencensus::stats::ViewDescriptor MetricsView::CustomModelPredictCount() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewCustomModelPredictCount)
      .set_measure(kMeasureCustomModelPredictCount)
      .set_aggregation(Aggregation::Sum())
      .add_column(MetricsTagKey::AnalysisName())
      .add_column(MetricsTagKey::AnalyzerName())
      .add_column(MetricsTagKey::ResponseCode());
}

opencensus::stats::ViewDescriptor MetricsView::InstanceUptime() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewInstanceUptime)
      .set_measure(kMeasureInstanceUptime)
      .set_aggregation(Aggregation::LastValue())
      .add_column(MetricsTagKey::AnalysisName());
}

opencensus::stats::ViewDescriptor MetricsView::StreamsReceivedBytesCount() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewStreamsReceivedBytesCount)
      .set_measure(kMeasureStreamsReceivedBytesCount)
      .set_aggregation(Aggregation::Sum())
      .add_column(MetricsTagKey::ProjectID())
      .add_column(MetricsTagKey::LocationID())
      .add_column(MetricsTagKey::ClusterID())
      .add_column(MetricsTagKey::StreamID())
      .add_column(MetricsTagKey::EventID());
}

opencensus::stats::ViewDescriptor MetricsView::StreamsReceivedPacketsCount() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewStreamsReceivedPacketsCount)
      .set_measure(kMeasureStreamsReceivedPacketsCount)
      .set_aggregation(Aggregation::Sum())
      .add_column(MetricsTagKey::ProjectID())
      .add_column(MetricsTagKey::LocationID())
      .add_column(MetricsTagKey::ClusterID())
      .add_column(MetricsTagKey::StreamID())
      .add_column(MetricsTagKey::EventID());
}

opencensus::stats::ViewDescriptor MetricsView::StreamsSentBytesCount() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewStreamsSentBytesCount)
      .set_measure(kMeasureStreamsSentBytesCount)
      .set_aggregation(Aggregation::Sum())
      .add_column(MetricsTagKey::ProjectID())
      .add_column(MetricsTagKey::LocationID())
      .add_column(MetricsTagKey::ClusterID())
      .add_column(MetricsTagKey::StreamID())
      .add_column(MetricsTagKey::EventID());
}

opencensus::stats::ViewDescriptor MetricsView::StreamsSentPacketsCount() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewStreamsSentPacketsCount)
      .set_measure(kMeasureStreamsSentPacketsCount)
      .set_aggregation(Aggregation::Sum())
      .add_column(MetricsTagKey::ProjectID())
      .add_column(MetricsTagKey::LocationID())
      .add_column(MetricsTagKey::ClusterID())
      .add_column(MetricsTagKey::StreamID())
      .add_column(MetricsTagKey::EventID());
}

opencensus::stats::ViewDescriptor MetricsView::ProcessStartTime() {
  return opencensus::stats::ViewDescriptor()
      .set_name(kViewProcessStartTime)
      .set_measure(kMeasureProcessStartTime)
      .set_aggregation(Aggregation::LastValue());
}

void MetricsTagKey::RegisterTagKeys(
    const absl::flat_hash_map<std::string, std::string>& global_tags) {
  for (auto it = global_tags.begin(); it != global_tags.end(); ++it) {
    if (!MetricsTagKey::tag_keys_.contains(it->first)) {
      MetricsTagKey::tag_keys_.emplace(
          it->first, opencensus::tags::TagKey::Register(it->first));
    }
  }
}

void MetricsView::RegisterViews(
    const absl::flat_hash_map<std::string, std::string>& global_tags) {
  std::vector<opencensus::stats::ViewDescriptor> view_descriptors{
      OperatorRequestCount(),        InstanceUptime(),
      OperatorRequestLatency(),      StreamsReceivedBytesCount(),
      StreamsReceivedPacketsCount(), StreamsSentBytesCount(),
      StreamsSentPacketsCount(),     ProcessStartTime(),
      CustomModelPredictCount(),     CustomModelPredictLatency(),
  };
  for (auto& view_descriptor : view_descriptors) {
    for (const auto& global_tag : global_tags) {
      view_descriptor.add_column(
          MetricsTagKey::tag_keys_.find(global_tag.first)->second);
    }
    view_descriptor.RegisterForExport();
  }
}

std::shared_ptr<prometheus::Registry> GlobalRegistry() {
  static std::shared_ptr<prometheus::Registry> registry =
      std::make_shared<prometheus::Registry>();
  return registry;
}

std::string GetAnalysisName() {
  return std::string(absl::NullSafeStringView(getenv(kAnalysisNameEnv)));
}

std::string GetAnalyzerName() {
  return std::string(absl::NullSafeStringView(getenv(kAnalyzerNameEnv)));
}

std::string GetAnalysisId() {
  std::vector<std::string> analysis_info_vec =
      absl::StrSplit(GetAnalysisName(), '/');
  if (analysis_info_vec.empty())
    return "";
  else
    return analysis_info_vec.back();
}

std::string GetProcessName() {
  return std::string(absl::NullSafeStringView(getenv(kProcessNameEnv)));
}

std::string GetInputStream() {
  return std::string(absl::NullSafeStringView(getenv(kInputStreamNameEnv)));
}

#undef REGISTER_MEASURE
#undef REGISTER_TAGKEY

}  // namespace visionai
