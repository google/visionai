// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/test_util.h"

#include "visionai/util/file_helpers.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

absl::StatusOr<GstreamerBuffer> GstreamerBufferFromFile(
    absl::string_view fname, absl::string_view caps_string) {
  return GstreamerBufferFromFile(fname, caps_string, -1, -1, -1);
}

absl::StatusOr<GstreamerBuffer> GstreamerBufferFromFile(
    absl::string_view fname, absl::string_view caps_string, int64_t pts,
    int64_t dts, int64_t duration) {
  std::string file_contents;
  VAI_RETURN_IF_ERROR(GetFileContents(fname, &file_contents));
  GstreamerBuffer gstreamer_buffer;
  gstreamer_buffer.set_caps_string(caps_string);
  gstreamer_buffer.assign(std::move(file_contents));
  gstreamer_buffer.set_pts(pts);
  gstreamer_buffer.set_dts(dts);
  gstreamer_buffer.set_duration(duration);
  return gstreamer_buffer;
}

bool IsValidJpeg(const char* data, int len) {
  static const char kJpegMagic[] = {static_cast<char>(0xff),
                                    static_cast<char>(0xd8),
                                    static_cast<char>(0xff)};
  return (len > sizeof(kJpegMagic) &&
          !memcmp(data, kJpegMagic, sizeof(kJpegMagic)));
}

}  // namespace visionai
