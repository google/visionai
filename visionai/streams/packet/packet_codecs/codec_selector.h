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
// The traits in this file decide which C++ types are able to convert to/from a
// Packet. Moreover, it decides which specific codec will be used to accomplish
// that conversion.
//
// For developers looking to extend the set of packet types, you must add
// specializations below to map your desired C++ types to your new packet codec,
// which you must also write (see packet_codec_base.h for documentation on how
// to write a new codec.)

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_CODEC_SELECTOR_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_CODEC_SELECTOR_H_

#include <string>
#include <type_traits>

 #include "visionai/streams/packet/packet_codecs/gstreamer_buffer_packet_codec.h"
 #include "visionai/streams/packet/packet_codecs/int_packet_codec.h"
 #include "visionai/streams/packet/packet_codecs/protobuf_packet_codec.h"
 #include "visionai/streams/packet/packet_codecs/raw_image_packet_codec.h"
 #include "visionai/streams/packet/packet_codecs/signal_packet_codec.h"
 #include "visionai/streams/packet/packet_codecs/string_packet_codec.h"
 #include "visionai/streams/packet/packet_codecs/type_traits.h"
 #include "visionai/types/gstreamer_buffer.h"

namespace visionai {
namespace packet_codecs {
namespace internal {

// ----------------------------------------------------------------------------
// Base type traits.
// ----------------------------------------------------------------------------
//
// Please specialize both templates to decide which C++ types convert to
// Packets, as well as which codec will be used to accomplish it.

template <typename T, typename Enable = void>
struct IsValidDataType;

template <typename T, typename Enable = void>
struct TypeToCodec {
  static_assert(IsValidDataType<T>::value,
                "Specified data type is not supported.");
};

// ----------------------------------------------------------------------------
// Specializations below
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// For std::string.
template <>
struct IsValidDataType<std::string> {
  static constexpr bool value = true;
};

template <>
struct TypeToCodec<std::string> {
  typedef std::string Type;
  typedef StringPacketCodec Codec;
};

// ----------------------------------------------------------------------------
// For GstreamerBuffers.
template <>
struct IsValidDataType<GstreamerBuffer> {
  static constexpr bool value = true;
};

template <>
struct TypeToCodec<GstreamerBuffer> {
  typedef GstreamerBuffer Type;
  typedef GstreamerBufferPacketCodec Codec;
};

// ----------------------------------------------------------------------------
// For RawImages.
template <>
struct IsValidDataType<RawImage> {
  static constexpr bool value = true;
};

template <>
struct TypeToCodec<RawImage> {
  typedef RawImage Type;
  typedef RawImagePacketCodec Codec;
};

// ----------------------------------------------------------------------------
// For Signals.
template <>
struct IsValidDataType<Signal> {
  static constexpr bool value = true;
};

template <>
struct TypeToCodec<Signal> {
  typedef Signal Type;
  typedef SignalPacketCodec Codec;
};

// ----------------------------------------------------------------------------
// For int.
template <>
struct IsValidDataType<int> {
  static constexpr bool value = true;
};

template <>
struct TypeToCodec<int> {
  typedef int Type;
  typedef IntPacketCodec Codec;
};

// ----------------------------------------------------------------------------
// For all protobufs.
template <typename T>
struct IsValidDataType<T,
                       typename std::enable_if<is_protobuf<T>::value>::type> {
  static constexpr bool value = true;
};

template <typename T>
struct TypeToCodec<T, typename std::enable_if<is_protobuf<T>::value>::type> {
  typedef std::remove_reference_t<T> Type;
  typedef ProtobufPacketCodec<std::remove_reference_t<T>> Codec;
};

}  // namespace internal
}  // namespace packet_codecs
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_CODEC_SELECTOR_H_
