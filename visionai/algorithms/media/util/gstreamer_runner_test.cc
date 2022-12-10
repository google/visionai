// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/gstreamer_runner.h"

#include <string>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstregistry.h"
#include "visionai/algorithms/media/util/test_util.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

constexpr char kTestImageLenaPath[] =
    "visionai/testing/testdata/media/jpegs/lena_color.jpg";
constexpr char kTestImageSquaresPath[] =
    "visionai/testing/testdata/media/jpegs/squares_color.jpg";
constexpr char kTestImageGooglePath[] =
    "visionai/testing/testdata/media/pngs/google_logo.png";
constexpr char kEncodedFrame1Path[] =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-1.264";
constexpr char kEncodedFrame2Path[] =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-2.264";
constexpr char kEncodedFrame3Path[] =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-3.264";
constexpr char kEncodedFrame4Path[] =
    "visionai/testing/testdata/media/encoded-frames/"
    "shadow-cubicle-frame-4.264";
constexpr char kJpegCapsString[] = "image/jpeg";
constexpr char kPngCapsString[] = "image/png";
constexpr char kH264CapsString[] =
    "video/x-h264, stream-format=(string)avc, alignment=(string)au, "
    "level=(string)1.3, profile=(string)high, "
    "codec_data=(buffer)"
    "0164000dffe100186764000dacd94161fb016c80000003008000001e078a14cb01000668eb"
    "e3cb22c0, width=(int)352, height=(int)240, framerate=(fraction)30/1, "
    "pixel-aspect-ratio=(fraction)1/1, chroma-format=(string)4:2:0, "
    "bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, parsed=(boolean)true";
constexpr char kProcessingPipelineString[] =
    "decodebin ! videoconvert ! video/x-raw,format=RGB";
}

extern "C" {
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(jpeg);
GST_PLUGIN_STATIC_DECLARE(videoconvert);
GST_PLUGIN_STATIC_DECLARE(libav);
}

class GstreamerRunnerTest : public ::testing::Test {
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
    GST_PLUGIN_STATIC_REGISTER(libav);
  }
};

TEST_F(GstreamerRunnerTest, JpegFeederTest) {
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(10);

  // Options to process Jpeg images.
  {
    GstreamerRunner::Options options;
    options.processing_pipeline_string = kProcessingPipelineString;
    options.appsrc_caps_string = kJpegCapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();

    // Feed the first jpeg.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    // Feed a different jpeg.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kTestImageSquaresPath, kJpegCapsString)
              .value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    // Feed a png. This should fail.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kTestImageGooglePath, kPngCapsString).value();
      EXPECT_FALSE(runner->Feed(gstreamer_buffer).ok());
    }

    // Feed the first jpeg again.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kTestImageLenaPath, kJpegCapsString).value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }
  }

  // Verify the results.
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    LOG(INFO) << "Caps: " << gstreamer_buffer.caps_cstr();

    GstCaps* gst_caps = gst_caps_from_string(gstreamer_buffer.caps_cstr());
    GstStructure* structure = gst_caps_get_structure(gst_caps, 0);
    std::string media_type(gst_structure_get_name(structure));
    int height, width;
    EXPECT_TRUE(gst_structure_get_int(structure, "height", &height) == TRUE);
    EXPECT_TRUE(gst_structure_get_int(structure, "width", &width) == TRUE);
    std::string format(gst_structure_get_string(structure, "format"));
    gst_caps_unref(gst_caps);

    EXPECT_EQ(media_type, "video/x-raw");
    EXPECT_EQ(height, 512);
    EXPECT_EQ(width, 512);
    EXPECT_EQ(format, "RGB");
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    LOG(INFO) << "Caps: " << gstreamer_buffer.caps_cstr();

    GstCaps* gst_caps = gst_caps_from_string(gstreamer_buffer.caps_cstr());
    GstStructure* structure = gst_caps_get_structure(gst_caps, 0);
    std::string media_type(gst_structure_get_name(structure));
    int height, width;
    EXPECT_TRUE(gst_structure_get_int(structure, "height", &height) == TRUE);
    EXPECT_TRUE(gst_structure_get_int(structure, "width", &width) == TRUE);
    std::string format(gst_structure_get_string(structure, "format"));
    gst_caps_unref(gst_caps);

    EXPECT_EQ(media_type, "video/x-raw");
    EXPECT_EQ(height, 243);
    EXPECT_EQ(width, 243);
    EXPECT_EQ(format, "RGB");
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    LOG(INFO) << "Caps: " << gstreamer_buffer.caps_cstr();

    GstCaps* gst_caps = gst_caps_from_string(gstreamer_buffer.caps_cstr());
    GstStructure* structure = gst_caps_get_structure(gst_caps, 0);
    std::string media_type(gst_structure_get_name(structure));
    int height, width;
    EXPECT_TRUE(gst_structure_get_int(structure, "height", &height) == TRUE);
    EXPECT_TRUE(gst_structure_get_int(structure, "width", &width) == TRUE);
    std::string format(gst_structure_get_string(structure, "format"));
    gst_caps_unref(gst_caps);

    EXPECT_EQ(media_type, "video/x-raw");
    EXPECT_EQ(height, 512);
    EXPECT_EQ(width, 512);
    EXPECT_EQ(format, "RGB");
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_FALSE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
  }
}

