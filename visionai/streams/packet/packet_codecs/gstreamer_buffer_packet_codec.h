// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// gstreamer_buffer_packet_codec.h
// -----------------------------------------------------------------------------
//
// This header defines a packet codec that converts any GstreamerBuffer objects
// into a Packet.
//
// It writes packets of type class "gst", and its subtype is the media type of
// the corresponding gst caps.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_GSTREAMER_BUFFER_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_GSTREAMER_BUFFER_PACKET_CODEC_H_

#include <string>
#include <utility>

#include "google/protobuf/util/time_util.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "visionai/streams/packet/packet_codecs/packet_codec_base.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace packet_codecs {
using ::google::protobuf::util::TimeUtil;

class GstreamerBufferPacketCodec
    : public internal::PacketCodecBase<GstreamerBufferPacketCodec,
                                       GstreamerBuffer> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;
  typedef google::cloud::visionai::v1::GstreamerBufferDescriptor
      GstreamerBufferDescriptor;

 public:
  std::string TypeClassImpl() const { return "gst"; }

  absl::Status PackTypeDescriptorImpl(const GstreamerBuffer& t,
                                      TypeDescriptor* td) const {
    if (td == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the type descriptor.");
    }
    *td->mutable_type() = t.media_type();
    GstreamerBufferDescriptor* descriptor =
        td->mutable_gstreamer_buffer_descriptor();
    *descriptor->mutable_caps_string() = t.caps_string();
    descriptor->set_is_key_frame(t.is_key_frame());
    *descriptor->mutable_pts_time() =
        TimeUtil::NanosecondsToTimestamp(t.get_pts());
    *descriptor->mutable_dts_time() =
        TimeUtil::NanosecondsToTimestamp(t.get_dts());
    *descriptor->mutable_duration() =
        TimeUtil::NanosecondsToDuration(t.get_duration());
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    if (!td.has_gstreamer_buffer_descriptor()) {
      return absl::InvalidArgumentError("No GstreamerBufferDescriptor found.");
    }
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const GstreamerBuffer& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    s->assign(t.data(), t.size());
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(GstreamerBuffer&& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    *s = std::move(t).ReleaseBuffer();
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 GstreamerBuffer* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination GstreamerBuffer.");
    }
    t->set_caps_string(td.gstreamer_buffer_descriptor().caps_string());
    t->set_is_key_frame(td.gstreamer_buffer_descriptor().is_key_frame());
    t->set_pts(TimeUtil::TimestampToNanoseconds(
        td.gstreamer_buffer_descriptor().pts_time()));
    t->set_dts(TimeUtil::TimestampToNanoseconds(
        td.gstreamer_buffer_descriptor().dts_time()));
    t->set_duration(TimeUtil::DurationToNanoseconds(
        td.gstreamer_buffer_descriptor().duration()));
    t->assign(s);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, std::string&& s,
                                 GstreamerBuffer* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination GstreamerBuffer.");
    }
    t->set_caps_string(td.gstreamer_buffer_descriptor().caps_string());
    t->set_is_key_frame(td.gstreamer_buffer_descriptor().is_key_frame());
    t->set_pts(TimeUtil::TimestampToNanoseconds(
        td.gstreamer_buffer_descriptor().pts_time()));
    t->set_dts(TimeUtil::TimestampToNanoseconds(
        td.gstreamer_buffer_descriptor().dts_time()));
    t->set_duration(TimeUtil::DurationToNanoseconds(
        td.gstreamer_buffer_descriptor().duration()));
    t->assign(std::move(s));
    return absl::OkStatus();
  }

  ~GstreamerBufferPacketCodec() = default;
  GstreamerBufferPacketCodec() = default;
  GstreamerBufferPacketCodec(const GstreamerBufferPacketCodec&) = delete;
  GstreamerBufferPacketCodec& operator=(const GstreamerBufferPacketCodec&) =
      delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_GSTREAMER_BUFFER_PACKET_CODEC_H_
