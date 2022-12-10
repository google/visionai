// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_PAIR_H_
#define THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_PAIR_H_

#include <stddef.h>

#include <tuple>
#include <utility>

#include "visionai/util/tuple/components/intrinsics.h"

namespace visionai {

namespace internal_pair {

// All symbols defined within namespace internal_pair are internal
// to pair.h. Do not reference them from outside or your code can break
// without notice.

template <::size_t N, class T, class U>
::std::tuple_element<N, std::pair<T, U>> element(const std::pair<T, U>&);

template <class T, class U>
void accept_pair(::std::pair<T, U>*);

}  // namespace internal_pair

struct pair_tag {};

template <class T>
struct tag<T, decltype(internal_pair::accept_pair(static_cast<T*>(nullptr)))> {
  typedef pair_tag type;
};

template <>
struct intrinsics<pair_tag> {
  using has_all_elements = std::true_type;

  template <class T, class U>
  struct assemble {
    typedef std::pair<T, U> type;
  };

  template <::size_t N, class T>
  using element = decltype(internal_pair::element<N>(::std::declval<T>()));

  template <class T>
  using size = ::std::integral_constant<::size_t, 2>;

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

#endif  // THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_PAIR_H_
