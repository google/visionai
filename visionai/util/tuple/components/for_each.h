// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Algorithm for_each() applies the unary function to all elements of the tuple,
// from first to the last.
//
// for_each(f, make_tuple(t1, t2, ..., tn)) is equivalent to
// f(t1), f(t2), ..., f(tn) and evaluates to void. for_each(f, make_tuple())
// has no effect.
//
// for_each_index(f, make_tuple(t1, t2, ..., tn)) is equivalent to
// f.operator()<0>(t1), f.operator()<1>(t2), ..., f.operator()<N-1>(tn).
//
// for_each<tuple<T1, T2, ..., TN>>(f) is equivalent to
// f<T1>(), f<T2>(), ..., f<TN>().
//
// for_each_index<tuple<T1, T2, ..., TN>>(f) is equivalent to
// f.operator()<0, T1>(), f.operator()<1, T2>(), ..., f.operator()<N-1, TN>().
//
// struct Logger {
//   template <class T>
//   void operator()(const T& element) const {
//     LOG(INFO) << element;
//   }
// };
//
// auto t = make_tuple("Hello", 42, 0.5);
// for_each(Logger(), t);
//
// IWYU pragma: private, include "visionai/util/tuple/tuple.h"

#ifndef THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_FOR_EACH_H_
#define THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_FOR_EACH_H_

#include <stddef.h>

#include <utility>

#include "visionai/util/tuple/components/array.h"
#include "visionai/util/tuple/components/ignore_index.h"
#include "visionai/util/tuple/components/intrinsics.h"
#include "visionai/util/tuple/components/iterate.h"
#include "visionai/util/tuple/components/pair.h"
#include "visionai/util/tuple/components/std_tuple.h"

namespace visionai {

namespace internal_for_each {

// All symbols defined within namespace internal_for_each are internal
// to for_each.h. Do not reference them from outside or your code can break
// without notice.

template <class T, class F>
struct value_op {
  template <::size_t N>
  void operator()() const {
    f.template operator()<N>(get<N>(::std::forward<T>(t)));
  }
  T&& t;
  const F& f;
};

template <class T, class F>
struct type_op {
  template <::size_t N>
  void operator()() const {
    f.template operator()<N, typename element<N, T>::type>();
  }
  const F& f;
};

}  // namespace internal_for_each

template <class T, class F>
void for_each_index(const F& f, T&& t) {
  ::visionai::iterate_index<size<T>::value>(
      internal_for_each::value_op<T, F>{::std::forward<T>(t), f});
}

template <class T, class F>
void for_each(const F& f, T&& t) {
  ::visionai::for_each_index(ignore_index(&f), ::std::forward<T>(t));
}

template <class T, class F>
void for_each_index(const F& f) {
  ::visionai::iterate_index<size<T>::value>(
      internal_for_each::type_op<T, F>{f});
}

template <class T, class F>
void for_each(const F& f) {
  ::visionai::for_each_index<T>(ignore_index_no_args(&f));
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_FOR_EACH_H_
