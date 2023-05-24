// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/file_source_capture.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/capture_module.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_path.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

namespace {

const char kTestFolder[] = "visionai/testing/testdata/media/";

class FileSourceCaptureTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { ASSERT_TRUE(GstRegisterPlugins().ok()); }

  void ExpectGstBufferPacket(
      const Packet& packet, int64_t expected_pts_seconds,
      int64_t expected_pts_nanos, int64_t expected_dts_seconds,
      int64_t expected_dts_nanos, int64_t expected_duration_seconds,
      int64_t expected_duration_nanos, int64_t expected_payload_size) {
    EXPECT_EQ(packet.header().type().type_class(), "gst");
    EXPECT_EQ(packet.header().type().type_descriptor().type(), "video/x-h264");
    EXPECT_EQ(packet.header().type().type_descriptor()
              .gstreamer_buffer_descriptor().pts_time().seconds(),
              expected_pts_seconds);
    EXPECT_EQ(packet.header().type().type_descriptor()
              .gstreamer_buffer_descriptor().pts_time().nanos(),
              expected_pts_nanos);
    EXPECT_EQ(packet.header().type().type_descriptor()
              .gstreamer_buffer_descriptor().dts_time().seconds(),
              expected_dts_seconds);
    EXPECT_EQ(packet.header().type().type_descriptor()
              .gstreamer_buffer_descriptor().dts_time().nanos(),
              expected_dts_nanos);
    EXPECT_EQ(packet.header().type().type_descriptor()
              .gstreamer_buffer_descriptor().duration().seconds(),
              expected_duration_seconds);
    EXPECT_EQ(packet.header().type().type_descriptor()
              .gstreamer_buffer_descriptor().duration().nanos(),
              expected_duration_nanos);
    EXPECT_EQ(packet.payload().size(), expected_payload_size);
  }
};

}  // namespace

TEST_F(FileSourceCaptureTest, RunTestEmptyFilepath) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add("");
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  EXPECT_EQ(capture.Init(), absl::InvalidArgumentError(
    "Given an empty filepath; while initializing the Capture"));
}

TEST_F(FileSourceCaptureTest, RunTestNoSuchFile) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "no_such_file.mp4"));
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  EXPECT_EQ(capture.Init(), absl::InvalidArgumentError(
      absl::StrFormat("No such file \"%s\"; while initializing the Capture",
      file::JoinPath(kTestFolder, "no_such_file.mp4"))));
}

TEST_F(FileSourceCaptureTest, RunTest) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(capture.Run().ok());

  // There are four frames in the test video.
  const int expected_frames = 4;
  ASSERT_EQ(output->count(), expected_frames);
  Packet packet;
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 0, 0, 0, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 0, 500000000, 0, 500000000, 0, 500000000,
                        17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 1, 0, 1, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 1, 500000000, 1, 500000000, 0, 500000000,
                        17884);
  ASSERT_FALSE(output->TryPopBack(packet));
}

TEST_F(FileSourceCaptureTest, RunTestLoopOnce) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "1";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(capture.Run().ok());

  // There are four frames in the test video.
  const int expected_frames = 4;
  ASSERT_EQ(output->count(), expected_frames);
  Packet packet;
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 0, 0, 0, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 0, 500000000, 0, 500000000, 0, 500000000,
                        17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 1, 0, 1, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 1, 500000000, 1, 500000000, 0, 500000000,
                        17884);
  ASSERT_FALSE(output->TryPopBack(packet));
}

TEST_F(FileSourceCaptureTest, RunTestLoopMultipleTimes) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "3";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(capture.Run().ok());

  // There are four frames in the test video.
  const int expected_frames = 12;
  ASSERT_EQ(output->count(), expected_frames);
  Packet packet;
  // First iteration.
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 0, 0, 0, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 0, 500000000, 0, 500000000, 0, 500000000,
                        17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 1, 0, 1, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 1, 500000000, 1, 500000000, 0, 500000000,
                        17884);
  // Second iteration. The timestamps still accumulates.
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 2, 0, 2, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 2, 500000000, 2, 500000000, 0, 500000000,
                        17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 3, 0, 3, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 3, 500000000, 3, 500000000, 0, 500000000,
                        17884);
  // Third iteration. The timestamps still accumulates.
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 4, 0, 4, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 4, 500000000, 4, 500000000, 0, 500000000,
                        17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 5, 0, 5, 0, 0, 500000000, 17884);
  ASSERT_TRUE(output->TryPopBack(packet));
  ExpectGstBufferPacket(packet, 5, 500000000, 5, 500000000, 0, 500000000,
                        17884);
  ASSERT_FALSE(output->TryPopBack(packet));
}

TEST_F(FileSourceCaptureTest, RunTestNegativeLoopCount) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(capture.Run().ok());
  ASSERT_TRUE(capture.Cancel().ok());

  // There are four frames in the test video.
  const int expected_frames = 0;
  ASSERT_EQ(output->count(), expected_frames);
  Packet packet;
  ASSERT_FALSE(output->TryPopBack(packet));
}

TEST_F(FileSourceCaptureTest, NegativeTimeoutValue) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));
  (*config.mutable_attr())["timeout_sec"] = "-5";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(absl::IsInvalidArgument(capture.Init()));
}

TEST_F(FileSourceCaptureTest, TerminatedByTimeout) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "5";
  (*config.mutable_attr())["timeout_sec"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(absl::IsDeadlineExceeded(capture.Run()));
}

TEST_F(FileSourceCaptureTest, RunLoopCancelled) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));
  (*config.mutable_attr())["loop"] = "true";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);

  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  std::thread t([&capture]() -> void {
    absl::SleepFor(absl::Seconds(1));
    ASSERT_TRUE(capture.Cancel().ok());
    return;
  });
  EXPECT_EQ(capture.Run(), absl::OkStatus());
  t.join();
}

TEST_F(FileSourceCaptureTest, RunH265Input) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "h265.mp4"));
  (*config.mutable_attr())["loop"] = "true";
  (*config.mutable_attr())["loop_count"] = "0";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);

  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  EXPECT_EQ(capture.Run(), absl::FailedPreconditionError(
    "The input media type - \"video/x-h265\" is not supported. "
    "Currently the only supported media type is \"video/x-h264\""));
}
}  // namespace visionai
