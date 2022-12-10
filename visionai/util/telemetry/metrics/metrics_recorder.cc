// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/telemetry/metrics/metrics_recorder.h"

#include "glog/logging.h"
#include "absl/base/attributes.h"
#include "absl/memory/memory.h"
#include "absl/synchronization/mutex.h"

namespace visionai {
namespace {

ABSL_CONST_INIT absl::Mutex metrics_recorder_mutex(absl::kConstInit);

// Global singleton object for a metrics recorder (go/totw/110). Needs to be a
// pointer so derived classes can override implementations.
MetricsRecorder* metrics_recorder = nullptr;

}  // namespace

// Initialize the global singleton
ScopedGlobalMetricsRecorder::ScopedGlobalMetricsRecorder(
    std::unique_ptr<MetricsRecorder> recorder) {
  absl::WriterMutexLock mutex_lock(&metrics_recorder_mutex);

  LOG_IF(FATAL, metrics_recorder != nullptr)
      << "Expected the global singleton metrics recorder to be uninitialized";
  metrics_recorder = recorder.release();
}

// Destruct the global singleton
ScopedGlobalMetricsRecorder::~ScopedGlobalMetricsRecorder() {
  absl::WriterMutexLock mutex_lock(&metrics_recorder_mutex);

  LOG_IF(FATAL, metrics_recorder == nullptr)
      << "Expected the global singleton metrics recorder to be initialized "
         "here!";
  auto clean_up = absl::WrapUnique(metrics_recorder);
  metrics_recorder = nullptr;
}

MetricsRecorder& GetMetricsRecorder() {
  absl::ReaderMutexLock mutex_lock(&metrics_recorder_mutex);
  LOG_IF(FATAL, metrics_recorder == nullptr)
      << "Expected the global singleton metrics recorder to be initialized "
         "here!";
  return *metrics_recorder;
}

}  // namespace visionai
