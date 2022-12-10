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
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/proto/ingester_config.pb.h"

namespace visionai {

class Foo : public EventWriter {
 public:
  Foo() = default;
  ~Foo() override {}
  absl::Status Init(EventWriterInitContext* ctx) override {
    return absl::OkStatus();
  }
  absl::Status Open(absl::string_view event_id) override {
    return absl::OkStatus();
  }
  absl::Status Write(Packet) override { return absl::OkStatus(); }
  absl::Status Close() override { return absl::OkStatus(); }
};

REGISTER_EVENT_WRITER_INTERFACE("Foo")
    .InputPacketType("input-foo-packet-type")
    .Attr("attr1", "int")
    .Attr("attr2", "string")
    .Doc("Foo documentation");

REGISTER_EVENT_WRITER_IMPLEMENTATION("Foo", Foo);

TEST(EventWriterDefRegistryTest, TestBasic) {
  auto event_writer_def = EventWriterDefRegistry::Global()->LookUp("Foo");
  EXPECT_TRUE(event_writer_def.ok());
  EXPECT_EQ((*event_writer_def)->name(), "Foo");
  EXPECT_EQ((*event_writer_def)->input_packet_type(), "input-foo-packet-type");
  EXPECT_EQ((*event_writer_def)->attr_size(), 2);
  EXPECT_EQ((*event_writer_def)->attr(0).name(), "attr1");
  EXPECT_EQ((*event_writer_def)->attr(0).type(), "int");
  EXPECT_EQ((*event_writer_def)->attr(1).name(), "attr2");
  EXPECT_EQ((*event_writer_def)->attr(1).type(), "string");
  EXPECT_EQ((*event_writer_def)->doc(), "Foo documentation");
}

TEST(EventWriterRegistryTest, TestBasic) {
  auto event_writer = EventWriterRegistry::Global()->CreateEventWriter("Foo");
  EXPECT_TRUE(event_writer.ok());
  EventWriterConfig config;
  config.set_name("Foo");
  auto init_ctx_statusor = EventWriterInitContext::Create(config);
  EXPECT_TRUE(init_ctx_statusor.ok());
  Packet p;
  EXPECT_TRUE((*event_writer)->Init(init_ctx_statusor->get()).ok());
  EXPECT_TRUE((*event_writer)->Open("some-event").ok());
  EXPECT_TRUE((*event_writer)->Write(std::move(p)).ok());
  EXPECT_TRUE((*event_writer)->Close().ok());
}

}  // namespace visionai
