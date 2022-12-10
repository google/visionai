// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_GSTREAMER_REGISTRY_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_GSTREAMER_REGISTRY_H_
#include "absl/status/status.h"

namespace visionai {

// Statically register the Gstreamer plugin libraries (currently includes the
// extensive list of all possibly used plugins).
absl::Status GstRegisterPlugins();
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_GSTREAMER_REGISTRY_H_
