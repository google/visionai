/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_TUPLE_TEST_UTIL_H_
#define THIRD_PARTY_VISIONAI_UTIL_TUPLE_TEST_UTIL_H_

#include "gtest/gtest.h"

namespace visionai {

class TestValues : public ::testing::Test {
 public:
  template <int N>
  class Value {
   public:
    static const int value = N;
    static const Value instance;

   private:
    Value() {}
    friend class TestValues;
    // this is a workaround for a bug in the gcc standard library.
    // make_tuple(make_tuple(a)) doesn't work if 'a' is an instance of an empty
    // struct.
    // // TODO(romanp): remove this field after the release of crosstool 17.
    int dummy;
  };

  typedef Value<0> A;
  typedef Value<1> B;
  typedef Value<2> C;
  typedef Value<3> D;

  typedef Value<-4> W;
  typedef Value<-3> X;
  typedef Value<-2> Y;
  typedef Value<-1> Z;

  A a;
  B b;
  C c;
  D d;

  W w;
  X x;
  Y y;
  Z z;
};

template <int N>
const int TestValues::Value<N>::value;

template <int N>
const TestValues::Value<N> TestValues::Value<N>::instance;

template <int N>
bool operator==(TestValues::Value<N> a, TestValues::Value<N> b) {
  return true;
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TUPLE_TEST_UTIL_H_
