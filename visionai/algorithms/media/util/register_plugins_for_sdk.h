// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_REGISTER_PLUGINS_FOR_SDK_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_REGISTER_PLUGINS_FOR_SDK_H_
#include "absl/status/status.h"

namespace visionai {

// Statically register all gstreamer plugins for sdk (excluding x264).
absl::Status RegisterGstPluginsForSDK();

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_REGISTER_PLUGINS_FOR_SDK_H_
