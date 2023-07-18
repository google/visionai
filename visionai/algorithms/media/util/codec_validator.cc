// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/codec_validator.h"

#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {
const absl::flat_hash_set<std::string> kSupportedMediaTypes = {"video/x-h264",
                                                               "video/x-h265"};
}

absl::Status IsVideoH264Input(const std::string& source_uri) {
  GstreamerRunner::Options pipeline_opts;
  std::string media_type;
  pipeline_opts.processing_pipeline_string =
      absl::StrFormat("filesrc location=%s ! parsebin", source_uri);
  pipeline_opts.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    media_type = buffer.media_type();
    // Once the pipeline received the first packet, pause/halt the pipeline.
    return absl::CancelledError();
  };

  VAI_ASSIGN_OR_RETURN(auto pipeline, GstreamerRunner::Create(pipeline_opts));
  while (!pipeline->IsCompleted()) {
  }
  if (media_type != "video/x-h264") {
    return absl::FailedPreconditionError(absl::StrFormat(
        "The input media type - \"%s\" is not supported. "
        "Currently the only supported media type is \"video/x-h264\"",
        media_type));
  }
  return absl::OkStatus();
}

absl::Status IsSupportedMediaType(const std::string& source_uri) {
  GstreamerRunner::Options pipeline_opts;
  std::string media_type;
  pipeline_opts.processing_pipeline_string =
      absl::StrFormat("filesrc location=%s ! parsebin", source_uri);
  pipeline_opts.receiver_callback =
      [&media_type](GstreamerBuffer buffer) -> absl::Status {
    media_type = buffer.media_type();
    // Once the pipeline received the first packet, pause/halt the pipeline.
    return absl::CancelledError();
  };

  VAI_ASSIGN_OR_RETURN(auto pipeline, GstreamerRunner::Create(pipeline_opts));
  while (!pipeline->IsCompleted()) {
  }
  if (!kSupportedMediaTypes.contains(media_type)) {
    return absl::FailedPreconditionError(
        absl::StrFormat("The input media type - \"%s\" is not supported. "
                        "Currently the supported media types are [%s]",
                        media_type, absl::StrJoin(kSupportedMediaTypes, ", ")));
  }
  return absl::OkStatus();
}

bool IsH265Supported() { return kSupportedMediaTypes.contains("video/x-h265"); }

}  // namespace visionai
