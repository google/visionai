// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_UTIL_WORK_AGENT_H_
#define VISIONAI_UTIL_WORK_AGENT_H_

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"

namespace visionai {

// WorkAgent is a CRTP base that enables derived classes to acquire the
// ability to repeatedly run a function in a loop. Moreover, this work may be
// run in a background thread.
//
// Derived class requirements:
// + Implement `absl::Status Work()`
//   This is executed repeatedly in the main loop.
//
// + Implement `void Cleanup()`
//   This is called when Finish() is called.
//
// + Ensure all initialization steps are done before `Start` is called.
//
// Example:
//
// class MyAgent : public WorkAgent<MyAgent> {
//   public:
//     static StatusOr<shared_ptr<MyAgent>> Create() {
//       // One stop shop for construction, initialization.
//     }
//     absl::Work() {
//       // something...
//     }
// };
//
// VAI_ASSIGN_OR_RETURN(auto my_agent, MyAgent::Create());
// std::thread t(
//   [my_agent] () -> absl::Status { return my_agent->Start();
// });
//
// // ... time to cleanup.
// auto status = my_agent->Finish();
// if (status.ok()) {
//   // joinable.
// } else {
//   // didn't stop in time.
//   // might need to detach to avoid a crash.
// }
//
template <typename Derived>
class WorkAgent {
 public:
  // Start working.
  //
  // If the worker has already been started or finished, then immediately
  // returns with absl::OkStatus();
  //
  // Otherwise, blocks the calling thread until one of the following happens:
  // 1. Finish() is requested separately. Returns OK.
  // 2. The worker loop encounters a cancellation. Returns OK.
  // 3. The worker loop encounters an error. Returns the error.
  absl::Status Start();

  // Finish working.
  //
  // If the work agent is running, it will complete the current iteration before
  // exiting the run loop.
  //
  // If the work agent wasn't already running, then it will immediately exit the
  // run loop.
  absl::Status Finish();

  // Set the timeout to wait for Start() to return when Finish() is called.
  //
  // Must call before `Start`.
  //
  // Defaults to absl::ZeroDuration.
  void SetFinishTimeout(absl::Duration);

  // Wait for the work agent to enter its run loop up to the given `timeout`.
  bool WaitForRunning(absl::Duration timeout);

  // Wait for the work agent to exit its run loop up to the given `timeout`.
  bool WaitForDone(absl::Duration timeout);

  WorkAgent();
  ~WorkAgent();

 protected:
  std::atomic<bool> started_or_finished_;

  absl::Notification is_running_;
  absl::Notification is_done_;
  std::atomic<bool> keep_working_;

  absl::Duration finish_timeout_ = absl::ZeroDuration();

 private:
  Derived* derived() { return static_cast<Derived*>(this); }
};

template <typename Derived>
WorkAgent<Derived>::WorkAgent() {
  started_or_finished_.store(false);
  keep_working_.store(true);
}

template <typename Derived>
absl::Status WorkAgent<Derived>::Start() {
  if (started_or_finished_.exchange(true)) {
    return absl::OkStatus();
  }
  is_running_.Notify();

  // Main loop.
  absl::Status status = absl::OkStatus();
  while (keep_working_.load()) {
    status = derived()->Work();
    if (!status.ok()) {
      if (absl::IsUnavailable(status)) {
        LOG(WARNING) << "Received transient error: " << status;
        continue;
      } else if (absl::IsCancelled(status)) {
        LOG(INFO) << "Received cancellation request: " << status;
        break;
      } else {
        LOG(FATAL) << "Received non-recoverable error: " << status;
        break;
      }
    }
  }
  derived()->Cleanup();

  is_done_.Notify();
  return status;
}

template <typename Derived>
absl::Status WorkAgent<Derived>::Finish() {
  keep_working_.store(false);
  if (started_or_finished_.exchange(true) && is_running_.HasBeenNotified()) {
    if (!WaitForDone(finish_timeout_)) {
      return absl::DeadlineExceededError("The WorkAgent did not stop in time.");
    }
  }
  return absl::OkStatus();
}

template <typename Derived>
WorkAgent<Derived>::~WorkAgent() {
  auto status = Finish();
  if (!status.ok()) {
    LOG(ERROR) << status;
  }
}

template <typename Derived>
inline bool WorkAgent<Derived>::WaitForRunning(absl::Duration timeout) {
  return is_running_.WaitForNotificationWithTimeout(timeout);
}

template <typename Derived>
inline bool WorkAgent<Derived>::WaitForDone(absl::Duration timeout) {
  return is_done_.WaitForNotificationWithTimeout(timeout);
}

}  // namespace visionai

#endif  // VISIONAI_UTIL_WORK_AGENT_H_
