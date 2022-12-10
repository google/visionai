/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_TUPLE_FOR_EACH_H_
#define VISIONAI_UTIL_TUPLE_FOR_EACH_H_

#include <tuple>

namespace visionai {

// Allows one to apply a function to each element of a tuple. The function can
// accept the tuple element by value or reference.
//
// For example:
//   using TwoNumbers = typename std::tuple<int,int>;
//   TwoNumbers numbers;
//   int i = 0;
//   for_each(numbers, [&i](auto& number) {number = i;});
//
//   for_each(numbers, [](auto number) {std::cout << number << " ";});
//
//   Running this will print:
//   0 1
//
template <std::size_t I = 0, typename Func, typename... T>
inline typename std::enable_if<I == sizeof...(T), void>::type for_each(
    std::tuple<T...>&, Func) {}

template <std::size_t I = 0, typename Func, typename... T>
    inline typename std::enable_if <
    I<sizeof...(T), void>::type for_each(std::tuple<T...>& t, Func f) {
  f(std::get<I>(t));
  for_each<I + 1, Func, T...>(t, f);
}

}  // namespace visionai

#endif  // VISIONAI_UTIL_TUPLE_FOR_EACH_H_
