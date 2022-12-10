// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_TYPES_GSTREAMER_BUFFER_H_
#define THIRD_PARTY_VISIONAI_TYPES_GSTREAMER_BUFFER_H_

#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"

namespace visionai {

// A GstreamerBuffer contains the data in a GstBuffer and a string that
// describes its GstCaps (the "type").
class GstreamerBuffer {
 public:
  // Construct an empty GstreamerBuffer.
  GstreamerBuffer() = default;

  // Set the caps of the held data to caps_string.
  //
  // Usually, you would pass the result of the Gstreamer function
  // gst_caps_to_string into caps_string.
  void set_caps_string(absl::string_view caps_string) {
    caps_ = std::string(caps_string);
  }

  // Return the currently set caps string.
  const std::string& caps_string() const { return caps_; }

  // Return a pointer to the currently set caps as a cstring.
  //
  // The reference remains valid between calls to set_caps_string.
  const char* caps_cstr() const { return caps_.c_str(); }

  // Return the media type of this buffer.
  //
  // See
  // https://gstreamer.freedesktop.org/documentation/gstreamer/gststructure.html?gi-language=c#the-serialization-format
  // for more information.
  std::string media_type() const {
    size_t i = caps_.find_first_of(",");
    if (i == std::string::npos) {
      return caps_;
    }
    return caps_.substr(0, i);
  }

  // Return whether this buffer is a key frame.
  bool is_key_frame() const { return is_key_frame_; }

  // Set this buffer to be  a keyframe.
  void set_is_key_frame(bool b) { is_key_frame_ = b; }

  // Replaces the contents of the held data buffer by the bytes held between the
  // address range [src, src+size).
  //
  // Usually, you would use gst_buffer_map to obtain the starting address of the
  // GstBuffer and its size. You can then pass those into src and size.
  void assign(const char* src, size_t size) { bytes_.assign(src, src + size); }

  // Replaces the contents of the held data buffer by copying the argument.
  void assign(const std::string& s) { bytes_ = s; }

  // Replaces the contents of the held data buffer by moving the argument.
  void assign(std::string&& s) { bytes_ = std::move(s); }

  // Returns a pointer to the first value of the held data buffer.
  //
  // The reference remains valid between assigns.
  char* data() {
    return const_cast<char*>(static_cast<const GstreamerBuffer&>(*this).data());
  }

  const char* data() const { return bytes_.data(); }

  // Returns the size of held data buffer.
  size_t size() const { return bytes_.size(); }

  // Returns the released byte buffer for the caller to acquire.
  std::string&& ReleaseBuffer() && { return std::move(bytes_); }

  // Set the presentation timestamp of the frame.
  void set_pts(int64_t pts) { pts_ = pts; }

  // Get the presentation timestamp of the frame.
  int64_t get_pts() const { return pts_; }

  // Set the decoding timestamp of the frame.
  void set_dts(int64_t dts) { dts_ = dts; }

  // Get the decoding timestamp of the frame.
  int64_t get_dts() const { return dts_; }

  // Set the duration of the frame.
  void set_duration(int64_t duration) { duration_ = duration; }

  // Get the duration of the frame.
  int64_t get_duration() const { return duration_; }

  // Request default copy-control members.
  ~GstreamerBuffer() = default;
  GstreamerBuffer(const GstreamerBuffer&) = default;
  GstreamerBuffer(GstreamerBuffer&&) = default;
  GstreamerBuffer& operator=(const GstreamerBuffer&) = default;
  GstreamerBuffer& operator=(GstreamerBuffer&&) = default;

 private:
  std::string caps_;
  std::string bytes_;
  bool is_key_frame_ = false;
  int64_t pts_ = -1;
  int64_t dts_ = -1;
  int64_t duration_ = -1;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TYPES_GSTREAMER_BUFFER_H_
