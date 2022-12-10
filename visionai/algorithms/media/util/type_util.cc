// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/type_util.h"

#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
extern "C"{
#include "third_party/ffmpeg/libavutil/motion_vector.h"
}
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/motion_vector.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/status/status_macros.h"

#define ROUND_UP_4(num) (((num) + 3) & ~0x3)

namespace visionai {

namespace {

constexpr char kRawImageGstreamerMimeType[] = "video/x-raw";

absl::StatusOr<RawImage::Format> GstVideoFormatToRawimageFormat(
    GstVideoFormat format) {
  switch (format) {
    case GST_VIDEO_FORMAT_RGB:
      return RawImage::Format::kSRGB;
    default:
      return absl::UnimplementedError(absl::StrFormat(
          "The given GstVideoFormat (id=%d) is unimplemented", format));
  }
}

// Structure that contains all metadata deducible from a GstreamerBuffer whose
// caps is of type video/x-raw.
//
// This has all the information necessary to identify the exact raw image type
// the buffer contains.
struct GstreamerRawImageInfo {
  GstVideoFormat gst_format_id;
  std::string format_name;
  int height = -1;
  int width = -1;
  int size = -1;
  int planes = -1;
  int components = -1;
  int rstride = -1;
  int pstride = -1;
};

// Parse the given gstremaer caps string (in Gstreamer's format) for the raw
// image metadata.
absl::Status ParseAsRawImageCaps(const std::string& caps_string,
                                 GstreamerRawImageInfo* info) {
  // Parse the caps.
  GstCaps* caps = gst_caps_from_string(caps_string.c_str());
  if (caps == nullptr) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Failed to create a GstCaps from \"%s\"; make sure it is a valid "
        "cap string",
        caps_string));
  }

  // Verify this is indeed a raw image caps.
  GstStructure* structure = gst_caps_get_structure(caps, 0);
  std::string media_type(gst_structure_get_name(structure));
  if (media_type != kRawImageGstreamerMimeType) {
    gst_caps_unref(caps);
    return absl::InvalidArgumentError(absl::StrFormat(
        "Given a GstCaps of \"%s\" which is not a raw image caps string",
        caps_string));
  }

  // Extract specific attributes and type of this raw image.
  GstVideoInfo gst_info;
  if (!gst_video_info_from_caps(&gst_info, caps)) {
    gst_caps_unref(caps);
    return absl::InvalidArgumentError(absl::StrFormat(
        "Unable to get format information from caps %s", caps_string));
  }

  gst_caps_unref(caps);
  info->gst_format_id = GST_VIDEO_INFO_FORMAT(&gst_info);
  info->format_name = GST_VIDEO_INFO_NAME(&gst_info);
  info->planes = GST_VIDEO_INFO_N_PLANES(&gst_info);
  info->components = GST_VIDEO_INFO_N_COMPONENTS(&gst_info);
  info->height = GST_VIDEO_INFO_HEIGHT(&gst_info);
  info->width = GST_VIDEO_INFO_WIDTH(&gst_info);
  info->size = GST_VIDEO_INFO_SIZE(&gst_info);

  // TODO: Support just single planed images to start.
  // This is fine for most machine learning use cases.
  if (info->planes == 1) {
    info->rstride = GST_VIDEO_INFO_PLANE_STRIDE(&gst_info, 0);
    info->pstride = GST_VIDEO_INFO_COMP_PSTRIDE(&gst_info, 0);
  } else {
    if (info->planes > 1) {
      return absl::UnimplementedError(
          "We currently support only single planed images");
    } else {
      return absl::InvalidArgumentError("The given image has no planes");
    }
  }

  return absl::OkStatus();
}

