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
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"

namespace visionai {

class Foo : public Capture {
 public:
  Foo() = default;
  ~Foo() override {}
  absl::Status Init(CaptureInitContext* ctx) override {
    return absl::OkStatus();
  }
  absl::Status Run(CaptureRunContext* ctx) override { return absl::OkStatus(); }
  absl::Status Cancel() override { return absl::OkStatus(); }
};

REGISTER_CAPTURE_INTERFACE("Foo")
    .OutputPacketType("foo-packet-type")
    .Attr("attr1", "int")
    .Attr("attr2", "string")
    .Doc("Foo documentation");

REGISTER_CAPTURE_IMPLEMENTATION("Foo", Foo);

TEST(CaptureDefRegistryTest, TestBasic) {
  auto capture_def = CaptureDefRegistry::Global()->LookUp("Foo");
  EXPECT_TRUE(capture_def.ok());
  EXPECT_EQ((*capture_def)->name(), "Foo");
  EXPECT_EQ((*capture_def)->output_packet_type(), "foo-packet-type");
  EXPECT_EQ((*capture_def)->attr_size(), 2);
  EXPECT_EQ((*capture_def)->attr(0).name(), "attr1");
  EXPECT_EQ((*capture_def)->attr(0).type(), "int");
  EXPECT_EQ((*capture_def)->attr(1).name(), "attr2");
  EXPECT_EQ((*capture_def)->attr(1).type(), "string");
  EXPECT_EQ((*capture_def)->doc(), "Foo documentation");
}

TEST(CaptureRegistryTest, TestBasic) {
  auto capture = CaptureRegistry::Global()->CreateCapture("Foo");
  EXPECT_TRUE(capture.ok());
  CaptureInitContext::InitData init_data;
  CaptureInitContext init_ctx(std::move(init_data));
  EXPECT_TRUE((*capture)->Init(&init_ctx).ok());
  CaptureRunContext::RunData run_data;
  CaptureRunContext run_ctx(std::move(run_data));
  EXPECT_TRUE((*capture)->Run(&run_ctx).ok());
  EXPECT_TRUE((*capture)->Cancel().ok());
}

}  // namespace visionai
