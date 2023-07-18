// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/public/streams.h"

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/register_plugins_for_sdk.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/ingester.h"
#include "visionai/util/random_string.h"
#include "visionai/util/status/status_macros.h"
#include "visionai/streams/client/platform_client.h"

namespace visionai {

namespace {

constexpr int kRandomNameLength = 8;
constexpr absl::Duration kDefaultPollPeriod = absl::Milliseconds(100);
constexpr absl::Duration kDefaultPollEventPeriod = absl::Seconds(10);

absl::StatusOr<ClusterSelection> ToClusterSelection(
    const ServiceConnectionOptions& options) {
  ClusterSelection cluster_selection;

  if (options.service_endpoint.empty()) {
    return absl::InvalidArgumentError("Given an empty service endpoint.");
  }
  cluster_selection.set_service_endpoint(options.service_endpoint);

  if (options.project_id.empty()) {
    return absl::InvalidArgumentError("Given an empty project_id.");
  }
  cluster_selection.set_project_id(options.project_id);

  if (options.location_id.empty()) {
    return absl::InvalidArgumentError("Given an empty location_id.");
  }
  cluster_selection.set_location_id(options.location_id);

  if (options.cluster_id.empty()) {
    return absl::InvalidArgumentError("Given an empty cluster_id.");
  }
  cluster_selection.set_cluster_id(options.cluster_id);

  return cluster_selection;
}

}  // namespace

// ----------------------------------------------------------------------------
// Control Methods
// ----------------------------------------------------------------------------

absl::Status CreateStream(const ServiceConnectionOptions& options,
                          absl::string_view stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ToClusterSelection(options));
  return CreateStream(cluster_selection, std::string(stream_id));
}

absl::Status DeleteStream(const ServiceConnectionOptions& options,
                          absl::string_view stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ToClusterSelection(options));
  return DeleteStream(cluster_selection, std::string(stream_id));
}

absl::Status AddStreamToApplication(const ServiceConnectionOptions& options,
                                    absl::string_view stream_id,
                                    absl::string_view application_id) {
  VAI_ASSIGN_OR_RETURN(auto platformClient,
                   PlatformClient::Create(options.service_endpoint));
  auto stream = resource_ids::Stream{
    .project_id = options.project_id,
    .location_id = options.location_id,
    .cluster_id = options.cluster_id,
    .stream_id = std::string(stream_id),
  };
  auto application = resource_ids::Application{
    .project_id = options.project_id,
    .location_id = options.location_id,
    .application_id = std::string(application_id),
  };
  return platformClient->AddStreamToApplication(stream, application);
}

absl::Status RemoveStreamFromApplication(
    const ServiceConnectionOptions& options,
    absl::string_view stream_id,
    absl::string_view application_id) {
  VAI_ASSIGN_OR_RETURN(auto platformClient,
                   PlatformClient::Create(options.service_endpoint));
  auto stream = resource_ids::Stream{
    .project_id = options.project_id,
    .location_id = options.location_id,
    .cluster_id = options.cluster_id,
    .stream_id = std::string(stream_id),
  };
  auto application = resource_ids::Application{
    .project_id = options.project_id,
    .location_id = options.location_id,
    .application_id = std::string(application_id),
  };
  return platformClient->RemoveStreamFromApplication(stream, application);
}

// ----------------------------------------------------------------------------
// Send Methods
// ----------------------------------------------------------------------------

absl::StatusOr<std::unique_ptr<StreamSender>> StreamSender::Create(
    const Options& options) {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection,
                       ToClusterSelection(options.service_connection_options));
  if (options.stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty stream_id.");
  }

  auto event_id = options.event_id.empty() ? NewEventId() : options.event_id;
  VAI_RETURN_IF_ERROR(CreateEventIfNotExist(cluster_selection, event_id));
  VAI_RETURN_IF_ERROR(Bind(cluster_selection, event_id, options.stream_id));

  PacketSender::Options packet_sender_options;
  packet_sender_options.cluster_selection = cluster_selection;
  packet_sender_options.channel.stream_id = options.stream_id;
  packet_sender_options.channel.event_id = event_id;
  if (options.sender_id.empty()) {
    packet_sender_options.sender = RandomString(kRandomNameLength);
  } else {
    packet_sender_options.sender = options.sender_id;
  }
  VAI_ASSIGN_OR_RETURN(auto packet_sender,
                       PacketSender::Create(packet_sender_options));

  auto stream_sender = std::make_unique<StreamSender>();
  stream_sender->packet_sender_ = std::move(packet_sender);

  // std::move needed to be compatible with OSS.
  return std::move(stream_sender);
}

