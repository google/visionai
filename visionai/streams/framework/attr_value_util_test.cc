// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/attr_value_util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "visionai/streams/framework/attr_value.pb.h"

namespace visionai {

// A few helpers to construct AttrValue protos.
template <typename T>
AttrValue V(T value) {
  AttrValue ret;
  SetAttrValue(value, &ret);
  return ret;
}

TEST(AttrValueUtil, HasType) {
  // OK.
  EXPECT_TRUE(AttrValueHasType(V(123), "int").ok());
  EXPECT_TRUE(AttrValueHasType(V(1.2f), "float").ok());
  EXPECT_TRUE(AttrValueHasType(V(1.2), "float").ok());
  EXPECT_TRUE(AttrValueHasType(V("hello"), "string").ok());
  EXPECT_TRUE(AttrValueHasType(V(true), "bool").ok());

  // not OK.
  EXPECT_FALSE(AttrValueHasType(V(123), "float").ok());
  EXPECT_FALSE(AttrValueHasType(V(1.2), "int").ok());
  EXPECT_FALSE(AttrValueHasType(V(1.2), "double").ok());
  EXPECT_FALSE(AttrValueHasType(V("hi"), "int").ok());
  EXPECT_FALSE(AttrValueHasType(V(true), "int").ok());
}

TEST(AttrValueUtil, ParseAttrValue) {
  {
    auto attr_value = ParseAttrValue("int", "1");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<int>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_EQ(1, *value);
  }
  {
    auto attr_value = ParseAttrValue("bool", "1");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<bool>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_EQ(true, *value);
  }
  {
    auto attr_value = ParseAttrValue("bool", "true");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<bool>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_EQ(true, *value);
  }
  {
    auto attr_value = ParseAttrValue("string", "hello!");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<std::string>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_EQ("hello!", *value);
  }
  {
    auto attr_value = ParseAttrValue("float", "1.2");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<float>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_FLOAT_EQ(1.2f, *value);
  }
  {
    auto attr_value = ParseAttrValue("float", "-3.14");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<float>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_FLOAT_EQ(-3.14f, *value);
  }
  {
    auto attr_value = ParseAttrValue("float", "-1.3e-2");
    EXPECT_TRUE(attr_value.ok());
    auto value = GetAttrValue<float>(*attr_value);
    EXPECT_TRUE(value.ok());
    EXPECT_FLOAT_EQ(-0.013f, *value);
  }
}

}  // namespace visionai
