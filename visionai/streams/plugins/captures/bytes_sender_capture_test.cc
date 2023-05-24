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

#include "visionai/streams/plugins/captures/bytes_sender_capture.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/streams/capture_module.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

class BytesSenderCaptureTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { ASSERT_TRUE(GstRegisterPlugins().ok()); }

  void ExpectPackets(std::shared_ptr<RingBuffer<Packet>> output) {
    // There are five packets in the output buffer.
    // 5.5 seconds / 1000 milliseconds = 5
    const int expected_packets = 5;
    ASSERT_EQ(output->count(), expected_packets);
    for (int i = 0; i < expected_packets; i++) {
      Packet packet;
      ASSERT_TRUE(output->TryPopBack(packet));
      std::string expected_payload(".");
      EXPECT_EQ(packet.payload(), expected_payload);
    }
  }
};

TEST_F(BytesSenderCaptureTest, RunTestNonPositiveBytesPerMessage) {
  CaptureConfig config;
  config.mutable_name()->assign("BytesSenderCapture");
  (*config.mutable_attr())["bytes_per_message"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_EQ(capture.Init(), absl::InvalidArgumentError(
        "The bytes per message must be positive. Got 0.; "
        "while initializing the Capture"));
}

TEST_F(BytesSenderCaptureTest, RunTestNonPositiveSendPeriodMS) {
  CaptureConfig config;
  config.mutable_name()->assign("BytesSenderCapture");
  (*config.mutable_attr())["bytes_per_message"] = "1";
  (*config.mutable_attr())["send_period_ms"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_EQ(capture.Init(), absl::InvalidArgumentError(
        "The send period must be positive. Got 0.; "
        "while initializing the Capture"));
}

TEST_F(BytesSenderCaptureTest, RunTest) {
  CaptureConfig config;
  config.mutable_name()->assign("BytesSenderCapture");
  (*config.mutable_attr())["bytes_per_message"] = "1";
  (*config.mutable_attr())["send_period_ms"] = "1000";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  auto worker = std::make_unique<streams_internal::Worker>();
  ASSERT_TRUE(worker->Work([&]() {
    return capture.Run();
  }).ok());

  absl::SleepFor(absl::Milliseconds(5500));
  ASSERT_TRUE(capture.Cancel().ok());
  ExpectPackets(output);
}
}  // namespace visionai
