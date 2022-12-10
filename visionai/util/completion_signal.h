/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_COMPLETION_SIGNAL_H_
#define VISIONAI_UTIL_COMPLETION_SIGNAL_H_

#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"

namespace visionai {

// CompletionSignal is a simple object used to communicate work progress and
// return status of an asynchronous worker to others.
//
// It is thread-safe.
//
// Example Usage
// -------------
//
// ```
// CompletionSignal signal;
//
// // Mark that the work is in progress and start the worker.
// signal.Start();
// std::thread worker([&signal]() {
//
//   // Do your thing.
//   absl::Status work_status = DoImportantThing();
//
//   // When done, communicate the status and mark it complete.
//   signal.SetStatus(work_status);
//   signal.End();
//   return;
// });
//
// // Wait for the work to complete.
// while (!signal.WaitUntilCompleted(absl::Second(5)));
//
// // Check the worker's return status
// absl::Status worker_status = signal.GetStatus();
// ```
class CompletionSignal {
 public:
  // Constructor.
  CompletionSignal();

  // Mark that the work is in progress.
  void Start();

  // Mark that the work has completed.
  void End();

  // Returns true if and only if work is not in progress.
  bool IsCompleted() const;

  // Blocks the caller until either the work has completed or if the specified
  // timeout has expired.
  //
  // Returns true if work is completed; otherwise, false.
  bool WaitUntilCompleted(absl::Duration timeout);

  // Get the status associated with this Signal.
  absl::Status GetStatus() const;

  // Set the status associated with this Signal.
  void SetStatus(const absl::Status& status);

  // Copy control.
  ~CompletionSignal();
  CompletionSignal(const CompletionSignal&) = delete;
  CompletionSignal& operator=(const CompletionSignal&) = delete;

 private:
  mutable absl::Mutex mu_;
  bool is_completed_ ABSL_GUARDED_BY(mu_) = true;
  absl::Status status_ ABSL_GUARDED_BY(mu_) = absl::OkStatus();
};

}  // namespace visionai

#endif  // VISIONAI_UTIL_COMPLETION_SIGNAL_H_
