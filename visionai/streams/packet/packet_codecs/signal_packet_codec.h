// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// signal_packet_codec.h
// -----------------------------------------------------------------------------
//
// This header defines a packet codec that converts a visionai::Signal object
// into a Packet.
//
// It writes packets of type class "signal", and the subtype is the specific
// code, e.g. phantom.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_SIGNAL_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_SIGNAL_PACKET_CODEC_H_

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/streams/packet/packet_codecs/packet_codec_base.h"
#include "visionai/types/signal.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace packet_codecs {

class SignalPacketCodec
    : public internal::PacketCodecBase<SignalPacketCodec, Signal> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;

 public:
  std::string TypeClassImpl() const { return "signal"; }

  absl::Status PackTypeDescriptorImpl(const Signal& t,
                                      TypeDescriptor* td) const {
    if (td == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the type descriptor.");
    }

    auto code_string = ToString(t.code());
    if (!code_string.ok()) {
      return code_string.status();
    }
    td->set_type(*code_string);
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    auto code = ToSignalCode(td.type());
    if (!code.ok()) {
      return code.status();
    }
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const Signal& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    s->clear();
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 Signal* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination Signl.");
    }

    auto code = ToSignalCode(td.type());
    if (!code.ok()) {
      return code.status();
    }
    *t = Signal(*code);

    return absl::OkStatus();
  }

  ~SignalPacketCodec() = default;
  SignalPacketCodec() = default;
  SignalPacketCodec(const SignalPacketCodec&) = delete;
  SignalPacketCodec& operator=(const SignalPacketCodec&) = delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_SIGNAL_PACKET_CODEC_H_
