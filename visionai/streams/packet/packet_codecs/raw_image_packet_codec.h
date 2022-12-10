// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// raw_image_packet_codec.h
// -----------------------------------------------------------------------------
//
// This header defines a packet codec that converts a visionai::RawImage object
// into a Packet.
//
// It writes packets of type class "raw-image", and the subtype is the specific
// format, e.g. srgb.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_RAW_IMAGE_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_RAW_IMAGE_PACKET_CODEC_H_

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "visionai/streams/packet/packet_codecs/packet_codec_base.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace packet_codecs {

class RawImagePacketCodec
    : public internal::PacketCodecBase<RawImagePacketCodec, RawImage> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;
  typedef google::cloud::visionai::v1::RawImageDescriptor
      RawImageDescriptor;

 public:
  std::string TypeClassImpl() const { return "raw-image"; }

  absl::Status PackTypeDescriptorImpl(const RawImage& t,
                                      TypeDescriptor* td) const {
    if (td == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the type descriptor.");
    }

    auto format_string = ToString(t.format());
    if (!format_string.ok()) {
      return absl::InvalidArgumentError(
          "Given a RawImage of an unknown format.");
    }
    td->set_type(*format_string);

    RawImageDescriptor* descriptor = td->mutable_raw_image_descriptor();
    descriptor->set_format(*format_string);
    descriptor->set_height(t.height());
    descriptor->set_width(t.width());
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    if (!td.has_raw_image_descriptor()) {
      return absl::InvalidArgumentError("No RawImageDescriptor found.");
    }

    auto format = ToRawImageFormat(td.raw_image_descriptor().format());
    if (!format.ok()) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Got unrecognized raw image format string \"%s\"",
                          td.raw_image_descriptor().format()));
    }

    int height = td.raw_image_descriptor().height();
    int width = td.raw_image_descriptor().width();

    // Check the image dimensions are positive.
    if (height < 0 || width < 0) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Got a raw image requiring negative dimensions (h=%d, w=%d).", height,
          width));
    }

    // Check the image dimensions don't overflow.
    auto buf_size = GetRawImageBufferSize(height, width, *format);
    if (!buf_size.ok()) {
      return absl::InvalidArgumentError(
          "Got a raw image packet that will overflow buffer sizes.");
    }

    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const RawImage& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    s->assign(t.data(), t.data() + t.size());
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(RawImage&& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    *s = std::move(t).ReleaseBuffer();
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 RawImage* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination RawImage.");
    }

    int height = td.raw_image_descriptor().height();
    int width = td.raw_image_descriptor().width();
    auto format = ToRawImageFormat(td.raw_image_descriptor().format());
    if (!format.ok()) {
      return absl::InternalError(
          "Got an unknown image format; this should have been checked while "
          "validating the type descriptor.");
    }

    RawImage r(height, width, *format);
    if (s.size() != r.size()) {
      return absl::InvalidArgumentError(
          absl::StrFormat("The payload has size %d, but the image requires %d.",
                          s.size(), r.size()));
    }
    VAI_RETURN_IF_ERROR(r.assign(s));
    *t = std::move(r);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, std::string&& s,
                                 RawImage* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination RawImage.");
    }
    int height = td.raw_image_descriptor().height();
    int width = td.raw_image_descriptor().width();
    auto format = ToRawImageFormat(td.raw_image_descriptor().format());
    if (!format.ok()) {
      return absl::InternalError(
          "Got an unknown image format; this should have been checked while "
          "validating the type descriptor.");
    }

    auto buf_size = GetRawImageBufferSize(height, width, *format);
    if (!buf_size.ok()) {
      return absl::InternalError(
          "Got a raw image packet that will overflow buffer sizes; this should "
          "have been checked while validating the type descriptor.");
    }
    if (s.size() != static_cast<size_t>(*buf_size)) {
      return absl::InvalidArgumentError(
          absl::StrFormat("The payload has size %d, but the image requires %d.",
                          s.size(), *buf_size));
    }

    RawImage r(height, width, *format, std::move(s));
    *t = std::move(r);
    return absl::OkStatus();
  }

  ~RawImagePacketCodec() = default;
  RawImagePacketCodec() = default;
  RawImagePacketCodec(const RawImagePacketCodec&) = delete;
  RawImagePacketCodec& operator=(const RawImagePacketCodec&) = delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_RAW_IMAGE_PACKET_CODEC_H_
