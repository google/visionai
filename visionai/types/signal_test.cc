// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/types/signal.h"

#include <memory>
#include <string>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

TEST(SignalTest, ConstructorTest) {
  {
    Signal s;
    EXPECT_EQ(s.code(), Signal::SignalCode::kUnknown);
  }

  {
    Signal s(Signal::SignalCode::kPhantom);
    EXPECT_EQ(s.code(), Signal::SignalCode::kPhantom);
  }
}

TEST(SignalTest, StringRepresentationTest) {
  {
    Signal s;
    auto code_string = ToString(s.code());
    EXPECT_TRUE(code_string.ok());
    EXPECT_EQ(*code_string, "unknown");
  }
  {
    Signal s(Signal::SignalCode::kPhantom);
    auto code_string = ToString(s.code());
    EXPECT_TRUE(code_string.ok());
    EXPECT_EQ(*code_string, "phantom");
  }
  {
    Signal s(Signal::SignalCode::kEOS);
    auto code_string = ToString(s.code());
    EXPECT_TRUE(code_string.ok());
    EXPECT_EQ(*code_string, "eos");
  }
  {
    std::string code_string = "unknown";
    auto code = ToSignalCode(code_string);
    EXPECT_TRUE(code.ok());
    EXPECT_EQ(*code, Signal::SignalCode::kUnknown);
  }
  {
    std::string code_string = "phantom";
    auto code = ToSignalCode(code_string);
    EXPECT_TRUE(code.ok());
    EXPECT_EQ(*code, Signal::SignalCode::kPhantom);
  }
  {
    std::string code_string = "eos";
    auto code = ToSignalCode(code_string);
    EXPECT_TRUE(code.ok());
    EXPECT_EQ(*code, Signal::SignalCode::kEOS);
  }
  {
    std::string code_string = "foo";
    auto code = ToSignalCode(code_string);
    EXPECT_FALSE(code.ok());
  }
}

}  // namespace visionai
