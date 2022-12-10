// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/picked_notification.h"

#include <thread>

#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

namespace visionai {
namespace streams_internal {

TEST(PickedNotificationTest, BasicTest) {
  auto notification = std::make_shared<PickedNotification>();
  std::thread t1([notification]() { notification->Notify(42); });
  notification->WaitForNotification();
  EXPECT_EQ(notification->GetPicked(), 42);
  t1.join();
}

}  // namespace streams_internal
}  // namespace visionai
