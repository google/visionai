// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/gstreamer_video_writer.h"

#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/types/gstreamer_buffer.h"

namespace visionai {

namespace {
constexpr char kOutputFile[] = "/tmp/output.mp4";
constexpr char kExerciseVideoPath[] =
    "visionai/testing/testdata/media/exercise_1min.mp4";
constexpr char kExerciseVideoCapString[] =
    "video/x-h264, stream-format=(string)avc, alignment=(string)au, "
    "level=(string)3.1, profile=(string)high, "
    "codec_data=(buffer)"
    "0164001fffe1001a6764001facd9405005bb011000000300100000030320f18319600100"
    "0668ebe3cb22c0, width=(int)1280, height=(int)720, "
    "framerate=(fraction)25/1, pixel-aspect-ratio=(fraction)1/1, "
    "chroma-format=(string)4:2:0, bit-depth-luma=(uint)8, "
    "bit-depth-chroma=(uint)8, parsed=(boolean)true";
constexpr char kVideoTestSrcCapString[] =
    "video/x-raw, format=(string)ABGR64_LE, width=(int)320, height=(int)240, "
    "framerate=(fraction)30/1, multiview-mode=(string)mono, "
    "pixel-aspect-ratio=(fraction)1/1, interlace-mode=(string)progressive";
}

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class GstreamerVideoWriterTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    GstRegisterPlugins().IgnoreError();
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  }
};

void VerifyOutputVideo(const std::string& output_file, int expected_count,
                       const std::vector<int64_t>& expected_pts,
                       const std::vector<int64_t>& expected_dts,
                       const std::vector<int64_t>& expected_duration) {
  GstreamerRunner::Options verify_runner_opt;
  verify_runner_opt.processing_pipeline_string =
      absl::StrFormat("filesrc location=%s ! qtdemux ! h264parse", output_file);
  int count = 0;
  verify_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    if (buffer.get_pts() != expected_pts[count]) {
      return absl::InternalError("pts mismatch");
    }
    if (buffer.get_dts() != expected_dts[count]) {
      return absl::InternalError("dts mismatch");
    }
    if (buffer.get_duration() != expected_duration[count]) {
      return absl::InternalError("duration mismatch");
    }
    count++;
    return absl::OkStatus();
  };
  auto verify_runner = GstreamerRunner::Create(verify_runner_opt).value();
  ASSERT_TRUE(verify_runner->WaitUntilCompleted(absl::Seconds(3)));
  ASSERT_EQ(count, expected_count);
}

void GetOutputVideoPTS(const std::string& output_file,
                       std::vector<int64_t>* got_pts,
                       std::vector<int64_t>* got_dts,
                       std::vector<int64_t>* got_duration) {
  GstreamerRunner::Options verify_runner_opt;
  verify_runner_opt.processing_pipeline_string =
      absl::StrFormat("filesrc location=%s ! qtdemux ! h264parse", output_file);
  verify_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    got_pts->push_back(buffer.get_pts());
    got_dts->push_back(buffer.get_dts());
    got_duration->push_back(buffer.get_duration());
    return absl::OkStatus();
  };
  auto verify_runner = GstreamerRunner::Create(verify_runner_opt).value();
  ASSERT_TRUE(verify_runner->WaitUntilCompleted(absl::Seconds(3)));
}

TEST_F(GstreamerVideoWriterTest, H264InputMuxOnly) {
  std::string output_file = "/tmp/H264InputMuxOnly.mp4";
  GstreamerVideoWriter::Options options;
  options.file_path = output_file;
  options.caps_string = kExerciseVideoCapString;
  options.h264_mux_only = true;
  auto writer = GstreamerVideoWriter::Create(options).value();

  EXPECT_EQ(writer->GetPipelineStr(),
            absl::StrFormat("video/x-h264 ! mp4mux ! filesink location=%s",
                            output_file));

  std::vector<int64_t> expected_pts, expected_dts, expected_duration;
  GstreamerRunner::Options input_runner_opt;
  input_runner_opt.processing_pipeline_string = absl::StrFormat(
      "filesrc location=%s ! qtdemux ! h264parse", kExerciseVideoPath);
  input_runner_opt.appsink_sync = true;
  input_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    expected_pts.push_back(buffer.get_pts());
    expected_dts.push_back(buffer.get_dts());
    expected_duration.push_back(buffer.get_duration());
    return writer->Put(buffer);
  };
  auto input_runner = GstreamerRunner::Create(input_runner_opt).value();
  absl::SleepFor(absl::Seconds(2));
  input_runner->SignalEOS();
  absl::SleepFor(absl::Milliseconds(100));
  writer.reset();

  // PTS/DTS/Duration should match perfectly.
  VerifyOutputVideo(output_file, expected_pts.size(), expected_pts,
                    expected_dts, expected_duration);
  std::remove(output_file.c_str());
}

