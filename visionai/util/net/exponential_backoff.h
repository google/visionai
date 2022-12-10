// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_EXPONENTIAL_BACKOFF_H_
#define THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_EXPONENTIAL_BACKOFF_H_

#include "absl/time/time.h"

namespace visionai {

class ExponentialBackoff {
 public:
  // Creates an exponential backoff class with the following parameters:
  // - initial_wait_time: time waited the first time Wait() is called.
  // - max_wait_time: maximum time to wait when Wait() is called.
  // - wait_time_multiplier: at each subsequent iteration, the wait time is
  // multiplied by this value (must be >= 1).
  ExponentialBackoff(absl::Duration initial_wait_time,
                     absl::Duration max_wait_time, float wait_time_multiplier);

  // Waits for the current wait time, and increments the wait time value.
  void Wait();

 private:
  absl::Duration initial_wait_time_;
  absl::Duration current_wait_time_;
  absl::Duration max_wait_time_;
  float wait_time_multiplier_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_EXPONENTIAL_BACKOFF_H_
