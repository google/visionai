/*
 * Copyright 2022 Google LLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "visionai/streams/plugins/captures/rtsp_capture.h"

#include <memory>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "testing/base/public/mock-log.h"
#include "absl/strings/str_format.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/ring_buffer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {

using ::base_logging::WARNING;
using ::testing::_;
using ::testing::kDoNotCaptureLogsYet;
using ::testing::ScopedMockLog;

constexpr char kTestRtspUri[] = "rtsp://localhost:554/11";
constexpr char kH264ValidatorPipeline[] =
    "rtspsrc location=rtsp://localhost:554/11 "
    "protocols=GST_RTSP_LOWER_TRANS_TCP ! application/x-rtp,media=video";
constexpr char kRtspCapturePipeline[] =
    "rtspsrc location=rtsp://localhost:554/11 "
    "protocols=GST_RTSP_LOWER_TRANS_TCP ! rtph264depay ! h264parse";
constexpr char kH264Capstring[] = "video/x-h264";
constexpr char kH265Capstring[] = "video/x-h265";
constexpr char kRTSPH264Capstring[] =
    "application/x-rtp, media=(string)video, payload=(int)96, "
    "clock-rate=(int)90000, encoding-name=(string)H264";
constexpr char kRTSPH265Capstring[] =
    "application/x-rtp, media=(string)video, payload=(int)96, "
    "clock-rate=(int)90000, encoding-name=(string)H265";

// FakeGstreamerFetchRunner accepts a list of gstreamer buffers, acting as if
// the buffers were fetched from the underlying gstreamer pipeline.
class FakeGstreamerFetchRunner {
 public:
  FakeGstreamerFetchRunner(
      const std::string& pipeline,
      const std::vector<GstreamerBuffer>& buffers_from_pipeline)
      : expected_pipeline_(pipeline),
        buffers_from_pipeline_(buffers_from_pipeline) {}

  void Run(const GstreamerRunner::Options& got_options) {
    ASSERT_EQ(got_options.processing_pipeline_string, expected_pipeline_);
    for (const GstreamerBuffer& buffer : buffers_from_pipeline_) {
      got_options.receiver_callback(buffer).IgnoreError();
    }
  }

 private:
  std::string expected_pipeline_;
  std::vector<GstreamerBuffer> buffers_from_pipeline_;
};

// RTSPCaptureForTest extends the RTSPCapture but replace the actual gstreamer
// runner with a FakeGstreamerFetchRunner.
class RTSPCaptureForTest : public RTSPCapture {
 public:
  RTSPCaptureForTest(
    const FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner,
    const FakeGstreamerFetchRunner fake_gstreamer_runner)
      : fake_h264_validator_gstreamer_runner_(
          fake_h264_validator_gstreamer_runner),
        fake_gstreamer_runner_(fake_gstreamer_runner) {}
  ~RTSPCaptureForTest() override {}

 private:
  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_;
  FakeGstreamerFetchRunner fake_gstreamer_runner_;

  absl::Status ExecuteH264ValidatorGstreamerRunner(
      const GstreamerRunner::Options& options) override {
    fake_h264_validator_gstreamer_runner_.Run(options);
    return absl::OkStatus();
  }

  absl::Status ExecuteGstreamerRunner(
      const GstreamerRunner::Options& options) override {
    fake_gstreamer_runner_.Run(options);
    return absl::OkStatus();
  }
};

class RTSPCaptureTest : public ::testing::Test {
 protected:
  void SetUp() override {
    CaptureInitContext::InitData init_data;
    init_data.input_urls.push_back(kTestRtspUri);
    AttrValue val;
    val.set_i(10);
    init_data.attrs["timeout"] = val;
    capture_init_context_ = std::make_unique<CaptureInitContext>(init_data);

    output_buffer_ = std::make_shared<RingBuffer<Packet>>(10);
    CaptureRunContext::RunData run_data{output_buffer_};
    capture_run_context_ = std::make_unique<CaptureRunContext>(run_data);
  }

  std::unique_ptr<CaptureInitContext> capture_init_context_;
  std::unique_ptr<CaptureRunContext> capture_run_context_;
  std::shared_ptr<RingBuffer<Packet>> output_buffer_;

  struct BufferData {
    int64_t pts;
    int64_t dts;
    int64_t duration;
    bool is_key_frame;
  };

  std::vector<GstreamerBuffer> ConstructGstreamerBuffers(
      const std::string& caps_string,
      const std::vector<BufferData>& buffer_data_list) {
    std::vector<GstreamerBuffer> buffers;
    for (const auto& buffer_data : buffer_data_list) {
      GstreamerBuffer buffer;
      buffer.set_caps_string(caps_string);
      buffer.set_is_key_frame(buffer_data.is_key_frame);
      buffer.set_pts(buffer_data.pts);
      buffer.set_dts(buffer_data.dts);
      buffer.set_duration(buffer_data.duration);
      buffers.push_back(buffer);
    }
    return buffers;
  }

  void AssertOutputGstreamerBufferTimestamps(
      const std::vector<BufferData>& buffer_data_list) {
    Packet p;
    int iter = 0;
    while (output_buffer_->TryPopBack(p)) {
      auto packet_as_gst_buffer = PacketAs<GstreamerBuffer>(p);
      ASSERT_TRUE(packet_as_gst_buffer.ok());
      auto buffer = packet_as_gst_buffer.value();
      ASSERT_EQ(buffer.get_pts(), buffer_data_list[iter].pts);
      ASSERT_EQ(buffer.get_dts(), buffer_data_list[iter].dts);
      ASSERT_EQ(buffer.get_duration(), buffer_data_list[iter].duration);
      ASSERT_EQ(buffer.is_key_frame(), buffer_data_list[iter].is_key_frame);
      iter++;
    }
    ASSERT_EQ(iter, buffer_data_list.size());
  }
};

}  // namespace

TEST_F(RTSPCaptureTest, NonRTSPInput) {
  std::vector<BufferData> buffer_data_list;

  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_(
      kH264ValidatorPipeline,
      ConstructGstreamerBuffers(kRTSPH265Capstring, buffer_data_list));
  FakeGstreamerFetchRunner fake_gstreamer_runner(
      kRtspCapturePipeline,
      ConstructGstreamerBuffers(kH265Capstring, buffer_data_list));

  RTSPCaptureForTest capture(fake_h264_validator_gstreamer_runner_,
    fake_gstreamer_runner);

  ASSERT_TRUE(capture.Init(capture_init_context_.get()).ok());
  ASSERT_EQ(capture.Run(capture_run_context_.get()),
    absl::FailedPreconditionError(
        absl::StrFormat("Could not fetch the media and encoding type form"
        " the source \"%s\"", kTestRtspUri)));
}

TEST_F(RTSPCaptureTest, NonH264RTSPInput) {
  std::vector<BufferData> buffer_data_list = {
      BufferData{
          .pts = 800, .dts = 400, .duration = 400, .is_key_frame = false},
  };
  // H265 Caps
  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_(
      kH264ValidatorPipeline,
      ConstructGstreamerBuffers(kRTSPH265Capstring, buffer_data_list));
  FakeGstreamerFetchRunner fake_gstreamer_runner(
      kRtspCapturePipeline,
      ConstructGstreamerBuffers(kH265Capstring, buffer_data_list));

  RTSPCaptureForTest capture(fake_h264_validator_gstreamer_runner_,
    fake_gstreamer_runner);

  ASSERT_TRUE(capture.Init(capture_init_context_.get()).ok());
  ASSERT_EQ(capture.Run(capture_run_context_.get()),
    absl::FailedPreconditionError(
      "The input media - \"video\", encoding - \"H265\" "
      "is not supported. Currently the only supported media type is "
      "\"video/x-h264\""));
}

TEST_F(RTSPCaptureTest, DropInitialNonKeyFrames) {
  std::vector<BufferData> buffer_data_list = {
      BufferData{
          .pts = 800, .dts = 400, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 400, .dts = 800, .duration = 400, .is_key_frame = false},
  };
  std::vector<BufferData> buffer_data_after_first_key_frame_list = {
      BufferData{
          .pts = 1200, .dts = 1200, .duration = 400, .is_key_frame = true},
      BufferData{
          .pts = 2000, .dts = 1600, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 1600, .dts = 2000, .duration = 400, .is_key_frame = false},
  };

  for (const auto& buffer : buffer_data_after_first_key_frame_list) {
    buffer_data_list.push_back(buffer);
  }

  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_(
      kH264ValidatorPipeline,
      ConstructGstreamerBuffers(kRTSPH264Capstring, buffer_data_list));
  FakeGstreamerFetchRunner fake_gstreamer_runner(
      kRtspCapturePipeline,
      ConstructGstreamerBuffers(kH264Capstring, buffer_data_list));

  RTSPCaptureForTest capture(fake_h264_validator_gstreamer_runner_,
    fake_gstreamer_runner);
  ASSERT_TRUE(capture.Init(capture_init_context_.get()).ok());
  ASSERT_TRUE(capture.Run(capture_run_context_.get()).ok());
  ASSERT_EQ(output_buffer_->count(),
            buffer_data_after_first_key_frame_list.size());
  AssertOutputGstreamerBufferTimestamps(buffer_data_after_first_key_frame_list);
}

TEST_F(RTSPCaptureTest, FixedFrameRateInputs) {
  // Timestamps of the gstreamer buffers from the gstreamer pipeline.
  std::vector<BufferData> buffer_data_list = {
      BufferData{
          .pts = 10000, .dts = -1, .duration = 400, .is_key_frame = true},
      BufferData{
          .pts = 11200, .dts = -1, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 10400, .dts = -1, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 10800, .dts = -1, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 11600, .dts = -1, .duration = 400, .is_key_frame = false},
  };

  // Timestamps of the gstreamer buffers to output to downstream.
  // The dts will be set according to duration.
  std::vector<BufferData> output_buffer_data_list = {
      BufferData{
          .pts = 10000, .dts = 10000, .duration = 400, .is_key_frame = true},
      BufferData{
          .pts = 11200, .dts = 10400, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 10400, .dts = 10800, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 10800, .dts = 11200, .duration = 400, .is_key_frame = false},
      BufferData{
          .pts = 11600, .dts = 11600, .duration = 400, .is_key_frame = false},
  };

  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_(
      kH264ValidatorPipeline,
      ConstructGstreamerBuffers(kRTSPH264Capstring, buffer_data_list));
  FakeGstreamerFetchRunner fake_gstreamer_runner(
      kRtspCapturePipeline,
      ConstructGstreamerBuffers(kH264Capstring, buffer_data_list));

  RTSPCaptureForTest capture(fake_h264_validator_gstreamer_runner_,
    fake_gstreamer_runner);
  ASSERT_TRUE(capture.Init(capture_init_context_.get()).ok());
  ASSERT_TRUE(capture.Run(capture_run_context_.get()).ok());
  ASSERT_EQ(output_buffer_->count(), buffer_data_list.size());
  AssertOutputGstreamerBufferTimestamps(output_buffer_data_list);
}

TEST_F(RTSPCaptureTest, VariableFrameRateInputs) {
  // Timestamps of the gstreamer buffers from the gstreamer pipeline.
  std::vector<BufferData> buffer_data_list = {
      BufferData{.pts = 20000, .dts = -1, .duration = -1, .is_key_frame = true},
      BufferData{
          .pts = 20400, .dts = -1, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 20800, .dts = -1, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21200, .dts = -1, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21600, .dts = -1, .duration = -1, .is_key_frame = false},
  };

  // Timestamps of the gstreamer buffers to output to downstream.
  // The dts will be set according to pts.
  std::vector<BufferData> output_buffer_data_list = {
      BufferData{
          .pts = 20000, .dts = 20000, .duration = -1, .is_key_frame = true},
      BufferData{
          .pts = 20400, .dts = 20400, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 20800, .dts = 20800, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21200, .dts = 21200, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21600, .dts = 21600, .duration = -1, .is_key_frame = false},
  };

  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_(
      kH264ValidatorPipeline,
      ConstructGstreamerBuffers(kRTSPH264Capstring, buffer_data_list));
  FakeGstreamerFetchRunner fake_gstreamer_runner(
      kRtspCapturePipeline,
      ConstructGstreamerBuffers(kH264Capstring, buffer_data_list));

  RTSPCaptureForTest capture(fake_h264_validator_gstreamer_runner_,
    fake_gstreamer_runner);
  ASSERT_TRUE(capture.Init(capture_init_context_.get()).ok());
  ASSERT_TRUE(capture.Run(capture_run_context_.get()).ok());
  ASSERT_EQ(output_buffer_->count(), buffer_data_list.size());
  AssertOutputGstreamerBufferTimestamps(output_buffer_data_list);
}

TEST_F(RTSPCaptureTest, VFRWithBFramesExpectWarningLog) {
  // Timestamps of the gstreamer buffers from the gstreamer pipeline.
  std::vector<BufferData> buffer_data_list = {
      BufferData{.pts = 20000, .dts = -1, .duration = -1, .is_key_frame = true},
      BufferData{
          .pts = 20800, .dts = -1, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 20400, .dts = -1, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21200, .dts = -1, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21600, .dts = -1, .duration = -1, .is_key_frame = false},
  };

  // Timestamps of the gstreamer buffers to output to downstream.
  // The dts will be set according to pts.
  std::vector<BufferData> output_buffer_data_list = {
      BufferData{
          .pts = 20000, .dts = 20000, .duration = -1, .is_key_frame = true},
      BufferData{
          .pts = 20800, .dts = 20800, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 20400, .dts = 20400, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21200, .dts = 21200, .duration = -1, .is_key_frame = false},
      BufferData{
          .pts = 21600, .dts = 21600, .duration = -1, .is_key_frame = false},
  };

  FakeGstreamerFetchRunner fake_h264_validator_gstreamer_runner_(
      kH264ValidatorPipeline,
      ConstructGstreamerBuffers(kRTSPH264Capstring, buffer_data_list));
  FakeGstreamerFetchRunner fake_gstreamer_runner(
      kRtspCapturePipeline,
      ConstructGstreamerBuffers(kH264Capstring, buffer_data_list));

  ScopedMockLog log(kDoNotCaptureLogsYet);
  EXPECT_CALL(log,
              Log(WARNING, _,
                  "This is likely an input of variable frame rate with "
                  "B-frames. We will expand the support to this case soon."))
      .Times(1);
  log.StartCapturingLogs();

  RTSPCaptureForTest capture(fake_h264_validator_gstreamer_runner_,
    fake_gstreamer_runner);
  ASSERT_TRUE(capture.Init(capture_init_context_.get()).ok());
  ASSERT_TRUE(capture.Run(capture_run_context_.get()).ok());
  ASSERT_EQ(output_buffer_->count(), buffer_data_list.size());
  AssertOutputGstreamerBufferTimestamps(output_buffer_data_list);
}

}  // namespace visionai
