// Copyright 2020 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/types/gstreamer_buffer.h"

#include <cstring>
#include <memory>
#include <string>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

TEST(GstreamerBufferTest, DefaultBufferTest) {
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_TRUE(gstreamer_buffer.caps_string().empty());
    EXPECT_EQ(gstreamer_buffer.caps_cstr()[0], '\0');
    std::string buffer = std::move(gstreamer_buffer).ReleaseBuffer();
    EXPECT_TRUE(buffer.empty());
  }
}

TEST(GstreamerBufferTest, CapsTest) {
  {
    std::string caps_string = "video/x-raw";
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.set_caps_string(caps_string);
    EXPECT_EQ(gstreamer_buffer.caps_string(), caps_string);
    EXPECT_FALSE(strcmp(gstreamer_buffer.caps_cstr(), caps_string.c_str()));
    const char another_caps_string[] = "video/x-h264";
    gstreamer_buffer.set_caps_string(another_caps_string);
    EXPECT_FALSE(strcmp(gstreamer_buffer.caps_cstr(), another_caps_string));
  }
}

TEST(GstreamerBufferTest, IsKeyFrameTest) {
  {
    GstreamerBuffer gstreamer_buffer;
    EXPECT_FALSE(gstreamer_buffer.is_key_frame());
    gstreamer_buffer.set_is_key_frame(true);
    EXPECT_TRUE(gstreamer_buffer.is_key_frame());
  }
}

TEST(GstreamerBufferTest, MediaTypeTest) {
  {
    std::string caps_string =
        "image/jpeg, sof-marker=(int)0, width=(int)320, height=(int)240, "
        "pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1, "
        "interlace-mode=(string)progressive, colorimetry=(string)bt601, "
        "chroma-site=(string)jpeg, multiview-mode=(string)mono, "
        "multiview-flags=(GstVideoMultiviewFlagsSet)0:ffffffff:/"
        "right-view-first/left-flipped/left-flopped/right-flipped/"
        "right-flopped/half-aspect/mixed-mono";
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.set_caps_string(caps_string);
    EXPECT_EQ(gstreamer_buffer.media_type(), "image/jpeg");
  }
  {
    std::string caps_string = "video/x-raw";
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.set_caps_string(caps_string);
    EXPECT_EQ(gstreamer_buffer.media_type(), "video/x-raw");
  }
}

TEST(GstreamerBufferTest, AssignTest) {
  {
    std::string some_data("hello");
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.assign(some_data.data(), some_data.size());
    std::string buffer_value = std::move(gstreamer_buffer).ReleaseBuffer();
    EXPECT_EQ(buffer_value, some_data);
  }
  {
    std::string some_data("hello");
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.assign(some_data);
    std::string buffer_value = std::move(gstreamer_buffer).ReleaseBuffer();
    EXPECT_EQ(buffer_value, some_data);
  }
  {
    std::string some_data("hello");
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.assign(some_data);
    std::string buffer_value = std::move(gstreamer_buffer).ReleaseBuffer();
    EXPECT_EQ(buffer_value, some_data);
  }
  {
    std::string some_data("hello");
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.assign(some_data);
    std::string buffer_value(gstreamer_buffer.data(), gstreamer_buffer.size());
    EXPECT_EQ(buffer_value, some_data);
  }
}

}  // namespace visionai
