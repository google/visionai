/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_LABELS_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_LABELS_H_

#include "visionai/proto/cluster_selection.pb.h"
namespace visionai {

struct MetricStreamResourceLabels {
  std::string project_id;
  std::string location_id;
  std::string cluster_id;
  std::string stream_id;
};

inline MetricStreamResourceLabels GetMetricStreamResourceLabels(
    const ClusterSelection& cluster_selection, const std::string& stream_id) {
  MetricStreamResourceLabels labels;
  labels.project_id = cluster_selection.project_id();
  labels.location_id = cluster_selection.location_id();
  labels.cluster_id = cluster_selection.cluster_id();
  labels.stream_id = stream_id;
  return labels;
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_LABELS_H_
