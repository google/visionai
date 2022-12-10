/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_TUPLE_APPLY_H_
#define VISIONAI_UTIL_TUPLE_APPLY_H_

#include <tuple>
#include <type_traits>

namespace visionai {

// Loosely an implementation of C++17 std::apply.
//
// Allows one to unpack a tuple into parameters for a given function.
//
// For example:
//   using TwoNumbers = typename std::tuple<int,int>;
//   TwoNumbers numbers(1,2);
//   apply([](int a, int b) {std::cout << a+b;}, numbers);
//
//   Running this will print: 3
//
//
namespace internal {

template <typename Function, typename Tuple, size_t... I>
auto apply_impl(Function&& f, Tuple&& t, std::index_sequence<I...>) {
  return f(std::get<I>(std::forward<Tuple>(t))...);
}

}  // namespace internal

template <typename Function, typename Tuple>
auto apply(Function&& f, Tuple&& t) {
  static constexpr auto size =
      std::tuple_size<std::remove_reference_t<Tuple>>::value;
  return internal::apply_impl(std::forward<Function>(f), std::forward<Tuple>(t),
                              std::make_index_sequence<size>{});
}

}  // namespace visionai

#endif  // VISIONAI_UTIL_TUPLE_APPLY_H_
