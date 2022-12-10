// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// packet_codecs.h
// -----------------------------------------------------------------------------
//
// This header is the only public interface that users should include to gain
// access to packet codecs. For the most part, users should not even worry about
// this file and strictly deal with packet.h.
//
// For codec users, only two methods are relevant: Pack and Unpack.
//
// For codec developers, please see packet_codec_base.h for more documentation
// on how to add a new codec and how to extend the set of packet types.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PACKET_CODECS_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PACKET_CODECS_H_

 #include "visionai/streams/packet/packet_codecs/codec_selector.h"

namespace visionai {
namespace packet_codecs {

// Marshals an object of type T into a Packet.
template <typename T>
absl::Status Pack(T&&, google::cloud::visionai::v1::Packet* p);

// Unmarshals a Packet into an object of type T.
template <typename T>
absl::Status Unpack(const google::cloud::visionai::v1::Packet&, T*);
template <typename T>
absl::Status Unpack(google::cloud::visionai::v1::Packet&&, T*);

// ----------------------------------------------------------------------------
// Implementation below.

namespace internal {

template <typename Codec, typename T>
absl::Status PackWithCodec(const Codec& c, T&& t,
                           google::cloud::visionai::v1::Packet* p) {
  VAI_RETURN_IF_ERROR(c.Pack(std::forward<T>(t), p));
  return absl::OkStatus();
}

template <typename Codec, typename T>
absl::Status UnpackWithCodec(const Codec& c,
                             const google::cloud::visionai::v1::Packet& p,
                             T* t) {
  VAI_RETURN_IF_ERROR(c.Unpack(p, t));
  return absl::OkStatus();
}

template <typename Codec, typename T>
absl::Status UnpackWithCodec(const Codec& c,
                             google::cloud::visionai::v1::Packet&& p,
                             T* t) {
  VAI_RETURN_IF_ERROR(c.Unpack(std::move(p), t));
  return absl::OkStatus();
}

}  // namespace internal

template <typename T>
absl::Status Pack(T&& t, google::cloud::visionai::v1::Packet* p) {
  typename internal::TypeToCodec<typename std::remove_reference<T>::type>::Codec
      c;
  VAI_RETURN_IF_ERROR(PackWithCodec(c, std::forward<T>(t), p));
  return absl::OkStatus();
}

template <typename T>
absl::Status Unpack(const google::cloud::visionai::v1::Packet& p, T* t) {
  typename internal::TypeToCodec<typename std::remove_reference<T>::type>::Codec
      c;
  VAI_RETURN_IF_ERROR(UnpackWithCodec(c, p, t));
  return absl::OkStatus();
}

template <typename T>
absl::Status Unpack(google::cloud::visionai::v1::Packet&& p, T* t) {
  typename internal::TypeToCodec<typename std::remove_reference<T>::type>::Codec
      c;
  VAI_RETURN_IF_ERROR(UnpackWithCodec(c, std::move(p), t));
  return absl::OkStatus();
}

}  // namespace packet_codecs

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PACKET_CODECS_H_
