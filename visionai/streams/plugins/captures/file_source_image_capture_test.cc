// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/file_source_image_capture.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/codec_validator.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/capture_module.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_path.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

namespace {

const char kTestFolder[] = "visionai/testing/testdata/media/";

class FileSourceImageCaptureTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { ASSERT_TRUE(GstRegisterPlugins().ok()); }
};

}  // namespace

TEST_F(FileSourceImageCaptureTest, RunTestEmptyFilepath) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceImageCapture");
  config.mutable_source_urls()->Add("");

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  EXPECT_EQ(capture.Init(),
            absl::InvalidArgumentError(
                "Given an empty filepath; while initializing the Capture"));
}

TEST_F(FileSourceImageCaptureTest, RunTestNoSuchFile) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceImageCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "no_such_file.mp4"));

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  EXPECT_EQ(capture.Init(),
            absl::InvalidArgumentError(absl::StrFormat(
                "No such file \"%s\"; while initializing the Capture",
                file::JoinPath(kTestFolder, "no_such_file.mp4"))));
}

TEST_F(FileSourceImageCaptureTest, RunLoopCancelled) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceImageCapture");
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

TEST_F(FileSourceImageCaptureTest, RunH265Input) {
  if (!IsH265Supported()) {
    GTEST_SKIP();
  }
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceImageCapture");
  config.mutable_source_urls()->Add(file::JoinPath(kTestFolder, "h265.mp4"));

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);

  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  EXPECT_EQ(capture.Run(), absl::OkStatus());
}

TEST_F(FileSourceImageCaptureTest, RunAV1Input) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceImageCapture");
  config.mutable_source_urls()->Add(file::JoinPath(kTestFolder, "av1.mp4"));

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);

  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  EXPECT_THAT(
      capture.Run(),
      StatusIs(
          absl::StatusCode::kFailedPrecondition,
          testing::HasSubstr(
              "The input media type - \"video/x-av1\" is not supported.")));
}

TEST_F(FileSourceImageCaptureTest, RunTest) {
  CaptureConfig config;
  config.mutable_name()->assign("FileSourceImageCapture");
  config.mutable_source_urls()->Add(
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4"));

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(capture.Run().ok());
  ASSERT_TRUE(capture.Cancel().ok());

  // There are four frames in the test video.
  const int expected_frames = 4;
  ASSERT_EQ(output->count(), expected_frames);
}

}  // namespace visionai
