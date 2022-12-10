// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_TEST_UTIL_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_TEST_UTIL_H_

#include "absl/status/statusor.h"
#include "visionai/types/gstreamer_buffer.h"

namespace visionai {

absl::StatusOr<GstreamerBuffer> GstreamerBufferFromFile(
    absl::string_view fname, absl::string_view caps_string);

absl::StatusOr<GstreamerBuffer> GstreamerBufferFromFile(
    absl::string_view fname, absl::string_view caps_string, int64_t pts,
    int64_t dts, int64_t duration);

// A function to check if the data is in correct JPEG format.
bool IsValidJpeg(const char* data, int len);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_TEST_UTIL_H_
