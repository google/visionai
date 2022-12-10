// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_TYPE_UTIL_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_TYPE_UTIL_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/motion_vector.h"
#include "visionai/types/raw_image.h"

namespace visionai {

// Converts a GstreamerBuffer into a RawImage.
// You should pass an rvalue for `gstreamer_buffer` if possible.
//
// Of course, this may not always be possible based just on the fact
// that a GstreamerBuffer can contain any caps whatsoever.
// In any case, the returned status will indicate why a conversion failed.
absl::StatusOr<RawImage> ToRawImage(GstreamerBuffer gstreamer_buffer);

// Converts a Gstreamer Buffer into a MotionVector.
//
// You should pass an rvalue for `gstreamer_buffer` if possible.
absl::StatusOr<MotionVectors> ToMotionVectors(GstreamerBuffer gstreamer_buffer);

// Convert the given RawImage into a GstreamerBuffer.
//
// You should pass an rvalue for `raw_image` if possible.
absl::StatusOr<GstreamerBuffer> ToGstreamerBuffer(RawImage raw_image);

// Construct a visionai::GstreamerBuffer from a pair of GstCaps and GstBuffer.
//
// It will be created by copying. The contents of the GstCaps and GstBuffer
// will not be changed.
absl::StatusOr<visionai::GstreamerBuffer> ToGstreamerBuffer(GstCaps *caps,
                                                            GstBuffer *buffer);
// Return the media type of the given caps string.
//
// See
// https://gstreamer.freedesktop.org/documentation/gstreamer/gststructure.html?gi-language=c#the-serialization-format
// for more information.
std::string MediaTypeFromCaps(const std::string &caps);

// Validate whether the video RTSP is encoded in H264.
absl::Status IsRTPCapsVideoH264(const std::string& caps,
                                const std::string& source_uri);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_TYPE_UTIL_H_
