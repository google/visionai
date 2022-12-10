// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_WORKER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_WORKER_H_

#include <functional>
#include <memory>
#include <thread>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"

namespace visionai {
namespace streams_internal {

// ----------------------------------------------------------------------------
// Worker
// ----------------------------------------------------------------------------

// The Worker class runs and manages an aynchronous task in the background.
//
// Users may delegate tasks to a Worker object to run asynchronously. They may
// later check on the worker's progress, get the return status if done, or
// cancel the task if it is cancellable.
//
// The Worker has three states. It transitions in the following order:
// 1. AVAILABLE: This is the start state of the worker.
// 2. WORKING: The worker transitions to this state once `Start` is called and
//             the background thread is running and about to block on the given
//             task.
// 3. DONE: The worker transitions to this state once the task returns.
//
// If the given task supports cancellation, calling Cancel will request that the
// worker transition from WORKING to DONE as soon as possible.
//
// The resources associated with the worker, including its managed background
// thread, are garbage collected on destruction.
//
// Example usage:
//
//   // Create a worker available to work.
//   std::unique_ptr<Worker> worker;
//
//   // Dispatch a task to it. Returns immediately.
//   worker->Start([]() { return background_work(); });
//
//   // ... do something ...
//
//   // Request the worker to stop.
//   worker->Cancel([]() { return stop_background_work(); });
//
//   // Release resources.
//   //
//   // You may consider separately arranging a timeout for the cancellation to
//   // take effect.
//   worker->reset();
class Worker {
 public:
  // Non-blocking call to run the given task in a background thread.
  //
  // Returns once the task has begun running.
  absl::Status Work(std::function<absl::Status(void)> task);

  // Requests that the currently executing task be cancelled.
  //
  // The given task canceller should contain the specific logic necessary for
  // the cancellation to happen. It must be safe to invoke even if the worker
  // has completed.
  //
  // The request will only be sent if:
  // 1. The worker might still be running.
  // 2. The worker has not been requested to cancel before.
  void Cancel(std::function<absl::Status(void)> task_canceller);

  // Returns true iff the a cancellation has been requested.
  bool IsCancelRequested() const;

  // Returns true iff the given task is done.
  bool IsDone() const;

  // Waits up to `timeout` for the task to complete. The return status of the
  // task will be stored in `return_status`.
  //
  // This function itself returns Unavailable if the task is not done yet.
  // Otherwise, returns OK.
  absl::Status GetReturnStatus(absl::Duration timeout,
                               absl::Status* return_status);

  // Copy-control members.
  //
  // Movable, but not Copyable.
  Worker() : return_status_(std::make_shared<ReturnStatus>()) {}
  ~Worker();
  Worker(Worker&&) = default;
  Worker& operator=(Worker&&) = default;
  Worker(const Worker&) = delete;
  Worker& operator=(const Worker&) = delete;

 private:
  std::thread worker_;
  absl::Notification is_working_;
  absl::Notification is_cancel_requested_;

  struct ReturnStatus {
    absl::Mutex mu;
    absl::Status status;
    absl::Notification is_done;
  };
  std::shared_ptr<ReturnStatus> return_status_;
};

}  // namespace streams_internal
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_WORKER_H_
