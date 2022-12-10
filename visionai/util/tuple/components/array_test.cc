// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/tuple/components/array.h"

#include <array>
#include <type_traits>

#include "gtest/gtest.h"
#include "visionai/util/tuple/components/intrinsics.h"

namespace visionai {
namespace {

typedef ::std::array<int, 2> A;

struct D : A {
  using A::A;
};

TEST(Array, Tag) {
  EXPECT_TRUE((::std::is_same<tag<A>::type, array_tag>::value));
  EXPECT_TRUE((::std::is_same<tag<D>::type, array_tag>::value));
}

TEST(Array, Assemble) {
  EXPECT_TRUE((::std::is_same<assemble<array_tag, int, int>::type, A>::value));
}

TEST(Array, Element) {
  EXPECT_TRUE((::std::is_same<element<0, A>::type, int>::value));
  EXPECT_TRUE((::std::is_same<element<1, A>::type, int>::value));
  EXPECT_TRUE((::std::is_same<element<0, D>::type, int>::value));
  EXPECT_TRUE((::std::is_same<element<1, D>::type, int>::value));
}

TEST(Array, Size) {
  EXPECT_EQ(2, size<A>::value);
  EXPECT_EQ(2, size<D>::value);
}

TEST(Array, GetImpl) {
  A a;
  D d;

  // Assignment to fields.
  get<0>(a) = 42;
  get<1>(a) = 24;
  get<0>(d) = 42;
  get<1>(d) = 24;

  // Non-const getter.
  EXPECT_EQ(42, get<0>(a));
  EXPECT_EQ(24, get<1>(a));
  EXPECT_EQ(42, get<0>(d));
  EXPECT_EQ(24, get<1>(d));

  // Const getter.
  const A& ca = a;
  EXPECT_EQ(42, get<0>(ca));
  EXPECT_EQ(24, get<1>(ca));
  const D& cd = d;
  EXPECT_EQ(42, get<0>(cd));
  EXPECT_EQ(24, get<1>(cd));
}

TEST(Array, Name) {
  EXPECT_EQ(nullptr, (name<0, A>()));
  EXPECT_EQ(nullptr, (name<1, A>()));
  EXPECT_EQ(nullptr, (name<0, D>()));
  EXPECT_EQ(nullptr, (name<1, D>()));
}

TEST(Array, HasAllElements) {
  EXPECT_TRUE(has_all_elements<A>::value);
  EXPECT_TRUE(has_all_elements<D>::value);
}

}  // namespace
}  // namespace visionai
