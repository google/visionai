// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/worker_v1.h"

#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

namespace visionai {
namespace streams_internal {

TEST(WorkerV1Test, BasicTest) {
  {
    absl::Notification is_cancelled;
    WorkerV1 worker(
        [&is_cancelled]() {
          is_cancelled.WaitForNotification();
          return absl::OkStatus();
        },
        [&is_cancelled]() {
          is_cancelled.Notify();
          return absl::OkStatus();
        });
    EXPECT_TRUE(worker.IsJoinable());
    worker.Cancel();
    EXPECT_TRUE(worker.IsJoinable());
    worker.Join();
    EXPECT_FALSE(worker.IsJoinable());
  }
}

}  // namespace streams_internal
}  // namespace visionai
