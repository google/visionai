// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/rtsp_image_capture.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/streams/capture_module.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

using ::testing::_;

class RTSPImageCaptureTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { ASSERT_TRUE(GstRegisterPlugins().ok()); }
};

TEST_F(RTSPImageCaptureTest, RunTest) {
  CaptureConfig config;
  config.mutable_name()->assign("RTSPImageCapture");
  // TODO: Avoid calling the external rtsp server - spin up a localhost rtsp
  // server instead.
  config.mutable_source_urls()->Add(
      "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4");
  config.mutable_attr()->operator[]("max_captured_images") = "15";
  config.mutable_attr()->operator[]("frame_rate") = "10/1";
  config.mutable_attr()->operator[]("camera_id") = "cam-1";

  CaptureModule capture(config);
  auto output = std::make_shared<RingBuffer<Packet>>(100);
  capture.AttachOutput(output);
  ASSERT_TRUE(capture.Prepare().ok());
  ASSERT_TRUE(capture.Init().ok());
  ASSERT_TRUE(capture.Run().ok());

  ASSERT_EQ(output->count(), 15);
}

}  // namespace visionai
