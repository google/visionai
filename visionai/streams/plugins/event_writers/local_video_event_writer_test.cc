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

#include "visionai/streams/plugins/event_writers/local_video_event_writer.h"

#include "google/protobuf/text_format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "testing/base/public/mock-log.h"
#include "absl/debugging/leak_check.h"
#include "absl/strings/substitute.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/file_helpers.h"

namespace visionai {

using ::base_logging::INFO;
using ::google::protobuf::TextFormat;
using ::testing::_;
using ::testing::kDoNotCaptureLogsYet;
using ::testing::ScopedMockLog;

constexpr char kOutputDir[] = "/tmp";
constexpr char kTestOutputFile[] = "/tmp/some-event-id.mp4";
constexpr char kTestEventId[] = "some-event-id";
constexpr char kTestExerciseVideoPath[] =
    "visionai/testing/testdata/media/exercise_1min.mp4";
constexpr char kTestConfigTemplate[] = R"pb(
  name: "LocalVideoEventWriter"
  attr { key: "skip_until_first_key_frame" value: "true" }
)pb";

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class LocalEventWriterTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    ASSERT_TRUE(GstInit().ok());
    ASSERT_TRUE(GstRegisterPlugins().ok());
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  }

  void VerifyOutputVideo(const std::string& output_file, int expected_count) {
    GstreamerRunner::Options verify_runner_opt;
    verify_runner_opt.processing_pipeline_string =
        absl::StrFormat("filesrc location=%s ! qtdemux ! h264parse",
                        output_file);
    int count = 0;
    LOG(ERROR) << verify_runner_opt.processing_pipeline_string;
    verify_runner_opt.receiver_callback =
        [&](GstreamerBuffer buffer) -> absl::Status {
      count++;
      return absl::OkStatus();
    };
    auto verify_runner = GstreamerRunner::Create(verify_runner_opt).value();
    ASSERT_TRUE(verify_runner->WaitUntilCompleted(absl::Seconds(3)));
    ASSERT_EQ(count, expected_count);
  }
};

TEST_F(LocalEventWriterTest, WriteVideoInLocal) {
  std::unique_ptr<LocalVideoEventWriter> writer =
      std::make_unique<LocalVideoEventWriter>();
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ScopedMockLog log(kDoNotCaptureLogsYet);
  EXPECT_CALL(log,
      Log(INFO, _, absl::StrCat("output attribute not set. Writing to default "
                  "temp location: ", kOutputDir)))
      .Times(1);
  log.StartCapturingLogs();
  ASSERT_TRUE(writer->Init(context.get()).ok());
  log.StopCapturingLogs();
  ASSERT_TRUE(writer->Open(kTestEventId).ok());

  int packets_count = 0;
  GstreamerRunner::Options input_runner_opt;
  input_runner_opt.processing_pipeline_string = absl::StrFormat(
      "filesrc location=%s ! qtdemux ! h264parse", kTestExerciseVideoPath);
  input_runner_opt.appsink_sync = true;
  input_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    buffer.set_is_key_frame(true);
    packets_count++;
    return writer->Write(MakePacket(std::move(buffer)).value());
  };
  auto input_runner = GstreamerRunner::Create(input_runner_opt).value();
  absl::SleepFor(absl::Seconds(2));
  input_runner->SignalEOS();
  absl::SleepFor(absl::Milliseconds(100));

  ASSERT_TRUE(writer->Close().ok());
  ASSERT_TRUE(FileExists(kTestOutputFile).ok());
  VerifyOutputVideo(kTestOutputFile, packets_count);
  ASSERT_TRUE(DeleteFile(kTestOutputFile).ok());
}

TEST_F(LocalEventWriterTest, WriteVideoFileFailedWithNoKeyFrame) {
  std::unique_ptr<LocalVideoEventWriter> writer =
      std::make_unique<LocalVideoEventWriter>();
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());
  ASSERT_TRUE(writer->Open(kTestEventId).ok());

  GstreamerRunner::Options input_runner_opt;
  input_runner_opt.processing_pipeline_string = absl::StrFormat(
      "filesrc location=%s ! qtdemux ! h264parse", kTestExerciseVideoPath);
  input_runner_opt.appsink_sync = true;
  input_runner_opt.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    buffer.set_is_key_frame(false);
    return writer->Write(MakePacket(std::move(buffer)).value());
  };
  auto input_runner = GstreamerRunner::Create(input_runner_opt).value();
  absl::SleepFor(absl::Seconds(2));
  input_runner->SignalEOS();
  absl::SleepFor(absl::Milliseconds(100));

  ASSERT_TRUE(writer->Close().ok());
  ASSERT_THAT(FileExists(kTestOutputFile),
              absl::NotFoundError("The path does not exist."));
}

}  // namespace visionai
