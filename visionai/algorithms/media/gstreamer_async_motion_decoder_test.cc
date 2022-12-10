// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/gstreamer_async_motion_decoder.h"

#include <memory>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/test_util.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/types/gstreamer_buffer.h"

namespace visionai {
namespace {

constexpr absl::string_view kEncodedFrame1Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-1.264";
constexpr absl::string_view kEncodedFrame2Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-2.264";
constexpr absl::string_view kEncodedFrame3Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-3.264";
constexpr absl::string_view kEncodedFrame4Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-4.264";
constexpr absl::string_view kH264CapsString =
    "video/x-h264, stream-format=(string)avc, alignment=(string)au, "
    "level=(string)1.3, profile=(string)high, "
    "codec_data=(buffer)"
    "0164000dffe100186764000dacd94161fb016c80000003008000001e078a14cb01000668eb"
    "e3cb22c0, width=(int)352, height=(int)240, framerate=(fraction)30/1, "
    "pixel-aspect-ratio=(fraction)1/1, chroma-format=(string)4:2:0, "
    "bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, parsed=(boolean)true";


class GstreamerAsyncMotionDecoderTest : public testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    ASSERT_TRUE(GstRegisterPlugins().ok());
  }
};

TEST_F(GstreamerAsyncMotionDecoderTest, H264SequenceTest) {
  absl::LeakCheckDisabler disabler;
  std::vector<std::pair<absl::StatusOr<MotionVectors>, int>> results;
  GstreamerAsyncMotionDecoder<int> decoder(
      [&results](absl::StatusOr<MotionVectors> motion_vectors,
                 int timestamp) -> absl::Status {
        VAI_RETURN_IF_ERROR(motion_vectors.status());
        results.emplace_back(std::move(motion_vectors), timestamp);
        return absl::OkStatus();
      });

  // Feed encoded frame 1.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 0);
    EXPECT_TRUE(status.ok());
    LOG(ERROR) << "OK";
  }
  // Feed encoded frame 2.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame2Path, kH264CapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 1);
    EXPECT_TRUE(status.ok());
  }
  // Feed encoded frame 3.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame3Path, kH264CapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 2);
    EXPECT_TRUE(status.ok());
  }
  // Feed encoded frame 4.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame4Path, kH264CapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 3);
    EXPECT_TRUE(status.ok());
  }

  decoder.SignalEOS();
  decoder.WaitUntilCompleted(absl::Seconds(1));

  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(results[i].first.status().ok());
    if (i == 0) {
      // No motion vectors for the first frame since it's an I-frame.
      EXPECT_EQ(results[i].first->size(), 0);
    } else {
      // Motion vectors exists for the following frames.
      EXPECT_GT(results[i].first->size(), 0);
    }
    EXPECT_EQ(results[i].second, i);
  }
}

}  // namespace
}  // namespace visionai
