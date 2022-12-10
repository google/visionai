// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/examine_stack.h"

#include "gtest/gtest.h"
#include "absl/strings/match.h"

namespace visionai {

namespace {
TEST(ExamineStack, CurrentStackTrace) {
  std::string stack_trace = CurrentStackTrace();
  EXPECT_TRUE(
      absl::StrContains(stack_trace, "ExamineStack_CurrentStackTrace_Test"));
}
}  // namespace

}  // namespace visionai
