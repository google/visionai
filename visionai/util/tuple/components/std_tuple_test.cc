// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/tuple/components/std_tuple.h"

#include <tuple>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"
#include "visionai/util/tuple/components/intrinsics.h"

namespace visionai {
namespace {

typedef ::std::tuple<int, char> T;

struct D : T {
  using T::T;
};

TEST(StdTuple, Tag) {
  EXPECT_TRUE((::std::is_same<tag<T>::type, std_tuple_tag>::value));
  EXPECT_TRUE((::std::is_same<tag<D>::type, std_tuple_tag>::value));
}

TEST(StdTuple, Assemble) {
  EXPECT_TRUE(
      (::std::is_same<assemble<std_tuple_tag, int, char>::type, T>::value));
}

TEST(StdTuple, Element) {
  EXPECT_TRUE((::std::is_same<element<0, T>::type, int>::value));
  EXPECT_TRUE((::std::is_same<element<1, T>::type, char>::value));
  EXPECT_TRUE((::std::is_same<element<0, D>::type, int>::value));
  EXPECT_TRUE((::std::is_same<element<1, D>::type, char>::value));
}

TEST(StdTuple, Size) {
  EXPECT_EQ(0, (size<::std::tuple<>>::value));
  EXPECT_EQ(1, (size<::std::tuple<int>>::value));
  EXPECT_EQ(2, (size<::std::tuple<int, int>>::value));
  EXPECT_EQ(2, (size<T>::value));
  EXPECT_EQ(2, (size<D>::value));
}

TEST(StdTuple, GetImpl) {
  T t;
  D d;

  // Assignment to fields.
  get<0>(t) = 42;
  get<1>(t) = 'A';
  get<0>(d) = 42;
  get<1>(d) = 'A';

  // Non-const getter.
  EXPECT_EQ(42, get<0>(t));
  EXPECT_EQ('A', get<1>(t));
  EXPECT_EQ(42, get<0>(d));
  EXPECT_EQ('A', get<1>(d));

  // Const getter.
  const T& ct = t;
  EXPECT_EQ(42, get<0>(ct));
  EXPECT_EQ('A', get<1>(ct));
  const D& cd = d;
  EXPECT_EQ(42, get<0>(cd));
  EXPECT_EQ('A', get<1>(cd));
}

template <class T, class U>
bool SameType(U&&) {
  return ::std::is_same<T, U&&>();
}

TEST(StdTuple, GetImplType) {
  int n = 0;
  ::std::tuple<int, const int, int&, const int&, int&&, const int&&> t(
      n, n, n, n, ::std::move(n), ::std::move(n));
  const auto& ct = t;

  EXPECT_TRUE(SameType<int&>(get<int>(t)));
  EXPECT_TRUE(SameType<const int&>(get<const int>(t)));
  EXPECT_TRUE(SameType<int&>(get<int&>(t)));
  EXPECT_TRUE(SameType<const int&>(get<const int&>(t)));
  EXPECT_TRUE(SameType<int&>(get<int&&>(t)));
  EXPECT_TRUE(SameType<const int&>(get<const int&&>(t)));

  EXPECT_TRUE(SameType<const int&>(get<int>(ct)));
  EXPECT_TRUE(SameType<const int&>(get<const int>(ct)));
  EXPECT_TRUE(SameType<int&>(get<int&>(ct)));
  EXPECT_TRUE(SameType<const int&>(get<const int&>(ct)));
  EXPECT_TRUE(SameType<int&>(get<int&&>(ct)));
  EXPECT_TRUE(SameType<const int&>(get<const int&&>(ct)));

  EXPECT_TRUE(SameType<int&&>(get<int>(::std::move(t))));
  EXPECT_TRUE(SameType<const int&&>(get<const int>(::std::move(t))));
  EXPECT_TRUE(SameType<int&>(get<int&>(::std::move(t))));
  EXPECT_TRUE(SameType<const int&>(get<const int&>(::std::move(t))));
  EXPECT_TRUE(SameType<int&&>(get<int&&>(::std::move(t))));
  EXPECT_TRUE(SameType<const int&&>(get<const int&&>(::std::move(t))));

  EXPECT_TRUE(SameType<const int&&>(get<int>(::std::move(ct))));
  EXPECT_TRUE(SameType<const int&&>(get<const int>(::std::move(ct))));
  EXPECT_TRUE(SameType<int&>(get<int&>(::std::move(ct))));
  EXPECT_TRUE(SameType<const int&>(get<const int&>(::std::move(ct))));
  EXPECT_TRUE(SameType<int&&>(get<int&&>(::std::move(ct))));
  EXPECT_TRUE(SameType<const int&&>(get<const int&&>(::std::move(ct))));
}

TEST(StdTuple, Name) {
  EXPECT_EQ(nullptr, (name<0, T>()));
  EXPECT_EQ(nullptr, (name<1, T>()));
  EXPECT_EQ(nullptr, (name<0, D>()));
  EXPECT_EQ(nullptr, (name<1, D>()));
}

TEST(StdTuple, HasAllElements) {
  EXPECT_TRUE(has_all_elements<T>::value);
  EXPECT_TRUE(has_all_elements<D>::value);
}

}  // namespace
}  // namespace visionai
