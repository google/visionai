// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/streams/framework/capture_def_registry.h"
#include "visionai/util/telemetry/metrics/stats.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

constexpr absl::Duration kReceivePacketInterval = absl::Seconds(10);
constexpr absl::Duration kPollEventInterval = absl::Seconds(10);
constexpr int kMaxReceiverErrorCount = 20;
constexpr int kLesseeStrLength = 20;
constexpr int kReceiverNameLength = 8;

namespace {

#define METRIC_RECEIVE_EVENT(cluster_selection, stream_id, lessee) \
  capture_received_events_total()                                  \
      .Add({{"project_id", cluster_selection.project_id()},        \
            {"location_id", cluster_selection.location_id()},      \
            {"cluster_id", cluster_selection.cluster_id()},        \
            {"stream_id", stream_id},                              \
            {"lesse", lessee}})                                    \
      .Increment();

#define METRIC_RECEIVE_PACKET_FROM_STREAM(option, packet)            \
  capture_received_packets_from_stream_total()                       \
      .Add({{"project_id", option.cluster_selection.project_id()},   \
            {"location_id", option.cluster_selection.location_id()}, \
            {"cluster_id", option.cluster_selection.cluster_id()},   \
            {"stream_id", option.channel.stream_id},                 \
            {"event_id", option.channel.event_id},                   \
            {"lesse", option.lessee}})                               \
      .Increment();                                                  \
  capture_received_bytes_from_stream_total()                         \
      .Add({{"project_id", option.cluster_selection.project_id()},   \
            {"location_id", option.cluster_selection.location_id()}, \
            {"cluster_id", option.cluster_selection.cluster_id()},   \
            {"stream_id", option.channel.stream_id},                 \
            {"event_id", option.channel.event_id},                   \
            {"lesse", option.lessee}})                               \
      .Increment(packet.ByteSizeLong());

// TODO(annikaz): Add unit tests.
class ReceiverInternal {
 public:
  explicit ReceiverInternal(CaptureRunContext* ctx,
                            PacketReceiver::Options options)
      : ctx_(ctx), options_(options) {}

  ~ReceiverInternal() = default;

  static absl::StatusOr<std::shared_ptr<ReceiverInternal>> Create(
      CaptureRunContext* ctx, PacketReceiver::Options options) {
    auto receiver_internal = std::make_shared<ReceiverInternal>(ctx, options);
    VAI_RETURN_IF_ERROR(receiver_internal->Initialize());
    return receiver_internal;
  }

  absl::Status Initialize() {
    VAI_ASSIGN_OR_RETURN(packet_receiver_, PacketReceiver::Create(options_));
    return absl::OkStatus();
  }

  void Work() {
    Packet p;
    while (!is_cancelled_.HasBeenNotified()) {
      auto s = packet_receiver_->Receive(kReceivePacketInterval, &p);
      if (!s.ok()) {
        LOG(ERROR) << absl::StrFormat(
            "Error from packet receiver (event: %s): %s",
            options_.channel.event_id, s.ToString());

        if (absl::IsNotFound(s) &&
            absl::StrContains(s.message(), kPacketReceiverErrMsgPrefix)) {
          // Continue to poll from the same packet receiver.
          continue;
        }
        if (absl::IsOutOfRange(s)) {
          // Work done.
          return;
        }
        // Reset the packet receiver to retry.
        packet_receiver_.reset();
        auto init_s = Initialize();
        if (!init_s.ok()) {
          LOG(ERROR) << init_s;
          return;
        }
        continue;
      }
      s = ctx_->Push(std::move(p));
      if (!s.ok()) {
        LOG(ERROR) << "Received error while pushing the packet to capture "
                      "context: "
                   << s.ToString();
        break;
      }
    }
  }

  void Cancel() { is_cancelled_.Notify(); }

 private:
  CaptureRunContext* ctx_;
  PacketReceiver::Options options_;
  absl::Notification is_cancelled_;
  std::unique_ptr<PacketReceiver> packet_receiver_ = nullptr;
};
}  // namespace

// StreamingServiceCapture receives packets from Vision AI streaming server.
class StreamingServiceCapture : public Capture {
 public:
  StreamingServiceCapture() {}

  ~StreamingServiceCapture() override {}

