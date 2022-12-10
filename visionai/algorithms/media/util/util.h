// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_UTIL_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_UTIL_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

// Initialize GStreamer if it hasn't already been.
//
// This calls gst_init_check and returns the result through Status.
// You should call this rather than gst_init() directly.
absl::Status GstInit();

// Launch a gstreamer pipeline and block until it is done.
//
// `gst_pipeline`: This is a string that you would normally pass to gst-launch.
// `play_duration_in_seconds`: This is the maximum amount of time to run the
//                             pipeline for. Set it to -1 if you do not want a
//                             cap.
//                             The run will always end if the pipeline itself
//                             signals EOS, even if the bound hasn't been
//                             reached. (e.g. if the input source actually
//                             ends).
//
// The single argument overload applies no bound to the play duration (it just
// passes -1 to play_duration_in_seconds).
absl::Status GstLaunchPipeline(const std::string& gst_pipeline);
absl::Status GstLaunchPipeline(const std::string& gst_pipeline,
                               int play_duration_in_seconds);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_UTIL_H_
