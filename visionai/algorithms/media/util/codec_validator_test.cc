// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/codec_validator.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/file_path.h"

namespace visionai {

namespace {
using ::testing::HasSubstr;

constexpr char kTestFolder[] = "visionai/testing/testdata/media/";

class CodecValidatorTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    ASSERT_TRUE(GstRegisterPlugins().ok());
  }
};

}  // namespace

TEST_F(CodecValidatorTest, RunH265VideoInput) {
  if (!IsH265Supported()) {
    GTEST_SKIP();
  }
  std::string h265_video_path = file::JoinPath(kTestFolder, "h265.mp4");

  ASSERT_EQ(IsVideoH264Input(h265_video_path),
            absl::FailedPreconditionError(
                "The input media type - \"video/x-h265\" is not supported. "
                "Currently the only supported media type is \"video/x-h264\""));

  ASSERT_TRUE(IsSupportedMediaType(h265_video_path).ok());
}

TEST_F(CodecValidatorTest, RunH264VideoInput) {
  std::string h264_video_path =
      file::JoinPath(kTestFolder, "page-brin-4-frames.mp4");

  ASSERT_TRUE(IsVideoH264Input(h264_video_path).ok());

  ASSERT_TRUE(IsSupportedMediaType(h264_video_path).ok());
}

TEST_F(CodecValidatorTest, RunAV1VideoInput) {
  std::string av1_video_path = file::JoinPath(kTestFolder, "av1.mp4");

  ASSERT_EQ(IsVideoH264Input(av1_video_path),
            absl::FailedPreconditionError(
                "The input media type - \"video/x-av1\" is not supported. "
                "Currently the only supported media type is \"video/x-h264\""));

  EXPECT_THAT(
      IsSupportedMediaType(av1_video_path),
      StatusIs(
          absl::StatusCode::kFailedPrecondition,
          HasSubstr(
              "The input media type - \"video/x-av1\" is not supported.")));
}

}  // namespace visionai
