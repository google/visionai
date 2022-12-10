/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_STATS_MACROS_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_STATS_MACROS_H_

namespace visionai {

#define COUNTER(name, help, registry)                                         \
  inline prometheus::Family<prometheus::Counter>& name() {                    \
    static auto& name =                                                       \
        prometheus::BuildCounter().Name(#name).Help(help).Register(registry); \
    return name;                                                              \
  }

#define GAUGE(name, help, registry)                                           \
  inline prometheus::Family<prometheus::Gauge>& name() {                      \
    static auto& name =                                                       \
        prometheus::BuildGauge().Name(#name).Help(help).Register(registry);   \
    return name;                                                              \
  }

#define HISTOGRAM(name, help, registry)                                       \
  inline prometheus::Family<prometheus::Histogram>& name() {                  \
    static auto& name =                                                       \
        prometheus::BuildHistogram().Name(#name).Help(help).Register(         \
            registry);                                                        \
    return name;                                                              \
  }

#define SUMMARY(name, help, registry)                                         \
  inline prometheus::Family<prometheus::Summary>& name() {                    \
    static auto& name =                                                       \
        prometheus::BuildSummary().Name(#name).Help(help).Register(registry); \
    return name;                                                              \
  }

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_STATS_MACROS_H_
