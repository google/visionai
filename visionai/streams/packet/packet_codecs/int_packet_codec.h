// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// int_packet_codec.h
// -----------------------------------------------------------------------------
//
// This header defines a packet codec that converts any int object
// into a Packet.
//
// It writes packets of type class "int".

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_INT_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_INT_PACKET_CODEC_H_

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

class IntPacketCodec : public internal::PacketCodecBase<IntPacketCodec, int> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;

 public:
  std::string TypeClassImpl() const { return "int"; }

  absl::Status PackTypeDescriptorImpl(const int& t, TypeDescriptor* td) const {
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const int t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    s->assign(std::to_string(t));
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(int&& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    *s = std::to_string(t);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 int* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination std::string.");
    }
    *t = std::stoi(s);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, std::string&& s,
                                 int* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination std::string.");
    }
    *t = std::stoi(std::move(s));
    return absl::OkStatus();
  }

  ~IntPacketCodec() = default;
  IntPacketCodec() = default;
  IntPacketCodec(const IntPacketCodec&) = delete;
  IntPacketCodec& operator=(const IntPacketCodec&) = delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_INT_PACKET_CODEC_H_
