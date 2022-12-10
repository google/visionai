// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_WORKER_V1_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_WORKER_V1_H_

#include <deque>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "absl/status/status.h"
#include "absl/time/time.h"
#include "visionai/streams/util/worker.h"

namespace visionai {
namespace streams_internal {

// `WorkerV1` is an internal object that supplies a `thread::Fiber` like
// programming interface.
class WorkerV1 {
 public:
  // Once `WorkerV1` is constructed, a background thread immediately begins
  // executing `task`. `canceller` is used to cancel `task`, and should cause
  // the thread to become joinable.
  WorkerV1(std::function<absl::Status(void)> task,
           std::function<absl::Status(void)> canceller)
      : task_(task), canceller_(canceller) {
    worker_ = std::make_unique<Worker>();
    worker_->Work(task_).IgnoreError();
  }

  // Explicitly request the worker to stop.
  void Cancel() { worker_->Cancel(canceller_); }

  // "Join"s the worker. It just waits for it to complete.
  void Join() {
    absl::Status s;
    worker_->GetReturnStatus(absl::InfiniteDuration(), &s).IgnoreError();
    s.IgnoreError();
    is_not_joined_ = false;
  }

  // Decides if the worker is "joinable".
  bool IsJoinable() { return is_not_joined_; }

 private:
  std::unique_ptr<Worker> worker_;
  std::function<absl::Status(void)> task_;
  std::function<absl::Status(void)> canceller_;
  bool is_not_joined_ = true;
};

}  // namespace streams_internal
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_WORKER_V1_H_
