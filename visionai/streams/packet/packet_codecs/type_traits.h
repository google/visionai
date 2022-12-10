// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_TYPE_TRAITS_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_TYPE_TRAITS_H_

#include <string>
#include <type_traits>

namespace visionai {

// We are using a similar pattern documented in
// https://en.cppreference.com/w/cpp/types/void_t
// to detect the presence of member functions we think are unique to protobuf
// messages. The is_protobuf_helper and conditional_t is used to simulate the
// behavior of void_t, which is only available in C++17 (we are currently on
// C++14 for this project).
template <typename T, typename _ = void>
struct is_protobuf : std::false_type {};

template <typename... Ts>
struct is_protobuf_helper {};

template <typename T>
struct is_protobuf<
    T, std::conditional_t<
           false,
           is_protobuf_helper<
               decltype(std::declval<std::remove_const_t<T>>().ParseFromString(
                   std::declval<std::string>())),
               decltype(std::declval<T>().SerializeToString(nullptr)),
               decltype(std::declval<T>().GetTypeName())>,
           void>> : public std::true_type {};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_TYPE_TRAITS_H_
