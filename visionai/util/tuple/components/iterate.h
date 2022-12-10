// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Function template iterate() applies the provided functor specified number
// of times.
//
// iterate<N>(f, val) is equivalent to f(...f(f(val))...) where f() is called N
// times.
//
// iterate<N>(f) is equivalent to f(), f(), ..., f(). This version is
// essentially a specialization of iterate<N>(f, val) for functions that
// accept and return void.
//
// iterate_index() is similar to iterate(). The only difference is that
// it passes the indices of the elements to the functor as the first template
// parameter of type size_t.
//
// iterate_index<N>(f, val) is equivalent to
// f.operator()<N-1>(...f.operator()<1>(f.operator()<0>(val))...).
//
// iterate_index<N>(f) is equivalent to f<0>(), f<1>(), ..., f<N-1>().

#ifndef THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_ITERATE_H_
#define THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_ITERATE_H_

#include <stddef.h>

#include <utility>

#include "visionai/util/tuple/components/ignore_index.h"
#include "visionai/util/tuple/components/internal_iterate.h"

namespace visionai {

namespace internal_iterate {

// All symbols defined within namespace internal_iterate are internal
// to iterate.h. Do not reference them from outside or your code can break
// without notice.

template <class F>
struct index_op {
  template <::size_t N>
  int operator()(int state) const {
    f.template operator()<N>();
    return 0;
  }
  const F& f;
};

}  // namespace internal_iterate

// iterate_index<N>(f, val) is equivalent to
// f.operator()<N-1>(...f.operator()<1>(f.operator()<0>(val))...).
template <::size_t N, class F, class S>
auto iterate_index(const F& f, S&& state)
    -> decltype(internal_iterate::index_folder<N>().template operator()<0>(
        f, ::std::forward<S>(state))) {
  return ::visionai::internal_iterate::index_folder<N>().template operator()<0>(
      f, ::std::forward<S>(state));
}

// iterate<N>(f, val) is equivalent to f(...f(f(val))...) where f() is called N
// times.
template <::size_t N, class F, class S>
auto iterate(const F& f, S&& state)
    -> decltype(internal_iterate::folder<N>()(f, ::std::forward<S>(state))) {
  return ::visionai::internal_iterate::folder<N>()(f, ::std::forward<S>(state));
}

// iterate_index<N>(f) is equivalent to f<0>(), f<1>(), ..., f<N-1>().
template <::size_t N, class F>
void iterate_index(const F& f) {
  ::visionai::iterate_index<N>(internal_iterate::index_op<F>{f}, 0);
}

// iterate<N>(f) is equivalent to f(), f(), ..., f().
template <::size_t N, class F>
void iterate(const F& f) {
  ::visionai::iterate_index<N>(ignore_index_no_args(&f));
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_ITERATE_H_
