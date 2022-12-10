/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_NO_COPY_PACKET_CODEC_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_NO_COPY_PACKET_CODEC_H_

#include <string>
#include <utility>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "visionai/streams/packet/packet_codecs/packet_codec_base.h"
#include "visionai/types/no_copy.h"

namespace visionai {
namespace packet_codecs {

class NoCopyPacketCodec
    : public internal::PacketCodecBase<NoCopyPacketCodec, NoCopy> {
 private:
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;

 public:
  std::string TypeClassImpl() const { return "NoCopy"; }

  absl::Status PackTypeDescriptorImpl(const NoCopy& t,
                                      TypeDescriptor* td) const {
    return absl::OkStatus();
  }

  absl::Status ValidateTypeDescriptorImpl(const TypeDescriptor& td) const {
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(const NoCopy& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    s->assign(t.data_);
    return absl::OkStatus();
  }

  absl::Status PackPayloadImpl(NoCopy&& t, std::string* s) const {
    if (s == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the payload string.");
    }
    *s = std::move(t.data_);
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, const std::string& s,
                                 NoCopy* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination NoCopy.");
    }
    t->data_ = s;
    return absl::OkStatus();
  }

  absl::Status UnpackPayloadImpl(const TypeDescriptor& td, std::string&& s,
                                 NoCopy* t) const {
    if (t == nullptr) {
      return absl::InvalidArgumentError(
          "Given a nullptr to the destination NoCopy.");
    }
    t->data_ = std::move(s);
    return absl::OkStatus();
  }

  ~NoCopyPacketCodec() = default;
  NoCopyPacketCodec() = default;
  NoCopyPacketCodec(const NoCopyPacketCodec&) = delete;
  NoCopyPacketCodec& operator=(const NoCopyPacketCodec&) = delete;
};

}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_NO_COPY_PACKET_CODEC_H_
