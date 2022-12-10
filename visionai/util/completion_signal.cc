/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#include "visionai/util/completion_signal.h"

namespace visionai {

CompletionSignal::CompletionSignal() = default;
CompletionSignal::~CompletionSignal() = default;

void CompletionSignal::Start() {
  absl::MutexLock lock(&mu_);
  is_completed_ = false;
}

void CompletionSignal::End() {
  absl::MutexLock lock(&mu_);
  is_completed_ = true;
}

bool CompletionSignal::IsCompleted() const {
  absl::MutexLock lock(&mu_);
  return is_completed_;
}

bool CompletionSignal::WaitUntilCompleted(absl::Duration timeout) {
  absl::MutexLock lock(&mu_);
  absl::Condition cond(
      +[](bool* is_completed) -> bool { return *is_completed; },
      &is_completed_);
  return mu_.AwaitWithTimeout(cond, timeout);
}

absl::Status CompletionSignal::GetStatus() const {
  absl::MutexLock lock(&mu_);
  return status_;
}

void CompletionSignal::SetStatus(const absl::Status& status) {
  absl::MutexLock lock(&mu_);
  status_ = status;
  return;
}

}  // namespace visionai
