// Copyright 2020 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/completion_signal.h"

#include <thread>  // NOLINT

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/time.h"

namespace visionai {

TEST(CompletionSignal, BasicTest) {
  CompletionSignal signal;
  signal.Start();
  std::thread worker([&signal]() {
    LOG(INFO) << "sleeping...";
    absl::SleepFor(absl::Seconds(2));
    LOG(INFO) << "awake...";
    signal.SetStatus(absl::UnknownError("Bogus error"));
    signal.End();
  });
  while (!signal.WaitUntilCompleted(absl::Seconds(1))) {
    LOG(INFO) << signal.IsCompleted();
    LOG(INFO) << "waiting...";
  }
  EXPECT_FALSE(signal.GetStatus().ok());
  worker.join();
}

}  // namespace visionai
