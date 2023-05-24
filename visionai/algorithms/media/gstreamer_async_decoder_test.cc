// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/gstreamer_async_decoder.h"

#include <string>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstregistry.h"
#include "visionai/algorithms/media/util/test_util.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/raw_image.h"

namespace visionai {

namespace {

constexpr absl::string_view kTestImageLenaPath =
    "visionai/testing/testdata/media/jpegs/lena_color.jpg";
constexpr absl::string_view kTestImageSquaresPath =
    "visionai/testing/testdata/media/jpegs/squares_color.jpg";
constexpr absl::string_view kTestImageGooglePath =
    "visionai/testing/testdata/media/pngs/google_logo.png";
constexpr absl::string_view kEncodedFrame1Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "page-brin-frame-1.264";
constexpr absl::string_view kEncodedFrame2Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "page-brin-frame-2.264";
constexpr absl::string_view kEncodedFrame3Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "page-brin-frame-3.264";
constexpr absl::string_view kEncodedFrame4Path =
    "visionai/testing/testdata/media/encoded-frames/"
    "page-brin-frame-4.264";
constexpr absl::string_view kPngCapsString = "image/png";
constexpr absl::string_view kJpegCapsString = "image/jpeg";
constexpr absl::string_view kH264CapsString = "video/x-h264";
constexpr int64_t kNanosPerSecond = 1000000000;

using testing::Not;
using testing::SizeIs;

}  // namespace

extern "C" {
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(jpeg);
GST_PLUGIN_STATIC_DECLARE(libav);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(videoconvert);
GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
}

class GstreamerAsyncDecoderTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    auto status = GstInit();
    ASSERT_TRUE(status.ok());
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(jpeg);
    GST_PLUGIN_STATIC_REGISTER(libav);
    GST_PLUGIN_STATIC_REGISTER(playback);
    GST_PLUGIN_STATIC_REGISTER(videoconvert);
    GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
  }
};

TEST_F(GstreamerAsyncDecoderTest, JpegSequenceTest) {
  absl::LeakCheckDisabler disabler;
  std::vector<absl::StatusOr<RawImage>> results;
  GstreamerAsyncDecoder<> decoder([&results](absl::StatusOr<RawImage> image) {
    results.push_back(std::move(image));
  });

  // Feed the jpeg.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
    ASSERT_TRUE(decoder.Feed(gstreamer_buffer).ok());

    // Give time for the callback to return.
    absl::SleepFor(absl::Seconds(1));

    ASSERT_THAT(results, SizeIs(1));
    EXPECT_EQ(results[0]->format(), RawImage::Format::kSRGB);
    EXPECT_EQ(results[0]->height(), 512);
    EXPECT_EQ(results[0]->width(), 512);
    EXPECT_EQ(results[0]->channels(), 3);
    EXPECT_EQ(results[0]->size(), 786432);
  }

  // Feed a png. This should fail.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageGooglePath, kPngCapsString).value();
    EXPECT_THAT(decoder.Feed(gstreamer_buffer), Not(IsOk()));
  }

  // Feed the jpeg again.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString).value();
    ASSERT_TRUE(decoder.Feed(gstreamer_buffer).ok());

    // Give time for the callback to return.
    absl::SleepFor(absl::Seconds(1));

    ASSERT_THAT(results, SizeIs(2));
    EXPECT_EQ(results[1]->format(), RawImage::Format::kSRGB);
    EXPECT_EQ(results[1]->height(), 243);
    EXPECT_EQ(results[1]->width(), 243);
    EXPECT_EQ(results[1]->channels(), 3);
    EXPECT_EQ(results[1]->size(), 177147);
  }

  // Signal EOS.
  decoder.SignalEOS();

  // Feed the jpeg again. This should fail.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString).value();
    EXPECT_THAT(decoder.Feed(gstreamer_buffer),
                StatusIs(absl::StatusCode::kResourceExhausted));
  }
}

// NOTE: This test may be flaky. If it is, just delete.
TEST_F(GstreamerAsyncDecoderTest, TimeoutTest) {
  absl::LeakCheckDisabler disabler;
  // Set the timeout to 0 seconds and queue size to 1 to force a timeout when
  // given 2+ inputs in immediate succession.
  GstreamerAsyncDecoder<> decoder(
      [](absl::StatusOr<RawImage> image) { return; }, 1, absl::ZeroDuration());

  GstreamerBuffer gstreamer_buffer1 =
      GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString).value();
  GstreamerBuffer gstreamer_buffer2 =
      GstreamerBufferFromFile(kEncodedFrame2Path, kH264CapsString).value();

  EXPECT_THAT(decoder.Feed(gstreamer_buffer1), IsOk());
  EXPECT_THAT(decoder.Feed(gstreamer_buffer2),
              StatusIs(absl::StatusCode::kDeadlineExceeded));
}

