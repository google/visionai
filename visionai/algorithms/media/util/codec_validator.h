// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_CODEC_VALIDATOR_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_CODEC_VALIDATOR_H_

#include <string>

#include "absl/status/status.h"

namespace visionai {

// Validates whether the media type of input video is "video/x-h264".
// The source_uri is the file path of the input video.
absl::Status IsVideoH264Input(const std::string& source_uri);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_CODEC_VALIDATOR_H_