  absl::Status Init(CaptureInitContext* ctx) override {
    VAI_RETURN_IF_ERROR(
        ctx->GetAttr<std::string>("stream_id", &options_.channel.stream_id));
    VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("lessee", &options_.lessee));
    if (options_.lessee.empty()) {
      options_.lessee = RandomResourceId(kLesseeStrLength).value();
    }
    VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("k8s_streaming_server_addr",
                                              &streaming_server_addr_));
    VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>(
        "service_endpoint",
        options_.cluster_selection.mutable_service_endpoint()));
    VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>(
        "project_id", options_.cluster_selection.mutable_project_id()));
    VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>(
        "location_id", options_.cluster_selection.mutable_location_id()));
    VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>(
        "cluster_id", options_.cluster_selection.mutable_cluster_id()));
    return absl::OkStatus();
  }

  absl::Status Run(CaptureRunContext* ctx) override {
    VAI_ASSIGN_OR_RETURN(auto event_update_receiver, MakeEventUpdateReceiver());
    std::shared_ptr<ReceiverInternal> receiver_internal = nullptr;
    std::thread t;

    bool read_ok = false;
    while (!is_cancelled_.HasBeenNotified()) {
      LOG_EVERY_T(INFO, 300) << "Polling events";
      EventUpdate event_update;
      if (!event_update_receiver->Receive(kPollEventInterval, &event_update,
                                          &read_ok)) {
        // Timeout, continue to retry.
        continue;
      }
      if (!read_ok) {
        absl::Status s = event_update_receiver->Finish();
        return s;
      }

      auto event_id = GetEventId(event_update);
      if (!event_id.ok()) {
        LOG(ERROR) << absl::StrFormat(
            "Failed to get an event id from a newly arrived `EventUpdate`: %s",
            event_id.status().message());
        break;
      }
      auto stream_id = GetStreamId(event_update);
      if (!stream_id.ok()) {
        LOG(ERROR) << absl::StrFormat(
            "Failed to get an stream id from a newly arrived `EventUpdate`: %s",
            stream_id.status().message());
        break;
      }

      LOG(INFO) << "Received new event: " << *event_id;
      options_.channel.event_id = *event_id;
      if (receiver_internal) {
        receiver_internal->Cancel();
        t.detach();
      }

      if (!streaming_server_addr_.empty()) {
        *options_.cluster_selection.mutable_cluster_endpoint() =
            streaming_server_addr_;
        options_.cluster_selection.set_use_insecure_channel(true);
      }
      VAI_ASSIGN_OR_RETURN(receiver_internal,
                       ReceiverInternal::Create(ctx, options_));
      t = std::thread(&ReceiverInternal::Work, receiver_internal);
    }
    return absl::OkStatus();
  }

  absl::Status Cancel() override {
    is_cancelled_.Notify();
    return absl::OkStatus();
  }

 private:
  PacketReceiver::Options options_;
  absl::Notification is_cancelled_;

  std::string streaming_server_addr_;
  std::string event_discovery_server_addr_;

  absl::StatusOr<std::unique_ptr<EventUpdateReceiver>>
  MakeEventUpdateReceiver() {
    EventUpdateReceiver::Options event_update_receiver_options;
    event_update_receiver_options.cluster_selection =
        options_.cluster_selection;
    event_update_receiver_options.stream_id = options_.channel.stream_id;
    VAI_ASSIGN_OR_RETURN(auto receiver_id, RandomResourceId(kReceiverNameLength));
    event_update_receiver_options.receiver = receiver_id;
    event_update_receiver_options.starting_logical_offset = "most-recent";
    event_update_receiver_options.fallback_starting_offset = "end";
    return EventUpdateReceiver::Create(event_update_receiver_options);
  }
};

// TODO: move the cluster selection attributes to the ingester config.
REGISTER_CAPTURE_INTERFACE("StreamingServiceCapture")
    .Attr("stream_id", "string")
    .Attr("lessee", "string")
    .Attr("k8s_streaming_server_addr", "string")  // TODO(b/235141621)
    .Attr("k8s_event_discovery_server_addr", "string")
    .Attr("service_endpoint", "string")
    .Attr("project_id", "string")
    .Attr("location_id", "string")
    .Attr("cluster_id", "string")
    .Doc(R"doc(
StreamingServiceCapture receives packets from Vision AI streaming server.

Attributes:
  stream_id (string, required):
    the resource id of the stream to receive from.
  lessee (string, required):
    the name under which the resource will be leased.
  streaming_server_address (string, optional):
    k8s DNS address of the streaming server. This field can be specified if the
    client connects to the streaming server in the same k8s cluster. If this
    field is not specified, the client will connect to the public endpoint of
    the cluster, which is determined by the service endpoint, project id,
    location id and cluster id.
  service_endpoint (string, optional):
    Vision AI service endpoint (e.g. visionai.googleapis.com).
  project_id (string, optional): GCP project id.
  location_id (string, optional): GCP location id.
  cluster_id (string, optional): the resource id of the cluster.
)doc");

REGISTER_CAPTURE_IMPLEMENTATION("StreamingServiceCapture",
                                StreamingServiceCapture);

}  // namespace visionai