TEST_F(GstreamerVideoWriterTest, H264InputTranscodingMux) {
  std::string output_file = "/tmp/H264InputTranscodingMux.mp4";
  GstreamerVideoWriter::Options options;
  options.file_path = output_file;
  options.caps_string = kExerciseVideoCapString;
  options.h264_mux_only = false;
  auto writer = GstreamerVideoWriter::Create(options).value();

  EXPECT_EQ(writer->GetPipelineStr(),
            absl::StrFormat("decodebin ! videoconvert ! video/x-raw ! "
                            "videorate ! video/x-raw,framerate=25/1 ! x264enc "
                            "! mp4mux ! filesink location=%s",
                            output_file));

  std::vector<int64_t> expected_pts, expected_dts, expected_duration;
  GstreamerRunner::Options input_runner_opt;
  input_runner_opt.processing_pipeline_string = absl::StrFormat(
      "filesrc location=%s ! qtdemux ! h264parse", kExerciseVideoPath);
  input_runner_opt.appsink_sync = true;
  input_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    expected_pts.push_back(buffer.get_pts());
    expected_dts.push_back(buffer.get_dts());
    expected_duration.push_back(buffer.get_duration());
    return writer->Put(buffer);
  };
  auto input_runner = GstreamerRunner::Create(input_runner_opt).value();
  absl::SleepFor(absl::Seconds(2));
  input_runner->SignalEOS();
  absl::SleepFor(absl::Milliseconds(100));
  writer.reset();

  std::vector<int64_t> got_pts, got_dts, got_duration;
  int64_t expect_duration = 40000000;
  GetOutputVideoPTS(output_file, &got_pts, &got_dts, &got_duration);
  ASSERT_EQ(got_pts.size(), got_dts.size());
  ASSERT_EQ(got_pts.size(), got_duration.size());
  for (int i = 0; i < got_duration.size(); i++) {
    EXPECT_EQ(expect_duration, got_duration[i]);
  }
  sort(got_pts.begin(), got_pts.end(), std::less<int64_t>());
  int64_t last_pts = 0;
  for (int i = 0; i < got_pts.size(); i++) {
    if (last_pts > 0) {
      EXPECT_NEAR(expect_duration, got_pts[i] - last_pts, 1);
    }
    last_pts = got_pts[i];
  }
  std::remove(output_file.c_str());
}

TEST_F(GstreamerVideoWriterTest, NonH264InputRejected) {
  GstreamerVideoWriter::Options options;
  options.file_path = kOutputFile;
  options.caps_string = "video/x-raw, width=(int)1280, height=(int)720";
  options.h264_only = true;
  auto writer = GstreamerVideoWriter::Create(options);

  EXPECT_FALSE(writer.ok());
  EXPECT_TRUE(absl::IsFailedPrecondition(writer.status()));
}

TEST_F(GstreamerVideoWriterTest, NonH264InputAllowed) {
  std::string output_file = "/tmp/RawImageInput.mp4";
  GstreamerVideoWriter::Options options;
  options.file_path = output_file;
  options.caps_string = kVideoTestSrcCapString;
  options.h264_mux_only = false;
  auto writer = GstreamerVideoWriter::Create(options).value();

  EXPECT_EQ(writer->GetPipelineStr(),
            absl::StrFormat("decodebin ! videoconvert ! video/x-raw ! "
                            "videorate ! video/x-raw,framerate=30/1 ! x264enc "
                            "! mp4mux ! filesink location=%s",
                            output_file));
  std::vector<int64_t> empty;
  int count = 0;
  GstreamerRunner::Options input_runner_opt;
  input_runner_opt.processing_pipeline_string = "videotestsrc";
  input_runner_opt.appsink_sync = true;
  input_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    count++;
    return writer->Put(buffer);
  };
  auto input_runner = GstreamerRunner::Create(input_runner_opt).value();
  absl::SleepFor(absl::Seconds(2));
  input_runner->SignalEOS();
  absl::SleepFor(absl::Milliseconds(100));
  writer.reset();

  std::vector<int64_t> got_pts, got_dts, got_duration;
  int64_t expect_duration = 33333333;
  GetOutputVideoPTS(output_file, &got_pts, &got_dts, &got_duration);
  EXPECT_NEAR(got_pts.size(), count, 2);
  EXPECT_NEAR(got_dts.size(), count, 2);
  EXPECT_NEAR(got_duration.size(), count, 2);
  for (int i = 0; i < got_duration.size(); i++) {
    EXPECT_EQ(expect_duration, got_duration[i]);
  }
  sort(got_pts.begin(), got_pts.end(), std::less<int64_t>());
  int64_t last_pts = 0;
  for (int i = 0; i < got_pts.size(); i++) {
    if (last_pts > 0) {
      EXPECT_NEAR(expect_duration, got_pts[i] - last_pts, 1);
    }
    last_pts = got_pts[i];
  }
  std::remove(output_file.c_str());
}

}  // namespace visionai
