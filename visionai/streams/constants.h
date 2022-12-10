// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_STREAMS_CONSTANTS_H_
#define VISIONAI_STREAMS_CONSTANTS_H_

#include <string>

namespace visionai {

// ----------------------------------------------------------------------------
// IngesterConfig Defaults

// Default for IngesterConfig::Parameters::sleep_period_ms.
constexpr int32_t kDefaultIngesterSleepPeriodMs = 1000;

// Default for IngesterConfig::Parameters::capture_output_buffer_capacity.
constexpr int32_t kDefaultCaptureOutputBufferCapacity = 100;

// Default for IngesterConfig::Parameters::capture_worker_finalize_timeout_ms.
constexpr int32_t kDefaultCaptureWorkerFinalizeTimeoutMs = 2000;

// Default for IngesterConfig::Parameters::filter_output_buffer_capacity.
constexpr int32_t kDefaultFilterOutputBufferCapacity = 100;

// Default for IngesterConfig::Parameters::filter_worker_finalize_timeout_ms.
constexpr int32_t kDefaultFilterWorkerFinalizeTimeoutMs = 2000;

// Default for IngesterConfig::Parameters::depositor_worker_finalize_timeout_ms.
constexpr int32_t kDefaultDepositorWorkerFinalizeTimeoutMs = 2000;

// Default for DespositorConfig::input_poll_timeout_ms.
constexpr int32_t kDefaultDepositorInputPollTimeoutMs = 20;

// ----------------------------------------------------------------------------
// EventSink Defaults

// Default write buffer capacity for an EventSink.
constexpr int32_t kDefaultEventSinkWriteBufferCapacity =
    std::numeric_limits<int>::max();

// Default event sink finalization timeout.
constexpr int32_t kDefaultEventSinkFinalizationTimeoutMs = 10000;

}  // namespace visionai

#endif  // VISIONAI_STREAMS_CONSTANTS_H_
