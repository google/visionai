// Copyright 2020 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_TYPES_RAW_IMAGE_H_
#define THIRD_PARTY_VISIONAI_TYPES_RAW_IMAGE_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

// A class to represent raw images.
class RawImage {
 public:
  // Raw image format.
  //
  // Currently just standard RGB.
  enum class Format {
    kSRGB,
  };

  // Constructs a raw image of the specified height, width, and format.
  RawImage(int height, int width, Format format);

  // Constructs a raw image but initialized with the given data.
  RawImage(int height, int width, Format format, std::string&&);

  // Constructs a zero height, zero width, SRGB image.
  RawImage();

  // Disallow the copy constructor.
  RawImage(const RawImage&) = delete;
  RawImage& operator=(const RawImage&) = delete;
  RawImage(RawImage&&) = default;
  RawImage& operator=(RawImage&&) = default;

  // Returns the height of the image.
  int height() const { return height_; }

  // Returns the width of the image.
  int width() const { return width_; }

  // Returns the number of channels of the image.
  int channels() const { return channels_; }

  // Returns the image format.
  Format format() const { return format_; }

  // Replaces the data contents of the raw image with those contained in the
  // given std::string.
  absl::Status assign(const std::string&);
  absl::Status assign(std::string&&);

  // Returns a reference to the i'th value of the image buffer.
  //
  // You must ensure i is in the range [0, size()).
  const uint8_t& operator()(size_t i) const {
    return reinterpret_cast<const uint8_t&>(data_[i]);
  }

  uint8_t& operator()(size_t i) {
    return const_cast<uint8_t&>(static_cast<const RawImage&>(*this)(i));
  }

  // Returns a pointer to the first value of the image.
  //
  // The valid values are in the contiguous address range
  // [data(), data()+size()).
  uint8_t* data() {
    return const_cast<uint8_t*>(static_cast<const RawImage&>(*this).data());
  }

  const uint8_t* data() const {
    return reinterpret_cast<const uint8_t*>(data_.data());
  }

  // Returns the total size of the image.
  size_t size() const { return data_.size(); }

  // Returns the released image buffer for the caller to acquire.
  std::string&& ReleaseBuffer() && { return std::move(data_); }

 private:
  int height_;
  int width_;
  int channels_;
  Format format_;
  std::string data_;
};

// Return a string representation of the raw image format.
absl::StatusOr<std::string> ToString(RawImage::Format format);

// Return the raw image format from the format string.
absl::StatusOr<RawImage::Format> ToRawImageFormat(const std::string&);

// Return the expected buffer size of an image.
absl::StatusOr<int> GetRawImageBufferSize(int height, int width,
                                          RawImage::Format format);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TYPES_RAW_IMAGE_H_
