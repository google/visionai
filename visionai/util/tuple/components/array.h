// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_ARRAY_H_
#define THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_ARRAY_H_

#include <stddef.h>

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

#include "visionai/util/tuple/components/intrinsics.h"

namespace visionai {

namespace internal_array {

// All symbols defined within namespace internal_array are internal
// to array.h. Do not reference them from outside or your code can break
// without notice.

template <class... Ts>
struct same_types;

template <>
struct same_types<> : ::std::true_type {};

template <class T>
struct same_types<T> : ::std::true_type {};

template <class T, class... Rest>
struct same_types<T, T, Rest...> : same_types<T, Rest...> {};

template <class T, class U, class... Rest>
struct same_types<T, U, Rest...> : ::std::false_type {};

template <class T, ::size_t N>
::std::integral_constant<::size_t, N> size(const ::std::array<T, N>&);

template <class T>
struct array_element {
  using type = T;
};

template <::size_t N, class T, ::size_t M>
array_element<T> element(const ::std::array<T, M>&);

template <class T, ::size_t N>
void accept_array(::std::array<T, N>*);

}  // namespace internal_array

struct array_tag {};

template <class T>
struct tag<T,
           decltype(internal_array::accept_array(static_cast<T*>(nullptr)))> {
  typedef array_tag type;
};

template <>
struct intrinsics<array_tag> {
  using has_all_elements = std::true_type;

  template <class T, class... Ts>
  struct assemble {
    static_assert(internal_array::same_types<T, Ts...>::value,
                  "All elements of std::array must be of the same type");
    typedef ::std::array<T, sizeof...(Ts) + 1> type;
  };

  template <::size_t N, class T>
  using element = decltype(internal_array::element<N>(::std::declval<T>()));

  template <class T>
  using size = decltype(internal_array::size(::std::declval<T>()));

  template <::size_t N, class T>
  static decltype(::std::get<N>(::std::declval<T>())) get(T&& t) {
    return ::std::get<N>(::std::forward<T>(t));
  }

  // std::get() is lacking an overload for const rvalues. We fix this oversight.
  template <::size_t N, class T>
  static const typename element<N, T>::type&& get(const T&& t) {
    return ::std::forward<const typename element<N, T>::type>(
        ::std::get<N>(::std::move(t)));
  }
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_ARRAY_H_
