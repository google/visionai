// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

 #include "visionai/types/raw_image.h"

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"

namespace visionai {

namespace {

int GetNumImageChannels(RawImage::Format format) {
  switch (format) {
    case RawImage::Format::kSRGB:
      return 3;
    default:
      LOG(FATAL) << absl::StrFormat(
          "The given raw image format (%d) is unimplemented", format);
      return 0;
  }
}

}  // namespace

absl::StatusOr<std::string> ToString(RawImage::Format format) {
  switch (format) {
    case RawImage::Format::kSRGB:
      return "srgb";
    default:
      return absl::InvalidArgumentError(
          absl::StrFormat("Given an unknown raw image format %d", format));
  }
}

absl::StatusOr<RawImage::Format> ToRawImageFormat(const std::string &s) {
  if (s == "srgb") {
    return RawImage::Format::kSRGB;
  }
  return absl::InvalidArgumentError(
      absl::StrFormat("Given an unknown format string \"%s\"", s));
}

absl::StatusOr<int> GetRawImageBufferSize(int height, int width,
                                          RawImage::Format format) {
  if (height < 0 || width < 0) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given raw image descriptor has negative dimensions "
        "(height=%d, width=%d). They must be non-negative.",
        height, width));
  }
  int channels = GetNumImageChannels(format);

  int pixels = 0;
  auto overflow = __builtin_mul_overflow(height, width, &pixels);
  if (overflow) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Multiplication overflow when multiplying height (%d) and width (%d). "
        "Please contact us if you really need an image this large.",
        height, width));
  }

  int buf_size = 0;
  overflow = __builtin_mul_overflow(pixels, channels, &buf_size);
  if (overflow) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Multiplication overflow when multiply number of "
                        "pixels (%d) and the number of channels (%d), Please "
                        "contact us if you really need an image this large.",
                        pixels, channels));
  }

  return buf_size;
}

RawImage::RawImage() : RawImage(0, 0, Format::kSRGB) {}

RawImage::RawImage(int height, int width, Format format)
    : height_(height), width_(width), format_(format) {
  channels_ = GetNumImageChannels(format_);

  auto buf_size = GetRawImageBufferSize(height_, width_, format_);
  if (!buf_size.ok()) {
    LOG(FATAL) << buf_size.status();
  }

  data_.resize(*buf_size);
}

RawImage::RawImage(int height, int width, Format format, std::string &&s)
    : height_(height), width_(width), format_(format) {
  channels_ = GetNumImageChannels(format_);

  auto buf_size = GetRawImageBufferSize(height_, width_, format_);
  if (!buf_size.ok()) {
    LOG(FATAL) << buf_size.status();
  }

  if (static_cast<size_t>(*buf_size) != s.size()) {
    LOG(FATAL) << "Got " << s.size() << " of the data, but expected "
               << *buf_size;
  }

  data_ = std::move(s);
}

absl::Status RawImage::assign(const std::string &s) {
  if (s.size() != data_.size()) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given string has size %d but the image requires %d", s.size(),
        data_.size()));
  }
  data_.assign(s);
  return absl::OkStatus();
}

absl::Status RawImage::assign(std::string &&s) {
  if (s.size() != data_.size()) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given string has size %d but the image requires %d", s.size(),
        data_.size()));
  }
  data_.assign(std::move(s));
  return absl::OkStatus();
}

}  // namespace visionai
