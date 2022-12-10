// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// string_packet_codec.h
// -----------------------------------------------------------------------------
//
// This header defines a packet codec that converts any std::string object
// into a Packet.
//
// It writes packets of type class "string".

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_STRING_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_STRING_PACKET_CODEC_H_

#include <string>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "visionai/streams/packet/packet_codecs/packet_codec_base.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace packet_codecs {

class StringPacketCodec
    : public internal::PacketCodecBase<StringPacketCodec, std::string> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;

 public:
  std::string TypeClassImpl() const { return "string"; }

  absl::Status PackTypeDescriptorImpl(const std::string& t,
                                      TypeDescriptor* td) const {
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const std::string& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    s->assign(t);
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(std::string&& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    *s = std::move(t);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 std::string* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination std::string.");
    }
    t->assign(s);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, std::string&& s,
                                 std::string* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination std::string.");
    }
    *t = std::move(s);
    return absl::OkStatus();
  }

  ~StringPacketCodec() = default;
  StringPacketCodec() = default;
  StringPacketCodec(const StringPacketCodec&) = delete;
  StringPacketCodec& operator=(const StringPacketCodec&) = delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_STRING_PACKET_CODEC_H_
