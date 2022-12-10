// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/tuple/components/intrinsics.h"

#include <stddef.h>

#include <string>
#include <type_traits>

#include "absl/strings/str_cat.h"
#include "gtest/gtest.h"

namespace visionai {

namespace {

struct S {};
struct Q {};

struct TagS {};
struct TagQ {};

TagQ get_tuple_tag(Q q);

}  // namespace

template <>
struct tag<S> {
  typedef TagS type;
};

template <>
struct intrinsics<TagS> {
  template <::size_t N, class T>
  struct element : ::std::integral_constant<int, N> {
    static_assert(::std::is_same<T, S>::value, "Wrong template argument");
  };

  template <class T>
  struct size : ::std::integral_constant<::size_t, 42> {
    static_assert(::std::is_same<T, S>::value, "Wrong template argument");
  };

  template <::size_t N, class T>
  static int get(T&& t) {
    static_assert(::std::is_same<typename ::std::decay<T>::type, S>::value,
                  "Wrong template argument");
    return N;
  }

  template <::size_t N, class T>
  static const char* name() {
    static const ::std::string* const res =
        new ::std::string(absl::StrCat("S", N));
    return res->c_str();
  }
};

template <>
struct intrinsics<TagQ> {
  using has_all_elements = std::true_type;

  template <::size_t N, class T>
  struct element : ::std::integral_constant<int, N> {
    static_assert(::std::is_same<T, Q>::value, "Wrong template argument");
  };

  template <class T>
  struct size : ::std::integral_constant<::size_t, 42> {
    static_assert(::std::is_same<T, Q>::value, "Wrong template argument");
  };

  template <::size_t N, class T>
  static int get(T&& t) {
    static_assert(::std::is_same<typename ::std::decay<T>::type, Q>::value,
                  "Wrong template argument");
    return N;
  }

  template <::size_t N, class T>
  static const char* name() {
    static const ::std::string* const res =
        new ::std::string(absl::StrCat("Q", N));
    return res->c_str();
  }
};

namespace {

template <class T, ::size_t N, int R>
void VerifyElement() {
  EXPECT_TRUE((::std::is_same<typename element<N, T>::type,
                              ::std::integral_constant<int, R>>::value));
}

TEST(Intrinsics, Element) {
  VerifyElement<S, 0, 0>();
  VerifyElement<S, 1, 1>();
  VerifyElement<Q, 0, 0>();
  VerifyElement<Q, 1, 1>();

  VerifyElement<const S, 0, 0>();
  VerifyElement<S&, 0, 0>();
  VerifyElement<volatile S, 0, 0>();
  VerifyElement<const volatile S&, 0, 0>();
  VerifyElement<const volatile S&&, 0, 0>();
  VerifyElement<const Q, 0, 0>();
  VerifyElement<Q&, 0, 0>();
  VerifyElement<volatile Q, 0, 0>();
  VerifyElement<const volatile Q&, 0, 0>();
  VerifyElement<const volatile Q&&, 0, 0>();
}

TEST(Intrinsics, Size) {
  EXPECT_EQ(42, size<S>::value);
  EXPECT_EQ(42, size<const S>::value);
  EXPECT_EQ(42, size<volatile S>::value);
  EXPECT_EQ(42, size<S&>::value);
  EXPECT_EQ(42, size<const volatile S&>::value);
  EXPECT_EQ(42, size<const volatile S&&>::value);

  EXPECT_EQ(42, size<Q>::value);
  EXPECT_EQ(42, size<const Q>::value);
  EXPECT_EQ(42, size<volatile Q>::value);
  EXPECT_EQ(42, size<Q&>::value);
  EXPECT_EQ(42, size<const volatile Q&>::value);
  EXPECT_EQ(42, size<const volatile Q&&>::value);
}

template <class T>
T Make() {
  return T();
}

TEST(Intrinsics, Get) {
  S s = {};
  const S cs = {};
  volatile S vs = {};
  const volatile S cvs = {};

  Q q = {};
  const Q cq = {};
  volatile Q vq = {};
  const volatile Q cvq = {};

  EXPECT_EQ(0, get<0>(s));
  EXPECT_EQ(1, get<1>(s));
  EXPECT_EQ(0, get<0>(q));
  EXPECT_EQ(1, get<1>(q));

  // With lvalues.
  EXPECT_EQ(0, get<0>(cs));
  EXPECT_EQ(0, get<0>(vs));
  EXPECT_EQ(0, get<0>(cvs));
  EXPECT_EQ(0, get<0>(cq));
  EXPECT_EQ(0, get<0>(vq));
  EXPECT_EQ(0, get<0>(cvq));

  // With rvalues.
  EXPECT_EQ(0, get<0>(Make<S>()));
  EXPECT_EQ(0, get<0>(Make<const S>()));
  EXPECT_EQ(0, get<0>(Make<Q>()));
  EXPECT_EQ(0, get<0>(Make<const Q>()));
}

TEST(Intrinsics, GetByType) {
  S s = {};
  const S cs = {};
  volatile S vs = {};
  const volatile S cvs = {};

  Q q = {};
  const Q cq = {};
  volatile Q vq = {};
  const volatile Q cvq = {};

  typedef ::std::integral_constant<int, 0> Zero;
  typedef ::std::integral_constant<int, 1> One;

  EXPECT_EQ(0, get<Zero>(s));
  EXPECT_EQ(1, get<One>(s));
  EXPECT_EQ(0, get<Zero>(q));
  EXPECT_EQ(1, get<One>(q));

  // With lvalues.
  EXPECT_EQ(0, get<Zero>(cs));
  EXPECT_EQ(0, get<Zero>(vs));
  EXPECT_EQ(0, get<Zero>(cvs));
  EXPECT_EQ(0, get<Zero>(cq));
  EXPECT_EQ(0, get<Zero>(vq));
  EXPECT_EQ(0, get<Zero>(cvq));

  // With rvalues.
  EXPECT_EQ(0, get<Zero>(Make<S>()));
  EXPECT_EQ(0, get<Zero>(Make<const S>()));
  EXPECT_EQ(0, get<Zero>(Make<Q>()));
  EXPECT_EQ(0, get<Zero>(Make<const Q>()));
}

TEST(Intrinsics, Name) {
  EXPECT_STREQ("S0", (name<0, S>()));
  EXPECT_STREQ("S1", (name<1, S>()));
  EXPECT_STREQ("Q0", (name<0, Q>()));
  EXPECT_STREQ("Q1", (name<1, Q>()));

  EXPECT_STREQ("S0", (name<0, const S>()));
  EXPECT_STREQ("S0", (name<0, volatile S>()));
  EXPECT_STREQ("S0", (name<0, S&>()));
  EXPECT_STREQ("S0", (name<0, const volatile S&>()));
  EXPECT_STREQ("S0", (name<0, const volatile S&&>()));
}

TEST(Intrinsics, HasAllElements) {
  // Not even a tuple.
  EXPECT_FALSE(has_all_elements<void>::value);
  EXPECT_FALSE(has_all_elements<int>::value);
  // Doesn't define the has_all_elements intrinsic.
  EXPECT_FALSE(has_all_elements<S>::value);
  // Defines the has_all_elements intrinsic.
  EXPECT_TRUE(has_all_elements<Q>::value);
}

}  // namespace

}  // namespace visionai
