/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_GSTREAMER_CONSTANTS_H_
#define THIRD_PARTY_VISIONAI_UTIL_GSTREAMER_CONSTANTS_H_

#include "absl/time/time.h"

namespace visionai {

inline constexpr absl::Duration kGstreamerRunnerFinalizationDeadline =
    absl::Seconds(5);

inline constexpr char kFramerateFieldName[] = "framerate";

inline constexpr int kDefaultFramerateNumerator = 25;

inline constexpr int kDefaultFramerateDenominator = 1;

// GstreammerBuffer video media type constants.
inline constexpr char kVideoH264[] = "video/x-h264";

inline constexpr char kVideoH265[] = "video/x-h265";

inline constexpr char kVideoRaw[] = "video/x-raw";

// GstreammerBuffer audio media type constants.
inline constexpr char kAudioMpeg[] = "audio/x-mpeg";

// GstreammerBuffer string media type constants.
inline constexpr char kString[] = "string";

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_GSTREAMER_CONSTANTS_H_
