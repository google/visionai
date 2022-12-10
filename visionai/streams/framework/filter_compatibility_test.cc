// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/attr_def.pb.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"

namespace visionai {

class Foo : public Filter {
 public:
  Foo() = default;
  ~Foo() override {}
  absl::Status Init(FilterInitContext* ctx) override {
    return absl::OkStatus();
  }
  absl::Status Run(FilterRunContext* ctx) override { return absl::OkStatus(); }
  absl::Status Cancel() override { return absl::OkStatus(); }
};

REGISTER_FILTER_INTERFACE("Foo")
    .InputPacketType("input-foo-packet-type")
    .OutputPacketType("output-foo-packet-type")
    .Attr("attr1", "int")
    .Attr("attr2", "string")
    .Doc("Foo documentation");

REGISTER_FILTER_IMPLEMENTATION("Foo", Foo);

TEST(FilterDefRegistryTest, TestBasic) {
  auto filter_def = FilterDefRegistry::Global()->LookUp("Foo");
  EXPECT_TRUE(filter_def.ok());
  EXPECT_EQ((*filter_def)->name(), "Foo");
  EXPECT_EQ((*filter_def)->input_packet_type(), "input-foo-packet-type");
  EXPECT_EQ((*filter_def)->output_packet_type(), "output-foo-packet-type");
  EXPECT_EQ((*filter_def)->attr_size(), 2);
  EXPECT_EQ((*filter_def)->attr(0).name(), "attr1");
  EXPECT_EQ((*filter_def)->attr(0).type(), "int");
  EXPECT_EQ((*filter_def)->attr(1).name(), "attr2");
  EXPECT_EQ((*filter_def)->attr(1).type(), "string");
  EXPECT_EQ((*filter_def)->doc(), "Foo documentation");
}

TEST(FilterRegistryTest, TestBasic) {
  auto filter = FilterRegistry::Global()->CreateFilter("Foo");
  EXPECT_TRUE(filter.ok());
  FilterInitContext::InitData init_data;
  FilterInitContext init_ctx(std::move(init_data));
  EXPECT_TRUE((*filter)->Init(&init_ctx).ok());
  FilterRunContext::RunData run_data;
  FilterRunContext run_ctx(std::move(run_data));
  EXPECT_TRUE((*filter)->Run(&run_ctx).ok());
  EXPECT_TRUE((*filter)->Cancel().ok());
}

}  // namespace visionai