absl::Status StreamSender::Send(Packet packet) {
  return packet_sender_->Send(std::move(packet));
}

absl::Status StreamSender::Send(Packet packet, absl::Duration timeout) {
  return packet_sender_->Send(std::move(packet), timeout);
}

// ----------------------------------------------------------------------------
// Receiving Methods
// ----------------------------------------------------------------------------

absl::StatusOr<std::unique_ptr<StreamReceiver>> StreamReceiver::Create(
    const Options& options) {
  auto stream_receiver = std::make_unique<StreamReceiver>();

  VAI_ASSIGN_OR_RETURN(stream_receiver->cluster_selection_,
                       ToClusterSelection(options.service_connection_options));

  if (options.stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty stream_id.");
  }
  stream_receiver->stream_id_ = options.stream_id;

  auto receiver_id = options.receiver_id.empty()
                         ? RandomString(kRandomNameLength)
                         : options.receiver_id;
  stream_receiver->receiver_id_ = receiver_id;

  if (options.event_id.empty()) {
    EventUpdateReceiver::Options event_update_receiver_options;
    event_update_receiver_options.cluster_selection =
        stream_receiver->cluster_selection_;
    event_update_receiver_options.stream_id = options.stream_id;
    event_update_receiver_options.receiver = receiver_id;
    event_update_receiver_options.starting_logical_offset = "most-recent";
    event_update_receiver_options.fallback_starting_offset = "end";
    VAI_ASSIGN_OR_RETURN(
        auto event_update_receiver,
        EventUpdateReceiver::Create(event_update_receiver_options));
    stream_receiver->event_update_receiver_ = std::move(event_update_receiver);
  } else {
    stream_receiver->event_id_ = options.event_id;
  }

  // std::move needed to be compatible with OSS.
  return std::move(stream_receiver);
}

absl::Status StreamReceiver::GetFirstEvent(absl::Duration timeout) {
  if (event_update_receiver_ == nullptr) {
    return absl::InternalError(
        "This function can only be called when the EventUpdateReceiver is "
        "initialized.");
  }
  if (!event_id_.empty()) {
    return absl::InternalError(
        "This function can only be called when no event_id is present.");
  }
  EventUpdate event_update;
  bool read_ok = false;
  auto success =
      event_update_receiver_->Receive(timeout, &event_update, &read_ok);
  if (!success) {
    return absl::NotFoundError("No event has arrived yet.");
  }
  if (!read_ok) {
    return event_update_receiver_->Finish();
  }
  event_id_ = event_update.event();
  event_update_receiver_->Cancel();
  auto cancel_status = event_update_receiver_->Finish();
  if (!absl::IsCancelled(cancel_status)) {
    if (cancel_status.ok()) {
      VAI_RETURN_IF_ERROR(
          absl::UnknownError("Expected a CANCELLED error from the "
                             "`EventUpdateReceiver`, but got OK"));
    } else {
      VAI_RETURN_IF_ERROR(cancel_status)
          << "which is unexpected after having successfully getting "
             "an event from the `EventUpdateReceiver`";
    }
  }
  return absl::OkStatus();
}

absl::Status StreamReceiver::Receive(Packet* packet) {
  return Receive(absl::InfiniteDuration(), packet);
}

absl::Status StreamReceiver::Receive(absl::Duration timeout, Packet* packet) {
  if (event_id_.empty()) {
    VAI_RETURN_IF_ERROR(GetFirstEvent(timeout));
  }

  if (packet_receiver_ == nullptr) {
    PacketReceiver::Options packet_receiver_options;
    packet_receiver_options.cluster_selection = cluster_selection_;
    packet_receiver_options.channel.stream_id = stream_id_;
    packet_receiver_options.channel.event_id = event_id_;
    packet_receiver_options.lessee = receiver_id_;
    VAI_ASSIGN_OR_RETURN(packet_receiver_,
                         PacketReceiver::Create(packet_receiver_options));
  }
  return packet_receiver_->Receive(timeout, packet);
}

