// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

 #include "visionai/util/net/exponential_backoff.h"

#include <algorithm>

#include "absl/time/clock.h"

namespace visionai {

using ::absl::Duration;
using ::absl::ZeroDuration;

ExponentialBackoff::ExponentialBackoff(absl::Duration initial_wait_time,
                                       absl::Duration max_wait_time,
                                       float wait_time_multiplier) {
  // Initial wait time should not be negative.
  // TODO(unknown): set positive value for initial_wait_time_.
  initial_wait_time_ = std::max(initial_wait_time, ZeroDuration());
  current_wait_time_ = initial_wait_time_;
  // Maximum wait time should not be lower than the initial wait time.
  max_wait_time_ = std::max(max_wait_time, initial_wait_time_);
  // Wait time multiplier should not be less than 1.
  wait_time_multiplier_ = std::max(wait_time_multiplier, 1.0f);
}

void ExponentialBackoff::Wait() {
  absl::SleepFor(current_wait_time_);
  if (current_wait_time_ < max_wait_time_) {
    current_wait_time_ =
        std::min(wait_time_multiplier_ * current_wait_time_, max_wait_time_);
  }
}
}  // namespace visionai
