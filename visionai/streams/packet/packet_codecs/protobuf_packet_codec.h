// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// protobuf_packet_codec.h
// -----------------------------------------------------------------------------
//
// This header defines a packet codec that converts any protobuf object into a
// Packet.
//
// It writes packets of type class "prototype", and its subtype is the fully
// qualified name of the protobuf message.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PROTOBUF_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PROTOBUF_PACKET_CODEC_H_

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "visionai/streams/packet/packet_codecs/packet_codec_base.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace packet_codecs {

template <typename T>
class ProtobufPacketCodec
    : public internal::PacketCodecBase<ProtobufPacketCodec<T>, T> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;

 public:
  std::string TypeClassImpl() const { return "protobuf"; }

  absl::Status PackTypeDescriptorImpl(const T& t, TypeDescriptor* td) const {
    *td->mutable_type() = t.GetTypeName();
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    if (td.type() != T().GetTypeName()) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Got protobuf of type \"%s\" but expected \"%s\".",
                          td.type(), T().GetTypeName()));
    }
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const T& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    if (!t.SerializeToString(s)) {
      return absl::InternalError(
          "Failed to serialize the protobuf into the payload string.");
    }
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 T* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination object.");
    }
    if (!t->ParseFromString(s)) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Failed to parse the given string as a %s.", T().GetTypeName()));
    }
    return absl::OkStatus();
  }

  ~ProtobufPacketCodec() = default;
  ProtobufPacketCodec() = default;
  ProtobufPacketCodec(const ProtobufPacketCodec&) = delete;
  ProtobufPacketCodec& operator=(const ProtobufPacketCodec&) = delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PROTOBUF_PACKET_CODEC_H_