// ----------------------------------------------------------------------------
// Ingest Methods
// ----------------------------------------------------------------------------

namespace {

absl::Status RunIngester(const IngesterConfig& config) {
  VAI_RETURN_IF_ERROR(RegisterGstPluginsForSDK());
  Ingester ingest(config);
  VAI_RETURN_IF_ERROR(ingest.Prepare());
  VAI_RETURN_IF_ERROR(ingest.Run());
  return absl::OkStatus();
}

}  // namespace

absl::Status IngestMp4(const ServiceConnectionOptions& options,
                       absl::string_view stream_id,
                       absl::string_view file_name) {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ToClusterSelection(options));

  IngesterConfig config;
  CaptureConfig* capture_config = config.mutable_capture_config();
  capture_config->set_name("FileSourceCapture");
  capture_config->add_source_urls(std::string(file_name));
  FilterConfig* filter_config = config.mutable_filter_config();
  filter_config->set_name("NoopFilter");
  EventWriterConfig* event_writer_config = config.mutable_event_writer_config();
  event_writer_config->set_name("StreamsEventWriter");
  *event_writer_config->mutable_cluster_selection() = cluster_selection;
  (*event_writer_config->mutable_attr())["stream_id"] = std::string(stream_id);
  VAI_RETURN_IF_ERROR(RunIngester(config));
  return absl::OkStatus();
}

absl::Status IngestRtsp(const ServiceConnectionOptions& options,
                        absl::string_view stream_id,
                        absl::string_view rtsp_url) {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ToClusterSelection(options));

  IngesterConfig config;
  CaptureConfig* capture_config = config.mutable_capture_config();
  capture_config->set_name("RTSPCapture");
  capture_config->add_source_urls(std::string(rtsp_url));
  FilterConfig* filter_config = config.mutable_filter_config();
  filter_config->set_name("NoopFilter");
  EventWriterConfig* event_writer_config = config.mutable_event_writer_config();
  event_writer_config->set_name("StreamsEventWriter");
  *event_writer_config->mutable_cluster_selection() = cluster_selection;
  (*event_writer_config->mutable_attr())["stream_id"] = std::string(stream_id);
  VAI_RETURN_IF_ERROR(RunIngester(config));
  return absl::OkStatus();
}

absl::Status IngestMotion(const ServiceConnectionOptions& options,
                          absl::string_view stream_id,
                          absl::string_view event_id,
                          absl::string_view file_name,
                          const MotionFilterOptions& motion_options) {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ToClusterSelection(options));
  IngesterConfig config;

  CaptureConfig* capture_config = config.mutable_capture_config();
  capture_config->set_name("FileSourceCapture");
  capture_config->add_source_urls(std::string(file_name));
  capture_config->mutable_attr()->insert({"loop", "true"});
  FilterConfig* filter_config = config.mutable_filter_config();

  filter_config->set_name("EncodedMotionFilter");

  filter_config->mutable_attr()->insert({"min_event_length_in_seconds",
                                  motion_options.min_event_length});
  filter_config->mutable_attr()->insert({"lookback_window_in_seconds",
                                  motion_options.lookback_window});
  filter_config->mutable_attr()->insert({"cool_down_period_in_seconds",
                                  motion_options.cool_down_period});
  filter_config->mutable_attr()->insert({"motion_detection_sensitivity",
                                  motion_options.motion_detection_sensitivity});

  EventWriterConfig* event_writer_config = config.mutable_event_writer_config();
  event_writer_config->set_name("StreamsEventWriter");
  *event_writer_config->mutable_cluster_selection() = cluster_selection;
  (*event_writer_config->mutable_attr())["stream_id"] = std::string(stream_id);

  config.mutable_ingest_policy()->set_event(event_id);
  config.mutable_ingest_policy()->set_continuous_mode(true);

  VAI_RETURN_IF_ERROR(RunIngester(config));

  return absl::OkStatus();
}

}  // namespace visionai
