// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/types/raw_image.h"

#include <memory>
#include <string>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

TEST(RawImageTest, GetRawImageBufferSizeTest) {
  {
    int height = 1080;
    int width = 1920;
    RawImage::Format format = RawImage::Format::kSRGB;
    auto bufsize = GetRawImageBufferSize(height, width, format);
    EXPECT_TRUE(bufsize.ok());
    EXPECT_EQ(*bufsize, 6220800);
  }

  {
    int height = 1 << 16;
    int width = 1 << 16;
    RawImage::Format format = RawImage::Format::kSRGB;
    auto bufsize = GetRawImageBufferSize(height, width, format);
    EXPECT_FALSE(bufsize.ok());
    LOG(ERROR) << bufsize.status();
  }
}

TEST(RawImageTest, DefaultConstructorTest) {
  RawImage r;
  EXPECT_EQ(r.height(), 0);
  EXPECT_EQ(r.width(), 0);
  EXPECT_EQ(r.format(), RawImage::Format::kSRGB);
  EXPECT_EQ(r.channels(), 3);
  EXPECT_EQ(r.size(), 0);
  EXPECT_NE(r.data(), nullptr);
}

TEST(RawImageTest, HeightWidthFormatConstructorTest) {
  {
    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);
    EXPECT_EQ(r.height(), height);
    EXPECT_EQ(r.width(), width);
    EXPECT_EQ(r.format(), format);
    EXPECT_EQ(r.channels(), 3);
    EXPECT_EQ(r.size(), 105);
  }

  {
    int height = -1;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;
    ASSERT_DEATH({ RawImage r(height, width, format); }, "");
  }

  {
    int height = 1080;
    int width = 1920;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);
    EXPECT_EQ(r.size(), 6220800);
  }

  {
    int height = 1 << 16;
    int width = 1 << 16;
    RawImage::Format format = RawImage::Format::kSRGB;
    ASSERT_DEATH({ RawImage r(height, width, format); }, "");
  }

  {
    int height = 1 << 16;
    int width = 1 << 15;
    RawImage::Format format = RawImage::Format::kSRGB;
    ASSERT_DEATH({ RawImage r(height, width, format); }, "");
  }
}

TEST(RawImageTest, HeightWidthFormatStringMoveConstructorTest) {
  {
    std::string src(105, 'a');

    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;

    std::string tmp(src);
    RawImage r(height, width, format, std::move(tmp));

    EXPECT_EQ(r.height(), height);
    EXPECT_EQ(r.width(), width);
    EXPECT_EQ(r.format(), format);
    EXPECT_EQ(r.channels(), 3);
    EXPECT_EQ(r.size(), 105);
  }

  {
    std::string src(104, 'a');

    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;

    std::string tmp(src);
    ASSERT_DEATH({ RawImage r(height, width, format, std::move(tmp)); }, "");
  }
}

TEST(RawImageTest, ToStringTest) {
  {
    RawImage::Format format = RawImage::Format::kSRGB;
    auto format_string = ToString(format);
    EXPECT_TRUE(format_string.ok());
    EXPECT_EQ(*format_string, "srgb");
  }
}

TEST(RawImageTest, ToRawImageFormatTest) {
  {
    auto format = ToRawImageFormat("srgb");
    EXPECT_TRUE(format.ok());
    EXPECT_EQ(*format, RawImage::Format::kSRGB);
  }
  {
    auto format = ToRawImageFormat("");
    EXPECT_FALSE(format.ok());
  }
  {
    auto format = ToRawImageFormat("bogus");
    EXPECT_FALSE(format.ok());
  }
}

TEST(RawImageTest, DataAccessTest) {
  {
    int height = 1;
    int width = 1;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);
    r(0) = 0;
    r(1) = 1;
    r(2) = 2;
    for (size_t i = 0; i < r.size(); ++i) {
      EXPECT_EQ(r(i), i);
      EXPECT_EQ(r.data()[i], i);
    }
  }
}

TEST(RawImageTest, AssignTest) {
  {
    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);

    std::string src(105, 'a');
    auto status = r.assign(src);
    EXPECT_TRUE(status.ok());

    for (size_t i = 0; i < r.size(); ++i) {
      EXPECT_EQ(src[i], r(i));
    }
  }

  {
    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);

    std::string src(101, 'a');
    auto status = r.assign(src);
    EXPECT_FALSE(status.ok());
  }

  {
    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);

    std::string src(105, 'a');
    std::string tmp(105, 'a');
    auto status = r.assign(std::move(tmp));
    EXPECT_TRUE(status.ok());

    for (size_t i = 0; i < r.size(); ++i) {
      EXPECT_EQ(src[i], r(i));
    }
  }

  {
    int height = 5;
    int width = 7;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);

    std::string src(101, 'a');
    auto status = r.assign(std::move(src));
    EXPECT_FALSE(status.ok());
  }
}

TEST(RawImageTest, ReleaseBufferTest) {
  {
    int height = 2;
    int width = 2;
    RawImage::Format format = RawImage::Format::kSRGB;
    RawImage r(height, width, format);

    std::string src(12, 1);
    auto status = r.assign(src);
    EXPECT_TRUE(status.ok());

    std::string dst = std::move(r).ReleaseBuffer();
    EXPECT_EQ(dst.size(), 12);
    for (size_t i = 0; i < dst.size(); ++i) {
      EXPECT_EQ(dst[i], 1);
    }
  }
}

}  // namespace visionai
