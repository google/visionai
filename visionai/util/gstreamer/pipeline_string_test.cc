// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/gstreamer/pipeline_string.h"

#include "gtest/gtest.h"

namespace visionai {
namespace {

constexpr char kMp4VideoFilePath[] = "/test.mp4";
constexpr char kMkvVideoFilePath[] = "/test.mkv";
constexpr char kVideoRawCapString[] =
    "video/x-raw, format=(string)ABGR64_LE, width=(int)320, height=(int)240, "
    "framerate=(fraction)30/1, multiview-mode=(string)mono, "
    "pixel-aspect-ratio=(fraction)1/1, interlace-mode=(string)progressive";
constexpr char kVideoRawCapStringWithZeroFrameRate[] =
    "video/x-raw, format=(string)ABGR64_LE, width=(int)320, height=(int)240, "
    "framerate=(fraction)0/1, multiview-mode=(string)mono, "
    "pixel-aspect-ratio=(fraction)1/1, interlace-mode=(string)progressive";
}  // namespace

TEST(pipeline_string, GetMp4FileSrcGstPipeline) {
  std::string gst_pipeline = FileSrcGstPipelineStr(kMp4VideoFilePath);
  EXPECT_EQ(gst_pipeline, "filesrc location=/test.mp4 ! qtdemux ! h264parse");
}

TEST(pipeline_string, GetMkvFileSrcGstPipeline) {
  std::string gst_pipeline = FileSrcGstPipelineStr(kMkvVideoFilePath);
  EXPECT_EQ(gst_pipeline, "filesrc location=/test.mkv ! parsebin");
}

TEST(pipeline_string, GetMp4FileSinkH264GstPipelineStr) {
  auto gst_pipeline =
      Mp4FileSinkH264GstPipelineStr(kMp4VideoFilePath);
  EXPECT_EQ(gst_pipeline.value(),
      "video/x-h264 ! mp4mux ! filesink location=/test.mp4");
}

TEST(pipeline_string, GetMp4FileSinkTranscodeGstPipelineStr) {
  auto gst_pipeline = Mp4FileSinkTranscodeGstPipelineStr(kVideoRawCapString,
                                                        kMp4VideoFilePath);
  EXPECT_EQ(gst_pipeline.value(),
      "decodebin ! videoconvert ! video/x-raw ! videorate ! "
      "video/x-raw,framerate=30/1 ! x264enc ! mp4mux ! "
      "filesink location=/test.mp4");
}

TEST(pipeline_string, GetMp4FileSinkTranscodeGstPipelineStrWithZeroFrameRate) {
  auto gst_pipeline = Mp4FileSinkTranscodeGstPipelineStr(
      kVideoRawCapStringWithZeroFrameRate,
      kMp4VideoFilePath);
  EXPECT_EQ(gst_pipeline.value(),
      "decodebin ! videoconvert ! video/x-raw ! videorate ! "
      "video/x-raw,framerate=25/1 ! x264enc ! mp4mux ! "
      "filesink location=/test.mp4");
}

}  // namespace visionai
