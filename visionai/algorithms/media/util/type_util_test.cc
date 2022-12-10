// Copyright 2020 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/type_util.h"

#include <string>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
extern "C"{
#include "third_party/ffmpeg/libavutil/motion_vector.h"
}
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/types/motion_vector.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

constexpr char kTestImageLenaPath[] =
    "visionai/testing/testdata/media/jpegs/lena_color.jpg";
constexpr char kTestImageSquaresPath[] =
    "visionai/testing/testdata/media/jpegs/squares_color.jpg";
constexpr char kJpegCapsString[] = "image/jpeg";
constexpr char kRgbPipeline[] =
    "decodebin ! videoconvert ! video/x-raw,format=RGB";
constexpr char kYuvPipeline[] = "decodebin";
constexpr char kRgbaPipeline[] =
    "decodebin ! videoconvert ! video/x-raw,format=RGBA";

absl::StatusOr<GstreamerBuffer> GstreamerBufferFromFile(
    const std::string& fname, const std::string& caps_string) {
  std::string file_contents;
  VAI_RETURN_IF_ERROR(GetFileContents(fname, &file_contents));
  GstreamerBuffer gstreamer_buffer;
  gstreamer_buffer.set_caps_string(caps_string);
  gstreamer_buffer.assign(std::move(file_contents));
  return gstreamer_buffer;
}

extern "C" {
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(jpeg);
GST_PLUGIN_STATIC_DECLARE(videoconvert);
}

class TypeUtilsTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    auto status = GstInit();
    ASSERT_TRUE(status.ok());
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(playback);
    GST_PLUGIN_STATIC_REGISTER(jpeg);
    GST_PLUGIN_STATIC_REGISTER(videoconvert);
  }
};

TEST_F(TypeUtilsTest, NoPaddingTest) {
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(1);

  {
    // Setup a pipeline to convert a jpeg into an RGB image.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kRgbPipeline;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Decode the jpeg into an RGB image and close the runner.
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
    EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
  }

  {
    // Get the decoded image and convert it to an RawImage.
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    auto raw_image_statusor = ToRawImage(std::move(gstreamer_buffer));
    EXPECT_TRUE(raw_image_statusor.ok());
    RawImage r = std::move(raw_image_statusor).value();
    EXPECT_EQ(r.format(), RawImage::Format::kSRGB);
    EXPECT_EQ(r.height(), 512);
    EXPECT_EQ(r.width(), 512);
    EXPECT_EQ(r.channels(), 3);
    EXPECT_EQ(r.size(), 786432);
  }
}

TEST_F(TypeUtilsTest, PaddingTest) {
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(1);

  {
    // Setup a pipeline to convert a jpeg into an RGB image.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kRgbPipeline;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Decode the jpeg into an RGB image and close the runner.
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString).value();
    EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
  }

  {
    // Get the decoded image and convert it to an RawImage.
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    EXPECT_EQ(gstreamer_buffer.size(), 177876);
    auto raw_image_statusor = ToRawImage(std::move(gstreamer_buffer));
    EXPECT_TRUE(raw_image_statusor.ok());
    RawImage r = std::move(raw_image_statusor).value();
    EXPECT_EQ(r.format(), RawImage::Format::kSRGB);
    EXPECT_EQ(r.height(), 243);
    EXPECT_EQ(r.width(), 243);
    EXPECT_EQ(r.channels(), 3);
    EXPECT_EQ(r.size(), 177147);
  }
}

TEST_F(TypeUtilsTest, YuvFailTest) {
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(1);

  {
    // Setup a pipeline to convert a jpeg into an YUV image.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kYuvPipeline;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Decode the jpeg into an YUV image and close the runner.
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString).value();
    EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
  }

  {
    // Get the decoded image and convert it to an RawImage.
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    auto raw_image_statusor = ToRawImage(std::move(gstreamer_buffer));
    EXPECT_FALSE(raw_image_statusor.ok());
    LOG(ERROR) << raw_image_statusor.status();
  }
}