TEST_F(GstreamerRunnerTest, FeedFetchAppsrcDoTimestampFalseTest) {
  ProducerConsumerQueue<GstreamerBuffer> pcqueue(10);
  {
    GstreamerRunner::Options options;
    options.appsrc_do_timestamps = false;
    options.processing_pipeline_string = "queue";
    options.appsrc_caps_string = kH264CapsString;
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer gstreamer_buffer) -> absl::Status {
      pcqueue.TryEmplace(std::move(gstreamer_buffer));
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    EXPECT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString,
                                  /*pts*/ 0, /*dts*/ 0, /*duration*/
                                  100)
              .value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame2Path, kH264CapsString,
                                  /*pts*/ 100, /*dts*/ 100, /*duration*/
                                  100)
              .value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame3Path, kH264CapsString,
                                  /*pts*/ 200, /*dts*/ 200, /*duration*/
                                  100)
              .value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame4Path, kH264CapsString,
                                  /*pts*/ 300, /*dts*/ 300, /*duration*/
                                  100)
              .value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }
  }

  // Verify the results.
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    EXPECT_EQ(0, gstreamer_buffer.get_pts());
    EXPECT_EQ(0, gstreamer_buffer.get_dts());
    EXPECT_EQ(100, gstreamer_buffer.get_duration());
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    EXPECT_EQ(100, gstreamer_buffer.get_pts());
    EXPECT_EQ(100, gstreamer_buffer.get_dts());
    EXPECT_EQ(100, gstreamer_buffer.get_duration());
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    EXPECT_EQ(200, gstreamer_buffer.get_pts());
    EXPECT_EQ(200, gstreamer_buffer.get_dts());
    EXPECT_EQ(100, gstreamer_buffer.get_duration());
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
    EXPECT_EQ(300, gstreamer_buffer.get_pts());
    EXPECT_EQ(300, gstreamer_buffer.get_dts());
    EXPECT_EQ(100, gstreamer_buffer.get_duration());
  }
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_FALSE(pcqueue.TryPop(gstreamer_buffer, absl::Seconds(1)));
  }
}

TEST_F(GstreamerRunnerTest, NoFeedFetchPipelineTest) {
  {
    GstreamerRunner::Options options;
    options.processing_pipeline_string =
        "videotestsrc num-buffers=50 is-live=true ! "
        "video/x-raw,format=RGB ! fakesink";
    auto runner_statusor = GstreamerRunner::Create(options);
    ASSERT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();
    while (!runner->WaitUntilCompleted(absl::Seconds(1))) {
    }
    EXPECT_TRUE(runner->IsCompleted());
  }
}

TEST_F(GstreamerRunnerTest, FetchOnlyPipelineTest) {
  {
    ProducerConsumerQueue<RawImage> pcqueue(10);
    GstreamerRunner::Options options;
    options.processing_pipeline_string =
        "videotestsrc num-buffers=7 is-live=true ! "
        "video/x-raw,format=RGB,height=100,width=100";
    options.receiver_callback =
        [&pcqueue](GstreamerBuffer buffer) -> absl::Status {
      auto raw_image_status_or = ToRawImage(std::move(buffer));
      if (!raw_image_status_or.ok()) {
        LOG(ERROR) << raw_image_status_or.status();
      }
      pcqueue.Emplace(std::move(raw_image_status_or).value());
      return absl::OkStatus();
    };
    auto runner_statusor = GstreamerRunner::Create(options);
    ASSERT_TRUE(runner_statusor.ok());
    auto runner = std::move(runner_statusor).value();
    while (!runner->WaitUntilCompleted(absl::Seconds(1))) {
    }
    EXPECT_TRUE(runner->IsCompleted());
    EXPECT_EQ(pcqueue.count(), 7);
    RawImage raw_image;
    EXPECT_TRUE(pcqueue.TryPop(raw_image, absl::Seconds(1)));
    EXPECT_EQ(raw_image.height(), 100);
    EXPECT_EQ(raw_image.width(), 100);
    EXPECT_EQ(raw_image.channels(), 3);
  }
}

TEST_F(GstreamerRunnerTest, MotionVectorTest) {
  {
    ProducerConsumerQueue<MotionVectors> pcqueue(10);
    GstreamerRunner::Options options;
    options.processing_pipeline_string = "avdec_h264 debug-mv=true";
    options.appsrc_caps_string = kH264CapsString;

    options.receiver_callback =
        [&pcqueue](GstreamerBuffer buffer) -> absl::Status {
      auto mvs_status_or = ToMotionVectors(std::move(buffer));
      if (!mvs_status_or.ok()) {
        LOG(ERROR) << mvs_status_or.status();
      }
      pcqueue.Emplace(std::move(*mvs_status_or));
      return absl::OkStatus();
    };

    auto runner_statusor = GstreamerRunner::Create(options);
    ASSERT_TRUE(runner_statusor.ok()) << runner_statusor.status();
    auto runner = std::move(runner_statusor).value();

    // Feed the 1st H264 frame.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame1Path, kH264CapsString).value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    // Feed the 2nd H264 frame.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame2Path, kH264CapsString).value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    // Feed the 3rd H264 frame.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame3Path, kH264CapsString).value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    // Feed the 4th H264 frame.
    {
      GstreamerBuffer gstreamer_buffer =
          GstreamerBufferFromFile(kEncodedFrame4Path, kH264CapsString).value();
      EXPECT_TRUE(runner->Feed(gstreamer_buffer).ok());
    }

    runner->SignalEOS();
    runner->WaitUntilCompleted(absl::Seconds(1));
    EXPECT_TRUE(runner->IsCompleted());
    EXPECT_EQ(pcqueue.count(), 4);
    for (int i = 0; i < 4; ++i) {
      MotionVectors mvs;
      EXPECT_TRUE(pcqueue.TryPop(mvs, absl::Seconds(1)));
      if (i == 0) {
        // No motion vectors for the 1st frame since it's a key frame;
        EXPECT_EQ(mvs.size(), 0);
      } else {
        // Motion vectors should exist for the following frames.
        EXPECT_GT(mvs.size(), 0);
      }
    }
  }
}

}  // namespace visionai
