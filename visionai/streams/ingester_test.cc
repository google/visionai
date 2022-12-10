// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/ingester.h"

#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

class TestCapture : public Capture {
 public:
  TestCapture() {}
  ~TestCapture() override {}
  absl::Status Init(CaptureInitContext* ctx) override {
    VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("rounds", &rounds_));
    return absl::OkStatus();
  }
  absl::Status Run(CaptureRunContext* ctx) override {
    for (int i = 0; i < rounds_; ++i) {
      Packet p;
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
      absl::SleepFor(absl::Milliseconds(100));
    }
    return absl::OkStatus();
  }
  absl::Status Cancel() override { return absl::OkStatus(); }

 private:
  int rounds_ = 0;
};
REGISTER_CAPTURE_INTERFACE("TestCapture")
    .OutputPacketType("foo-packet-type")
    .Attr("rounds", "int")
    .Doc("TestCapture documentation");
REGISTER_CAPTURE_IMPLEMENTATION("TestCapture", TestCapture);

class TestFilter : public Filter {
 public:
  TestFilter() {}
  ~TestFilter() override {}
  absl::Status Init(FilterInitContext* ctx) override {
    return absl::OkStatus();
  }
  absl::Status Run(FilterRunContext* ctx) override {
    VAI_ASSIGN_OR_RETURN(auto event_id, ctx->StartEvent());
    while (!is_cancelled_.HasBeenNotified()) {
      Packet p;
      if (absl::IsDeadlineExceeded(ctx->Poll(&p, absl::Milliseconds(100)))) {
        continue;
      }
      VAI_RETURN_IF_ERROR(ctx->Push(event_id, p));
    }
    VAI_RETURN_IF_ERROR(ctx->EndEvent(event_id));
    return absl::OkStatus();
  }
  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  absl::Notification is_cancelled_;
};
REGISTER_FILTER_INTERFACE("TestFilter")
    .InputPacketType("foo-packet-type")
    .OutputPacketType("foo-packet-type")
    .Doc("TestFilter documentation");
REGISTER_FILTER_IMPLEMENTATION("TestFilter", TestFilter);

class TestEventWriter : public EventWriter {
 public:
  TestEventWriter() = default;
  ~TestEventWriter() override {}
  absl::Status Init(EventWriterInitContext* ctx) override {
    VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("magic-number", &magic_number_));
    return absl::OkStatus();
  }

  absl::Status Open(absl::string_view event_id) override {
    return absl::OkStatus();
  }
  absl::Status Write(Packet p) override {
    LOG(INFO) << "pretend to write something " << p.DebugString();
    return absl::OkStatus();
  }
  absl::Status Close() override { return absl::OkStatus(); }

 private:
  int magic_number_ = 0;
};
REGISTER_EVENT_WRITER_INTERFACE("TestEventWriter")
    .InputPacketType("foo-packet-type")
    .Attr("magic-number", "int")
    .Doc("TestEventWriter documentation");
REGISTER_EVENT_WRITER_IMPLEMENTATION("TestEventWriter", TestEventWriter);

TEST(IngesterTest, BasicTest) {
  IngesterConfig config;
  config.mutable_capture_config()->set_name("TestCapture");
  (*config.mutable_capture_config()->mutable_attr())["rounds"] = "2";
  config.mutable_filter_config()->set_name("TestFilter");
  config.mutable_event_writer_config()->set_name("TestEventWriter");
  (*config.mutable_event_writer_config()->mutable_attr())["magic-number"] = "2";
  Ingester ingester(config);
  auto s = ingester.Prepare();
  ASSERT_TRUE(s.ok());
  s = ingester.Run();
  EXPECT_TRUE(s.ok());
}

}  // namespace visionai