TEST_F(TypeUtilsTest, RgbaFailTest) {
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(1);

  {
    // Setup a pipeline to convert a jpeg into an RGB image.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kRgbaPipeline;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Decode the jpeg into an RGB image and close the runner.
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString).value();
    EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
  }

  {
    // Get the decoded image and convert it to an RawImage.
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    auto raw_image_statusor = ToRawImage(std::move(gstreamer_buffer));
    EXPECT_FALSE(raw_image_statusor.ok());
    LOG(ERROR) << raw_image_statusor.status();
  }
}

TEST_F(TypeUtilsTest, NoPaddingRgbRawImageToGstreamerBuffer) {
  // Get a gstreamer raw image by decoding a JPEG.
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(1);
  {
    // Setup a pipeline to convert a jpeg into an RGB image.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kRgbPipeline;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Decode the jpeg into an RGB image and close the runner.
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
    EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
  }

  // Actual test starts here.
  {
    GstreamerBuffer gstreamer_buffer_src;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer_src, absl::Seconds(1)));
    auto raw_image_statusor = ToRawImage(gstreamer_buffer_src);
    EXPECT_TRUE(raw_image_statusor.ok());
    RawImage r_src = std::move(raw_image_statusor).value();
    EXPECT_EQ(r_src.format(), RawImage::Format::kSRGB);
    EXPECT_EQ(r_src.height(), 512);
    EXPECT_EQ(r_src.width(), 512);
    EXPECT_EQ(r_src.channels(), 3);
    EXPECT_EQ(r_src.size(), 786432);

    auto gstreamer_buffer_statusor = ToGstreamerBuffer(std::move(r_src));
    EXPECT_TRUE(gstreamer_buffer_statusor.ok());
    auto gstreamer_buffer_dst = std::move(gstreamer_buffer_statusor).value();
    raw_image_statusor = ToRawImage(std::move(gstreamer_buffer_dst));
    EXPECT_TRUE(raw_image_statusor.ok());
    RawImage r_dst = std::move(raw_image_statusor).value();
    EXPECT_EQ(r_dst.format(), RawImage::Format::kSRGB);
    EXPECT_EQ(r_dst.height(), 512);
    EXPECT_EQ(r_dst.width(), 512);
    EXPECT_EQ(r_dst.channels(), 3);
    EXPECT_EQ(r_dst.size(), 786432);
  }
}

TEST_F(TypeUtilsTest, PaddingRgbRawImageToGstreamerBuffer) {
  // Get a gstreamer raw image by decoding a JPEG.
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(1);
  {
    // Setup a pipeline to convert a jpeg into an RGB image.
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kRgbPipeline;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Decode the jpeg into an RGB image and close the runner.
    GstreamerBuffer gstreamer_buffer =
        GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString).value();
    EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
  }

  // Actual test starts here.
  {
    GstreamerBuffer gstreamer_buffer_src;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer_src, absl::Seconds(1)));
    auto raw_image_statusor = ToRawImage(gstreamer_buffer_src);
    EXPECT_TRUE(raw_image_statusor.ok());
    RawImage r_src = std::move(raw_image_statusor).value();
    EXPECT_EQ(r_src.format(), RawImage::Format::kSRGB);
    EXPECT_EQ(r_src.height(), 243);
    EXPECT_EQ(r_src.width(), 243);
    EXPECT_EQ(r_src.channels(), 3);
    EXPECT_EQ(r_src.size(), 177147);

    auto gstreamer_buffer_statusor = ToGstreamerBuffer(std::move(r_src));
    EXPECT_TRUE(gstreamer_buffer_statusor.ok());
    auto gstreamer_buffer_dst = std::move(gstreamer_buffer_statusor).value();
    raw_image_statusor = ToRawImage(std::move(gstreamer_buffer_dst));
    EXPECT_TRUE(raw_image_statusor.ok());
    RawImage r_dst = std::move(raw_image_statusor).value();
    EXPECT_EQ(r_dst.format(), RawImage::Format::kSRGB);
    EXPECT_EQ(r_dst.height(), 243);
    EXPECT_EQ(r_dst.width(), 243);
    EXPECT_EQ(r_dst.channels(), 3);
    EXPECT_EQ(r_dst.size(), 177147);
  }
}

