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

#include "visionai/streams/plugins/event_writers/encoded_stream_log_event_writer.h"

#include <memory>

#include "google/protobuf/text_format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "testing/base/public/mock-log.h"
#include "absl/debugging/leak_check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/test_util.h"
#include "visionai/algorithms/media/util/util.h"

namespace visionai {

using ::base_logging::INFO;
using ::testing::_;
using ::testing::kDoNotCaptureLogsYet;
using ::testing::ScopedMockLog;

constexpr absl::string_view kTestEventId = "some-event-id";

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class EncodedStreamLogEventWriterTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    ASSERT_TRUE(GstInit().ok());
    ASSERT_TRUE(GstRegisterPlugins().ok());
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  }

  void WritePackets(std::shared_ptr<EncodedStreamLogEventWriter> writer,
                    ScopedMockLog& log) {
    int count = 0;
    GstreamerRunner::Options options;
    options.processing_pipeline_string =
        "videotestsrc num-buffers=10 is-live=true ! "
        "videoconvert ! video/x-raw,format=RGB";
    options.receiver_callback =
        [&](GstreamerBuffer buffer) -> absl::Status {
      VAI_ASSIGN_OR_RETURN(Packet p, MakePacket(buffer));
      return writer->Write(p);
    };
    auto runner = GstreamerRunner::Create(options).value();
    EXPECT_CALL(log,
    Log(INFO, _, "Launching the gstreamer pipeline: rawvideoparse ! "
                  "videorate ! video/x-raw,framerate=25/1 ! x264enc"))
    .Times(1);
    EXPECT_CALL(log,
    Log(INFO, _, "Accepting the caps string: video/x-raw, width=(int)320, "
                 "height=(int)240, framerate=(fraction)30/1, "
                 "multiview-mode=(string)mono, format=(string)RGB, "
                 "pixel-aspect-ratio=(fraction)1/1, "
                 "interlace-mode=(string)progressive"))
    .Times(1);
    runner->WaitUntilCompleted(absl::Seconds(3));
    // Verify all packets have been loged
    EXPECT_CALL(log, Log(INFO, _, absl::StrFormat("(%s) ", kTestEventId)))
    .Times(count);
  }
};

TEST_F(EncodedStreamLogEventWriterTest, LogsTheReceivedPacket) {
  std::shared_ptr<EncodedStreamLogEventWriter> writer =
      std::make_shared<EncodedStreamLogEventWriter>();
  std::unique_ptr<EventWriterInitContext> init_ctx =
      std::make_unique<EventWriterInitContext>();
  ASSERT_TRUE(writer->Init(init_ctx.get()).ok());
  ASSERT_TRUE(writer->Open(kTestEventId).ok());
  ScopedMockLog log(kDoNotCaptureLogsYet);
  log.StartCapturingLogs();
  WritePackets(writer, log);
  log.StopCapturingLogs();
  ASSERT_TRUE(writer->Close().ok());
}

}  // namespace visionai
