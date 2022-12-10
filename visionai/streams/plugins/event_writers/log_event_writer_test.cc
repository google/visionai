// Copyright 2022 Google LLC
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "visionai/streams/plugins/event_writers/log_event_writer.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "testing/base/public/mock-log.h"
#include "absl/debugging/leak_check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"

namespace visionai {

using ::base_logging::INFO;
using ::testing::_;
using ::testing::kDoNotCaptureLogsYet;
using ::testing::ScopedMockLog;

constexpr absl::string_view kTestEventId = "some-event-id";

class LogEventWriterTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
  }
};

TEST_F(LogEventWriterTest, WriteLog) {
  Packet p;

  // Create a basic string Packet
  std::string s("hello!");
  auto original_packet = MakePacket(s);
  absl::Time t = absl::FromUnixSeconds(123) + absl::Nanoseconds(456);
  EXPECT_TRUE(SetCaptureTime(t, &original_packet.value()).ok());

  std::unique_ptr<LogEventWriter> writer = std::make_unique<LogEventWriter>();
  std::unique_ptr<EventWriterInitContext> init_ctx =
      std::make_unique<EventWriterInitContext>();
  ASSERT_TRUE(writer->Init(init_ctx.get()).ok());
  ASSERT_TRUE(writer->Open(kTestEventId).ok());
  ScopedMockLog log(kDoNotCaptureLogsYet);
  EXPECT_CALL(log,
      Log(INFO, _, absl::StrCat(absl::StrFormat("(%s) ", kTestEventId),
                                p.DebugString())))
      .Times(1);
  log.StartCapturingLogs();
  ASSERT_TRUE(writer->Write(p).ok());
  log.StopCapturingLogs();
  ASSERT_TRUE(writer->Close().ok());
}

}  // namespace visionai