TEST_F(TypeUtilsTest, MediaTypeFromCapsTest) {
  std::string caps;
  caps = "video/x-raw";
  EXPECT_EQ(MediaTypeFromCaps(caps), "video/x-raw");

  caps = "video/x-h264, width=(int)1280, height=(int)720";
  EXPECT_EQ(MediaTypeFromCaps(caps), "video/x-h264");
}

TEST(TypeUtils, GstreamerBufferToMotionVectors) {
  // Generate GstreamerBuffer with motion vectors.
  MotionVectors expected_mvs;
  expected_mvs.push_back({/* .source = */ 1,
                          /* .w = */ 8,
                          /* .h = */ 16,
                          /* .src_x = */ 10,
                          /* .src_y = */ 20,
                          /* .dst_x = */ 30,
                          /* .dst_y = */ 40,
                          /* .motion_x = */ 40,
                          /* .motion_y = */ 40,
                          /* .motion_scale = */ 2});

  expected_mvs.push_back({/* .source = */ -1,
                          /* .w = */ 16,
                          /* .h = */ 8,
                          /* .src_x = */ 50,
                          /* .src_y = */ 60,
                          /* .dst_x = */ 70,
                          /* .dst_y = */ 80,
                          /* .motion_x = */ -20,
                          /* .motion_y = */ -20,
                          /* .motion_scale = */ 1});

  std::vector<AVMotionVector> av_mvs;

  for (int i = 0; i < expected_mvs.size(); i++) {
    av_mvs.push_back({/* .source = */ expected_mvs.at(i).source,
                      /* .w = */ expected_mvs.at(i).w,
                      /* .h = */ expected_mvs.at(i).h,
                      /* .src_x = */ expected_mvs.at(i).src_x,
                      /* .src_y = */ expected_mvs.at(i).src_y,
                      /* .dst_x = */ expected_mvs.at(i).dst_x,
                      /* .dst_y = */ expected_mvs.at(i).dst_y,
                      /* .flags = */ 0,
                      /* .motion_x = */ expected_mvs.at(i).motion_x,
                      /* .motion_y = */ expected_mvs.at(i).motion_y});
  }
  GstreamerBuffer gstreamer_buffer;
  gstreamer_buffer.assign(reinterpret_cast<char*>(av_mvs.data()),
                          expected_mvs.size() * sizeof(AVMotionVector));

  // Test conversion from GstreamerBuffer to MotionVectors.
  auto motion_vectors_statusor = ToMotionVectors(gstreamer_buffer);
  ASSERT_TRUE(motion_vectors_statusor.ok());

  ASSERT_TRUE(motion_vectors_statusor.ok());
  MotionVectors mvs = std::move(motion_vectors_statusor.value());
  EXPECT_EQ(mvs.size(), expected_mvs.size());
  for (int i = 0; i < expected_mvs.size(); i++) {
    EXPECT_EQ(mvs.at(i).source, expected_mvs.at(i).source);
    EXPECT_EQ(mvs.at(i).w, expected_mvs.at(i).w);
    EXPECT_EQ(mvs.at(i).h, expected_mvs.at(i).h);
    EXPECT_EQ(mvs.at(i).src_x, expected_mvs.at(i).src_x);
    EXPECT_EQ(mvs.at(i).src_y, expected_mvs.at(i).src_y);
    EXPECT_EQ(mvs.at(i).dst_x, expected_mvs.at(i).dst_x);
    EXPECT_EQ(mvs.at(i).dst_y, expected_mvs.at(i).dst_y);
  }
}

}  // namespace
}  // namespace visionai