TEST_F(GstreamerAsyncDecoderTest, H264SequenceTest) {
  absl::LeakCheckDisabler disabler;
  std::vector<std::pair<absl::StatusOr<RawImage>, int>> results;
  GstreamerAsyncDecoder<int> decoder(
      [&results](absl::StatusOr<RawImage> image, int timestamp) {
        if (image.ok()) {
          results.push_back(std::make_pair(std::move(image), timestamp));
        }
      });

  // Feed encoded frame 1.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString).value();
    ASSERT_THAT(decoder.Feed(gstreamer_buffer, 0), IsOk());
  }
  // Feed encoded frame 2.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame2Path, kH264CapsString).value();
    ASSERT_THAT(decoder.Feed(gstreamer_buffer, 1), IsOk());
  }
  // Feed encoded frame 3.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame3Path, kH264CapsString).value();
    ASSERT_THAT(decoder.Feed(gstreamer_buffer, 2), IsOk());
  }
  // Feed encoded frame 4.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame4Path, kH264CapsString).value();
    ASSERT_THAT(decoder.Feed(gstreamer_buffer, 3), IsOk());
  }

  decoder.SignalEOS();
  decoder.WaitUntilCompleted(absl::Seconds(1));

  ASSERT_THAT(results, SizeIs(4));

  for (int i = 0; i < 4; i++) {
    EXPECT_EQ(results[i].first->format(), RawImage::Format::kSRGB);
    EXPECT_EQ(results[i].first->height(), 300);
    EXPECT_EQ(results[i].first->width(), 460);
    EXPECT_EQ(results[i].first->channels(), 3);
    EXPECT_EQ(results[i].first->size(), 414000);
    EXPECT_EQ(results[i].second, i);
  }
}

TEST_F(GstreamerAsyncDecoderTest, LimitFrameRateTest) {
  absl::LeakCheckDisabler disabler;
  std::vector<std::pair<absl::StatusOr<GstreamerBuffer>, int>> results;
  GstreamerAsyncDecoder<int> decoder(
      [&results](absl::StatusOr<GstreamerBuffer> gbuf, int timestamp) {
        if (gbuf.ok()) {
          results.push_back({std::move(gbuf), timestamp});
        }
      },
      /*queue_size=*/30,
      /*feed_timeout=*/absl::Seconds(60),
      /*output_period_nanos=*/1 * kNanosPerSecond);

  // Feed encoded frame 1.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString, 0, 0, 0)
            .value();
    ASSERT_TRUE(decoder.Feed(gstreamer_buffer, 0).ok());
  }
  // Feed encoded frame 2.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame2Path, kH264CapsString, 1, 1, 0)
            .value();
    ASSERT_TRUE(decoder.Feed(gstreamer_buffer, 1).ok());
  }
  // Feed encoded frame 3.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame3Path, kH264CapsString, 2, 2, 0)
            .value();
    ASSERT_TRUE(decoder.Feed(gstreamer_buffer, 2).ok());
  }
  // Feed encoded frame 4.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame4Path, kH264CapsString, 3, 3, 0)
            .value();
    ASSERT_TRUE(decoder.Feed(gstreamer_buffer, 3).ok());
  }

  decoder.SignalEOS();
  decoder.WaitUntilCompleted(absl::Seconds(2));

  // Only one frame is kept because we set `output_fps` as 1.
  ASSERT_THAT(results, SizeIs(1));
  EXPECT_EQ(results[0].second, 0);
}

TEST_F(GstreamerAsyncDecoderTest, LegalOutputPeriodTest) {
  absl::LeakCheckDisabler disabler;
  std::vector<std::pair<absl::StatusOr<GstreamerBuffer>, int>> results;
  GstreamerAsyncDecoder<int> decoder(
      [&results](absl::StatusOr<GstreamerBuffer> gbuf, int timestamp) {
        results.push_back(std::make_pair(std::move(gbuf), timestamp));
      },
      /*queue_size=*/30,
      /*feed_timeout=*/absl::Seconds(60),
      /*output_period_nanos=*/-1);

  // Feed encoded frame 1.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString).value();
    EXPECT_THAT(decoder.Feed(gstreamer_buffer, 0),
                StatusIs(absl::StatusCode::kInvalidArgument));
  }
}

}  // namespace visionai