// The caller is responsible for ensuring that the GstreamerRawImageInfo is a
// RGB image. The GstreamerRawImageInfo must also be parsed from the given
// GstreamerBuffer.
absl::StatusOr<RawImage> ToRgbRawImage(const GstreamerRawImageInfo& info,
                                       GstreamerBuffer gstreamer_buffer) {
  auto format = GstVideoFormatToRawimageFormat(GST_VIDEO_FORMAT_RGB);
  if (!format.ok()) {
    return absl::InternalError(
        "Failed to convert RAW_IMAGE_FORMAT_SRGB into a RawImage::Format.");
  }

  // Fast path when there is no extra padding.
  if (info.pstride == info.components &&
      info.rstride == info.width * info.pstride &&
      static_cast<int>(gstreamer_buffer.size()) == info.height * info.rstride) {
    RawImage r(info.height, info.width, *format,
               std::move(gstreamer_buffer).ReleaseBuffer());
    return std::move(r);
  }

  // Slow path to close extra padding.
  RawImage r(info.height, info.width, *format);
  for (int i = 0; i < info.height; ++i) {
    int src_row_start = info.rstride * i;
    int dst_row_start = info.width * info.components * i;
    for (int j = 0; j < info.width; ++j) {
      int src_pix_start = src_row_start + info.pstride * j;
      int dst_pix_start = dst_row_start + info.components * j;
      for (int k = 0; k < info.components; ++k) {
        r(dst_pix_start + k) = gstreamer_buffer.data()[src_pix_start + k];
      }
    }
  }
  return std::move(r);
}

absl::StatusOr<GstreamerBuffer> RgbRawImageToGstreamerBuffer(RawImage r) {
  // Set the caps string.
  GstreamerBuffer gstreamer_buffer;
  GstCaps* caps = gst_caps_new_simple(
      kRawImageGstreamerMimeType, "format", G_TYPE_STRING, "RGB", "width",
      G_TYPE_INT, r.width(), "height", G_TYPE_INT, r.height(), "framerate",
      GST_TYPE_FRACTION, 0, 1, NULL);
  gchar* caps_string = gst_caps_to_string(caps);
  gstreamer_buffer.set_caps_string(caps_string);
  g_free(caps_string);
  gst_caps_unref(caps);

  // Set key frame.
  gstreamer_buffer.set_is_key_frame(true);

  // Fast path for when padding is not needed.
  // See
  //
  // https://gstreamer.freedesktop.org/documentation/additional/design/mediatype-video-raw.html?gi-language=c
  // for more details. In this case, RGB images must pad each row up to the
  // nearest size divisible by 4.
  if (!(r.width() * r.channels() % 4)) {
    gstreamer_buffer.assign(std::move(r).ReleaseBuffer());
    return gstreamer_buffer;
  }

  // Slow path for when padding is needed.
  int row_size = r.width() * r.channels();
  int row_stride = ROUND_UP_4(row_size);
  std::string bytes;
  bytes.resize(row_stride * r.height());
  for (int i = 0; i < r.height(); ++i) {
    std::copy(r.data() + row_size * i, r.data() + row_size * (i + 1),
              const_cast<char*>(bytes.data() + row_stride * i));
  }
  gstreamer_buffer.assign(std::move(bytes));
  return gstreamer_buffer;
}

}  // namespace

absl::StatusOr<RawImage> ToRawImage(GstreamerBuffer gstreamer_buffer) {
  VAI_RETURN_IF_ERROR(GstInit());

  GstreamerRawImageInfo info;
  auto status = ParseAsRawImageCaps(gstreamer_buffer.caps_string(), &info);
  if (!status.ok()) {
    LOG(ERROR) << status;
    return absl::InvalidArgumentError(
        "Failed to parse the given buffer as a raw image");
  }

  switch (info.gst_format_id) {
    case GST_VIDEO_FORMAT_RGB:
      return ToRgbRawImage(info, std::move(gstreamer_buffer));
    default:
      return absl::UnimplementedError(absl::StrFormat(
          "We currently do not support \"%s\"", info.format_name));
  }
}

absl::StatusOr<GstreamerBuffer> ToGstreamerBuffer(RawImage raw_image) {
  VAI_RETURN_IF_ERROR(GstInit());

  if (raw_image.format() != RawImage::Format::kSRGB) {
    auto format_string = ToString(raw_image.format());
    if (!format_string.ok()) {
      return format_string.status();
    }
    return absl::UnimplementedError(absl::StrFormat(
        "We currently do not support raw images with your given format (%s)",
        *format_string));
  }
  return RgbRawImageToGstreamerBuffer(std::move(raw_image));
}

