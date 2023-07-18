// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/gstreamer_async_jpeg_encoder.h"

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/gstreamer_async_decoder.h"
#include "visionai/algorithms/media/util/test_util.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/file_path.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
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
constexpr absl::string_view kH264CapsString = "video/x-h264";

constexpr char kTestImageLenaPath[] =
    "visionai/testing/testdata/media/jpegs/lena_color.jpg";
constexpr absl::string_view kJpegCapsString = "image/jpeg";

extern "C" {
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(jpeg);
GST_PLUGIN_STATIC_DECLARE(videoconvert);
GST_PLUGIN_STATIC_DECLARE(videoscale);
GST_PLUGIN_STATIC_DECLARE(libav);
GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
GST_PLUGIN_STATIC_DECLARE(multifile);
}

class GstreamerAsyncJpegEncoderTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    auto status = GstInit();
    ASSERT_TRUE(status.ok());
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(playback);
    GST_PLUGIN_STATIC_REGISTER(jpeg);
    GST_PLUGIN_STATIC_REGISTER(videoconvert);
    GST_PLUGIN_STATIC_REGISTER(videoscale);
    GST_PLUGIN_STATIC_REGISTER(libav);
    GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
    GST_PLUGIN_STATIC_REGISTER(multifile);
  }
};

}  // namespace

// Broken because caps isn't stored in the raw format?
TEST_F(GstreamerAsyncJpegEncoderTest, H264FrameSequenceTest) {
  absl::LeakCheckDisabler disabler;
  std::vector<std::pair<absl::StatusOr<GstreamerBuffer>, int>> results;
  OutputImageDimension dimension;
  dimension.width = 640;
  dimension.height = 320;
  std::string local_dir = file::JoinPath(::testing::TempDir(), "test_jpeg_dir");
  ASSERT_TRUE(CreateDir(local_dir).ok());
  GstreamerAsyncJpegEncoder<int> encoder(
      [&results](
          absl::StatusOr<GstreamerBuffer> jpeg_gstreamer_buffer_status_or,
          int timestamp) {
        if (jpeg_gstreamer_buffer_status_or.ok()) {
          results.push_back(std::make_pair(
              std::move(jpeg_gstreamer_buffer_status_or), timestamp));
        }
      },
      dimension,
      file::JoinPath(local_dir, "%d.jpg"));
  GstreamerAsyncDecoder<int> decoder(
      [&encoder](absl::StatusOr<GstreamerBuffer> image, int timestamp) {
        if (image.ok()) {
          absl::Status status = encoder.Feed(*image, timestamp);
          EXPECT_TRUE(status.ok());
          EXPECT_EQ(absl::OkStatus(), status);
        }
      });

  // Feed encoded frame 1.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 0);
    EXPECT_TRUE(status.ok());
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

  // Give time for the callback to return.
  decoder.WaitUntilCompleted(absl::Seconds(2));

  encoder.SignalEOS();

  encoder.WaitUntilCompleted(absl::Seconds(2));

  for (int i = 0; i < 4; i++) {
    // Test if the output is valid JPEG.
    EXPECT_TRUE(
        IsValidJpeg(results[i].first->data(), results[i].first->size()));
    EXPECT_EQ(results[i].second, i);
    // Test if the file has been saved and is a vaild JPEG.
    std::string file_path =
        file::JoinPath(local_dir, absl::StrCat(i, ".jpg"));
    std::string file_contents;
    ASSERT_TRUE(FileExists(file_path).ok());
    ASSERT_TRUE(GetFileContents(file_path, &file_contents).ok());
    VAI_ASSERT_OK_AND_ASSIGN(auto file_size, GetFileSize(file_path));
    ASSERT_TRUE(IsValidJpeg(file_contents.c_str(), file_size));
  }
}

TEST_F(GstreamerAsyncJpegEncoderTest, JPEGSequenceInputTest) {
  std::vector<std::pair<absl::StatusOr<GstreamerBuffer>, int>> results;
  OutputImageDimension dimension;
  dimension.width = 640;
  dimension.height = 320;
  GstreamerAsyncJpegEncoder<int> encoder(
      [&results](
          absl::StatusOr<GstreamerBuffer> jpeg_gstreamer_buffer_status_or,
          int timestamp) {
        if (jpeg_gstreamer_buffer_status_or.ok()) {
          results.push_back(std::make_pair(
              std::move(jpeg_gstreamer_buffer_status_or), timestamp));
        }
      },
      dimension);
  GstreamerAsyncDecoder<int> decoder(
      [&encoder](absl::StatusOr<GstreamerBuffer> image, int timestamp) {
        if (image.ok()) {
          absl::Status status = encoder.Feed(std::move(*image), timestamp);
          EXPECT_TRUE(status.ok());
          EXPECT_EQ(absl::OkStatus(), status);
        }
      });
  // Feed jpeg image.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 0);
    EXPECT_TRUE(status.ok());
  }

  decoder.SignalEOS();

  // Give time for the callback to return.
  decoder.WaitUntilCompleted(absl::Seconds(2));

  encoder.SignalEOS();

  encoder.WaitUntilCompleted(absl::Seconds(2));
  EXPECT_EQ(results.size(), 1);
  EXPECT_TRUE(IsValidJpeg(results[0].first->data(), results[0].first->size()));
  EXPECT_EQ(results[0].second, 0);
}

TEST_F(GstreamerAsyncJpegEncoderTest, JPEGSequenceInputWithNoResizeTest) {
  std::vector<std::pair<absl::StatusOr<GstreamerBuffer>, int>> results;
  GstreamerAsyncJpegEncoder<int> encoder(
      [&results](
          absl::StatusOr<GstreamerBuffer> jpeg_gstreamer_buffer_status_or,
          int timestamp) {
        if (jpeg_gstreamer_buffer_status_or.ok()) {
          results.push_back(std::make_pair(
              std::move(jpeg_gstreamer_buffer_status_or), timestamp));
        }
      });
  GstreamerAsyncDecoder<int> decoder(
      [&encoder](absl::StatusOr<GstreamerBuffer> image, int timestamp) {
        if (image.ok()) {
          absl::Status status = encoder.Feed(std::move(*image), timestamp);
          EXPECT_TRUE(status.ok());
          EXPECT_EQ(absl::OkStatus(), status);
        }
      });
  // Feed jpeg image.
  {
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
    absl::Status status = decoder.Feed(gstreamer_buffer, 0);
    EXPECT_TRUE(status.ok());
  }

  decoder.SignalEOS();

  // Give time for the callback to return.
  decoder.WaitUntilCompleted(absl::Seconds(2));

  encoder.SignalEOS();

  encoder.WaitUntilCompleted(absl::Seconds(2));
  EXPECT_EQ(results.size(), 1);
  EXPECT_TRUE(IsValidJpeg(results[0].first->data(), results[0].first->size()));
  EXPECT_EQ(results[0].second, 0);
}

}  // namespace visionai
