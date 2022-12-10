// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/util/worker.h"

#include "glog/logging.h"
#include "absl/synchronization/mutex.h"

namespace visionai {
namespace streams_internal {

absl::Status Worker::Work(std::function<absl::Status(void)> task) {
  worker_ = std::thread([this, return_status = this->return_status_, task]() {
    is_working_.Notify();
    auto status = task();
    {
      absl::MutexLock lock(&return_status->mu);
      return_status->status = status;
    }
    return_status->is_done.Notify();
  });
  is_working_.WaitForNotification();
  return absl::OkStatus();
}

void Worker::Cancel(std::function<absl::Status(void)> task_canceller) {
  if (!IsDone() && !IsCancelRequested()) {
    is_cancel_requested_.Notify();
    auto s = task_canceller();
    if (!s.ok()) {
      LOG(ERROR) << s;
    }
  }
}

bool Worker::IsDone() const {
  return return_status_->is_done.HasBeenNotified();
}

bool Worker::IsCancelRequested() const {
  return is_cancel_requested_.HasBeenNotified();
}

absl::Status Worker::GetReturnStatus(absl::Duration timeout,
                                     absl::Status* return_status) {
  if (!return_status_->is_done.WaitForNotificationWithTimeout(timeout)) {
    return absl::UnavailableError("The task has not completed yet.");
  }
  absl::MutexLock lock(&return_status_->mu);
  *return_status = return_status_->status;
  return absl::OkStatus();
}

Worker::~Worker() {
  return_status_->is_done.HasBeenNotified() ? worker_.join() : worker_.detach();
}

}  // namespace streams_internal
}  // namespace visionai