absl::StatusOr<visionai::GstreamerBuffer> ToGstreamerBuffer(GstCaps* caps,
                                                            GstBuffer* buffer) {
  VAI_RETURN_IF_ERROR(GstInit());

  if (caps == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a GstCaps.");
  }
  if (buffer == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a GstBuffer.");
  }

  visionai::GstreamerBuffer gstreamer_buffer;

  // Set the caps string.
  gchar* caps_string = gst_caps_to_string(caps);
  if (caps_string == nullptr) {
    return absl::UnknownError(
        "Failed to convert the given GstCaps to a C string.");
  }
  gstreamer_buffer.set_caps_string(caps_string);
  g_free(caps_string);

  // Set the bytes buffer.
  GstMapInfo map;
  if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
    return absl::UnknownError("Failed to map the given GstBuffer.");
  }
  gstreamer_buffer.assign((char*)map.data, map.size);
  gst_buffer_unmap(buffer, &map);

  // Set/cache packet flags.
  //
  // TODO: Find a way to decide whether a buffer is the frame head.
  //       One possibility is to use GST_BUFFER_FLAG_MARKER, but this bit is
  //       not used uniformly across plugins.
  //       Turn it on uniformly for the time being as it is by far the common
  //       case.
  bool is_key_frame =
      !GST_BUFFER_FLAG_IS_SET(buffer, GST_BUFFER_FLAG_DELTA_UNIT);
  gstreamer_buffer.set_is_key_frame(is_key_frame);

  return gstreamer_buffer;
}

absl::StatusOr<MotionVectors> ToMotionVectors(
    GstreamerBuffer gstreamer_buffer) {
  if (gstreamer_buffer.size() % sizeof(AVMotionVector) != 0) {
    return absl::InvalidArgumentError(
        absl::StrFormat("`gstreamer_buffer.size()` should be divisible by %d, "
                        "but got %d instead.",
                        sizeof(AVMotionVector), gstreamer_buffer.size()));
  }
  AVMotionVector* av_mvs =
      reinterpret_cast<AVMotionVector*>(gstreamer_buffer.data());

  MotionVectors mvs;
  int mvs_size = gstreamer_buffer.size() / sizeof(*av_mvs);
  mvs.reserve(mvs_size);
  for (int i = 0; i < mvs_size; i++) {
    const AVMotionVector* av_mv = &av_mvs[i];
    mvs.push_back({.source = av_mv->source,
                   .w = av_mv->w,
                   .h = av_mv->h,
                   .src_x = av_mv->src_x,
                   .src_y = av_mv->src_y,
                   .dst_x = av_mv->dst_x,
                   .dst_y = av_mv->dst_y,
                   .motion_x = av_mv->motion_x,
                   .motion_y = av_mv->motion_y,
                   .motion_scale = av_mv->motion_scale});
  }
  return mvs;
}

std::string MediaTypeFromCaps(const std::string& caps) {
  size_t i = caps.find_first_of(',');
  if (i == std::string::npos) {
    return caps;
  }
  return caps.substr(0, i);
}

absl::Status IsRTPCapsVideoH264(const std::string& caps,
                        const std::string& source_uri) {
  GstCaps* gst_caps = gst_caps_from_string(caps.c_str());

  if (gst_caps == nullptr) {
    return absl::FailedPreconditionError(
      absl::StrFormat("Could not connect to \"%s\"", source_uri));
  }

  GstStructure* gst_structure = gst_caps_get_structure(gst_caps, 0);
  const gchar* media_type;
  const gchar* encoding_name;

  if (!(media_type = gst_structure_get_string(gst_structure, "media"))||
        !(encoding_name =
            gst_structure_get_string(gst_structure, "encoding-name"))) {
      gst_caps_unref(gst_caps);
      return absl::FailedPreconditionError(
        absl::StrFormat("Could not fetch the media and encoding type form"
        " the source \"%s\"", source_uri));
  }

  if (g_strcmp0(media_type, "video") != 0 ||
        g_strcmp0(encoding_name, "H264") != 0) {
    std::string media_type_string = media_type;
    std::string encoding_name_string = encoding_name;
    gst_caps_unref(gst_caps);
    return absl::FailedPreconditionError(
      absl::StrFormat("The input media - \"%s\", encoding - \"%s\" "
        "is not supported. Currently the only supported media type is "
        "\"video/x-h264\"", media_type_string, encoding_name_string));
  }

  gst_caps_unref(gst_caps);

  return absl::OkStatus();
}

}  // namespace visionai
