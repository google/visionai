// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_INTRINSICS_H_
#define THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_INTRINSICS_H_

#include <stddef.h>

#include <numeric>
#include <string>
#include <type_traits>
#include <utility>

namespace visionai {

// Class templates tag<T> and intrinsics<Tag> are declared here but not defined.
// They should be specialized for any tuple-like data structure before it can be
// used with algorithms defined in //util/tuple. These are the only extension
// points of the library, nothing else can be specialized. Do not use these
// classed directly, use the facades defined below: assemble, size, element,
// get, etc.

// If the compiler complains about undefined tag<T>::type, you probably need to
// include util/tuple/pair.h or util/tuple/std_tuple.h.
// Unless you have a good reason not to, include util/tuple/tuple.h and avoid
// the hassle of finding the right set of includes for your code.

// A metafunction that associates a type tag with each tuple.
// For example, tag<std::tuple<int, char>>::type is struct std_tuple.
// The type tag must be a complete type.
//
// For any type T either tag<T> or Tag<tag<T>::type> should not be specialized.
// In other words, a type isn't allowed to be both a tuple and a tag.
//
// The second template parameter is an implementation detail and shouldn't be
// explicitly specified when querying for a tag or specializing it.
template <class T, class E = void>
struct tag {};

namespace internal_intrinsics {

// All symbols defined within namespace internal_intrinsics are internal
// to intrinsics.h. Do not reference them from outside or your code can break
// without notice.

template <class T>
using remove_cvref_t =
    typename ::std::remove_cv<typename ::std::remove_reference<T>::type>::type;

template <class T, class E = void>
struct has_get_tuple_tag : std::false_type {};

template <class T>
struct has_get_tuple_tag<T, typename ::std::enable_if<sizeof(get_tuple_tag(
                                ::std::declval<const T&>()))>::type>
    : std::true_type {};

}  // namespace internal_intrinsics

// Another way to associate a tag with a type is to declare function
// get_tuple_tag(const T&) in the same namespace where T is defined. The
// function's return type is the tag. The function shouldn't be defined, a
// declaration is enough.
//
// Note: The specialization is explicitly disabled for references and cv
// qualified types to avoid ambiguity with the partial specialization below.
template <class T>
struct tag<
    T, typename ::std::enable_if<
           ::std::is_same<T, internal_intrinsics::remove_cvref_t<T>>::value &&
           internal_intrinsics::has_get_tuple_tag<T>::value>::type> {
  typedef decltype(get_tuple_tag(::std::declval<const T&>())) type;
};

template <class T>
struct tag<T, typename ::std::enable_if<!::std::is_same<
                  T, internal_intrinsics::remove_cvref_t<T>>::value>::type>
    : tag<internal_intrinsics::remove_cvref_t<T>> {};

// Class template intrinsics<Tag> must be specialized on tag<T>::type.
// Every specialization must have:
//
// * Metafunction assemble<Elements...> that returns the type of the tuple
//   with given element types.
//
// * Metafunction element<N, Tuple> that returns the type of the Nth element
//   in the tuple.
//
// * Metafunction size<Tuple> that returns the size of the tuple.
//
// * Static function template get<N>(Tuple& t) that returns the Nth element
//   of the tuple.
//
// See util/tuple/std_tuple.h and util/tuple/pair.h for example.
template <class Tag>
struct intrinsics;

// Metafunction that returns the type of the tuple with given element types.
//
// For example, assemble<pair_tag, int, char>::type is pair<int, char> and
// assemble<std_tuple_tag, string>::type is std::tuple<string>.
template <class Tag, class... Elements>
struct assemble : intrinsics<Tag>::template assemble<Elements...> {};

// Metafunction that returns the number of elements in the tuple.
// This is the facade for size_impl declared above and which should
// be specialized. Class template size must not be specialized.
//
// assert((size<tuple<int, char>>::value == 2));
template <class T>
struct size : intrinsics<typename tag<T>::type>::template size<
                  internal_intrinsics::remove_cvref_t<T>> {};

// Metafunction that returns the type of the element in the tuple.
// This is the facade for element_impl declared above and which should
// be specialized. Class template element must not be specialized.
//
// element<0, tuple<int, char>>::type is int.
// element<1, tuple<int, char>>::type is char.
template <::size_t N, class T>
struct element : intrinsics<typename tag<T>::type>::template element<
                     N, internal_intrinsics::remove_cvref_t<T>> {};

namespace internal_intrinsics {

// All symbols defined within namespace internal_intrinsics are internal
// to intrinsics.h. Do not reference them from outside or your code can break
// without notice.

// The "invalid index" constant.
typedef ::std::integral_constant<::size_t, static_cast<::size_t>(-1)> npos;

// Linearly searches for the first element in tuple T with type equal to Elem.
// The search is performed within a [I, I + N) subrange of the tuple.
// Returns npos if not found.
template <class Elem, class T, ::size_t I, ::size_t N>
struct find_first
    : ::std::conditional<
          ::std::is_same<Elem, typename element<I, T>::type>::value,
          ::std::integral_constant<::size_t, I>,
          find_first<Elem, T, I + 1, N - 1>>::type {};

template <class Elem, class T, ::size_t I>
struct find_first<Elem, T, I, 0> : npos {};

// Returns the index of the first element in tuple T with type equal to Elem.
// Requires: T must contain exactly one element of type Elem.
template <class Elem, class T>
struct index_of : find_first<Elem, T, 0, size<T>::value> {
  static_assert(
      index_of::value != npos(),
      "Tuple t doesn't contain elements of type Elem in get<Elem>(t)");
  static_assert(
      find_first<Elem, T, index_of::value + 1,
                 size<T>::value - index_of::value - 1>() == npos(),
      "Tuple t contains more than one element of type Elem in get<Elem>(t)");
};

template <class T>
struct mk_void {
  typedef void type;
};

template <class T, class E = void>
struct has_all_elements : ::std::false_type {};

template <class T>
struct has_all_elements<T, typename mk_void<typename intrinsics<
                               typename tag<T>::type>::has_all_elements>::type>
    : intrinsics<typename tag<T>::type>::has_all_elements {};

template <::size_t N, class T>
auto name(int rank)
    -> decltype(intrinsics<typename tag<T>::type>::template name<
                N, remove_cvref_t<T>>()) {
  return intrinsics<typename tag<T>::type>::template name<N,
                                                          remove_cvref_t<T>>();
}

template <::size_t N, class T>
const char* name(unsigned rank) {
  return nullptr;
}

}  // namespace internal_intrinsics

// Returns the name of the Nth field in tuple T, or NULL if it has no name.
template <::size_t N, class T>
const char* name() {
  return internal_intrinsics::name<N, T>(0);
}

// Returns true if T has tuple-like interface and all its elements exist
// simultaneously. Naturally, has_all_elements<tuple<int, string>>::value is
// true.
//
// has_all_elements<union_type<int, string>>::value would be false for a
// hypothetical type union_type<Ts...> that at any moment has only one active
// element.
template <class T>
struct has_all_elements : internal_intrinsics::has_all_elements<T> {};

namespace internal_intrinsics_adl_barrier {

// Returns the Nth element of the tuple. This is the facade for get_impl
// declared above and which should be specialized. Function template get must
// not be specialized.
//
// auto t = make_tuple(42, 0.5);
// assert(get<0>(t) == 42);
// assert(get<1>(t) == 0.5);
//
// The same function is used to mutate tuple elements.
//
// tuple<int, double> t;
// get<0>(t) = 42;
// get<1>(t) = 0.5;
template <::size_t N, class T>
constexpr auto get(T&& t)
    -> decltype(intrinsics<typename tag<T>::type>::template get<N>(
        ::std::forward<T>(t))) {
  return intrinsics<typename tag<T>::type>::template get<N>(
      ::std::forward<T>(t));
}
// Returns a reference to the tuple element of the specified type.
//
// Requires: tuple T must have exactly one element of type Elem.
//
// tuple<int, double, int> t = {1, 3.5, 7};
// assert(get<double>(t) == 3.5);
// get<double>(t) = 3.14;
//
// get<int>(t);   // Compile error: there are two ints in the tuple.
// get<bool>(t);  // Compile error: there are no bools in the tuple.
template <class Elem, class T>
constexpr auto get(T&& t)
    -> decltype(get<internal_intrinsics::index_of<Elem, T>{}>(
        ::std::forward<T>(t))) {
  return get<internal_intrinsics::index_of<Elem, T>{}>(::std::forward<T>(t));
}

}  // namespace internal_intrinsics_adl_barrier

// The using-directive is used to disable ADL for util::tuple::get().
using namespace internal_intrinsics_adl_barrier;

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TUPLE_COMPONENTS_INTRINSICS_H_
