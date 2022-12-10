// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/util/worker.h"

#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

namespace visionai {
namespace streams_internal {

TEST(WorkerTest, BasicTest) {
  {
    Worker worker;
    absl::Notification is_cancelled;
    auto s = worker.Work([&is_cancelled]() {
      is_cancelled.WaitForNotification();
      return absl::OkStatus();
    });
    EXPECT_TRUE(s.ok());
    EXPECT_FALSE(worker.IsDone());
    absl::Status return_status =
        worker.GetReturnStatus(absl::ZeroDuration(), &return_status);
    EXPECT_TRUE(absl::IsUnavailable(return_status));
    worker.Cancel([&is_cancelled]() {
      is_cancelled.Notify();
      return absl::OkStatus();
    });
    return_status = worker.GetReturnStatus(absl::Seconds(5), &return_status);
    EXPECT_TRUE(worker.IsDone());
    EXPECT_TRUE(return_status.ok());
  }
}

TEST(WorkerTest, DetachWorkerThreadTest) {
  {
    Worker worker;
    absl::Notification is_cancelled;
    auto s = worker.Work([&is_cancelled]() {
      is_cancelled.WaitForNotification();
      absl::SleepFor(absl::Seconds(2));
      return absl::OkStatus();
    });
    EXPECT_TRUE(s.ok());
    worker.Cancel([&is_cancelled]() {
      is_cancelled.Notify();
      return absl::OkStatus();
    });
    EXPECT_FALSE(worker.IsDone());
  }
  absl::SleepFor(absl::Seconds(2));
}

}  // namespace streams_internal
}  // namespace visionai
