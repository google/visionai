// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/control.h"

#include <algorithm>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streams_resources.pb.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/client/streaming_control_grpc_client.h"
#include "visionai/streams/client/streams_control_grpc_client.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {
absl::StatusOr<std::unique_ptr<StreamsControlGrpcClient>>
CreateStreamsControlGrpcClient(const ClusterSelection& selection) {
  StreamsControlGrpcClient::Options options;
  options.target_address = selection.service_endpoint();
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      selection.use_insecure_channel());
  return StreamsControlGrpcClient::Create(options);
}

absl::StatusOr<std::unique_ptr<StreamingControlGrpcClient>>
CreateStreamingControlGrpcClient(const ClusterSelection& selection) {
  VAI_ASSIGN_OR_RETURN(const auto endpoint, GetClusterEndpoint(selection),
                   _ << "while getting the cluster endpoint");
  StreamingControlGrpcClient::Options options;
  options.target_address = endpoint;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      selection.use_insecure_channel());
  return StreamingControlGrpcClient::Create(options);
}

absl::StatusOr<std::string> ProjectLocationNameFrom(
    const ClusterSelection& selection) {
  VAI_RETURN_IF_ERROR(ValidateClusterSelection(selection));
  VAI_ASSIGN_OR_RETURN(
      auto project_location_name,
      MakeProjectLocationName(selection.project_id(), selection.location_id()));
  return project_location_name;
}

absl::Status UpdateHlsPlayback(const ClusterSelection& selection,
                               const std::string& stream_id, bool enable) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto stream_name, MakeStreamName(selection, stream_id),
                   _ << "while deducing the stream name");
  google::cloud::visionai::v1::Stream stream;
  stream.set_name(stream_name);
  stream.set_enable_hls_playback(enable);
  google::protobuf::FieldMask mask;
  mask.add_paths("enable_hls_playback");
  VAI_ASSIGN_OR_RETURN(auto operation,
                   client->UpdateStream(cluster_name, stream, mask));
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

absl::Status UpdateMwhExporter(const ClusterSelection& selection,
                               const std::string& stream_id,
                               const std::string& mwh_asset_name) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto stream_name, MakeStreamName(selection, stream_id),
                   _ << "while deducing the stream name");
  google::cloud::visionai::v1::Stream stream;
  stream.set_name(stream_name);
  stream.set_media_warehouse_asset(mwh_asset_name);
  google::protobuf::FieldMask mask;
  mask.add_paths("media_warehouse_asset");
  VAI_ASSIGN_OR_RETURN(auto operation,
                   client->UpdateStream(cluster_name, stream, mask));
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

}  // namespace

// ----------------------------------------------------------------------------
// (Implementation) Methods for managing Streams, Events, and Channels.
// ----------------------------------------------------------------------------

absl::Status CreateStream(const ClusterSelection& selection,
                          const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto operation,
                   client->CreateStream(cluster_name, stream_id),
                   _ << "while creating a Stream");
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

absl::StatusOr<std::vector<google::cloud::visionai::v1::Stream>>
ListStreams(const ClusterSelection& selection) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto streams, client->ListStreams(cluster_name, ""),
                   _ << "while listing streams");
  return streams;
}

absl::StatusOr<google::cloud::visionai::v1::Stream> GetStream(
    const ClusterSelection& selection, const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  if (stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty stream id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto stream, client->GetStream(cluster_name, stream_id),
                   _ << "while getting a stream");
  return stream;
}

absl::Status DeleteStream(const ClusterSelection& selection,
                          const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto operation,
                   client->DeleteStream(cluster_name, stream_id),
                   _ << "while deleting a Stream");
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

absl::Status CheckStreamExists(const ClusterSelection& selection,
                               const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name,ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  auto stream_statusor = client->GetStream(cluster_name, stream_id);
  return stream_statusor.status();
}

absl::Status CreateEvent(const ClusterSelection& selection,
                         const std::string& event_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto operation, client->CreateEvent(cluster_name, event_id),
                   _ << "while creating an Event");
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

// TODO: Issuing two separate requests doesn't actually solve the problem since
// another client could act between calls. Only AlreadyExists can ensure
// atomicity.
absl::Status CreateEventIfNotExist(const ClusterSelection& selection,
                                   const std::string& event_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  auto event = client->GetEvent(cluster_name, event_id);
  if (event.ok()) {
    return absl::OkStatus();
  }
  VAI_ASSIGN_OR_RETURN(auto operation, client->CreateEvent(cluster_name, event_id),
                   _ << "while creating an Event");
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

absl::StatusOr<std::vector<google::cloud::visionai::v1::Event>>
ListEvents(const ClusterSelection& selection) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto events, client->ListEvents(cluster_name, ""),
                   _ << "while listing events");
  return events;
}

absl::StatusOr<google::cloud::visionai::v1::Event> GetEvent(
    const ClusterSelection& selection, const std::string& event_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  if (event_id.empty()) {
    return absl::InvalidArgumentError("Given an empty event id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto event, client->GetEvent(cluster_name, event_id),
                   _ << "while getting an event");
  return event;
}

// TODO: Issuing two separate requests doesn't actually solve the problem since
// another client could act between calls. Only AlreadyExists can ensure
// atomicity.
absl::Status Bind(const ClusterSelection& selection,
                  const std::string& event_id, const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, ClusterNameFrom(selection),
                   _ << "while deducing the name of the selected cluster");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto channel_id, MakeChannelId(event_id, stream_id),
                   _ << "while deducing the channel id");
  auto channel = client->GetSeries(cluster_name, channel_id);
  if (channel.ok()) {
    return absl::OkStatus();
  }
  VAI_ASSIGN_OR_RETURN(
      auto operation,
      client->CreateSeries(cluster_name, channel_id, stream_id, event_id),
      _ << "while creating a Series");
  auto wait_result_statusor = client->Wait(cluster_name, operation);
  return wait_result_statusor.status();
}

// Enable HLS playback.
absl::Status EnableHlsPlayback(const ClusterSelection& selection,
                               const std::string& stream_id) {
  return UpdateHlsPlayback(selection, stream_id, true);
}

// Disable HLS playback.
absl::Status DisableHlsPlayback(const ClusterSelection& selection,
                                const std::string& stream_id) {
  return UpdateHlsPlayback(selection, stream_id, false);
}

// Enable the media warehouse exporter.
absl::Status EnableMwhExporter(const ClusterSelection& selection,
                               const std::string& stream_id,
                               const std::string& mwh_asset_name) {
  return UpdateMwhExporter(selection, stream_id, mwh_asset_name);
}

// Disable the media warehouse exporter.
absl::Status DisableMwhExporter(const ClusterSelection& selection,
                                const std::string& stream_id) {
  return UpdateMwhExporter(selection, stream_id, "");
}

// ----------------------------------------------------------------------------
// (Implementation) Methods for managing leases.
// ----------------------------------------------------------------------------

absl::StatusOr<ChannelLease> AcquireChannelLease(
    const ClusterSelection& selection, const LeaseOptions& options) {
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamingControlGrpcClient(selection),
                   _ << "while creating the StreamingControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(
      auto channel_id,
      MakeChannelId(options.channel.event_id, options.channel.stream_id),
      _ << "while deducing the channel id");
  VAI_ASSIGN_OR_RETURN(auto channel_name, MakeChannelName(selection, channel_id),
                   _.LogError() << "while deducing the channel name");
  auto status_or_lease =
      options.lease_type == ChannelLeaseType::kWriters
          ? client->AcquireWritersLease(channel_name, options.lessee,
                                        options.duration)
          : client->AcquireReadersLease(channel_name, options.lessee,
                                        options.duration);
  if (!status_or_lease.ok()) {
    return status_or_lease.status();
  }
  ChannelLease lease;
  lease.id = status_or_lease->id();
  lease.channel.event_id = options.channel.event_id;
  lease.channel.stream_id = options.channel.stream_id;
  lease.duration = options.duration;
  lease.lessee = options.lessee;
  lease.acquired_time = absl::Now();
  lease.lease_type = options.lease_type;
  return lease;
}

absl::Status RenewLease(const ClusterSelection& selection,
                        ChannelLease* lease) {
  if (lease->id.empty()) {
    return absl::InvalidArgumentError("Given an empty id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamingControlGrpcClient(selection),
                   _ << "while creating the StreamingControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(
      auto channel_id,
      MakeChannelId(lease->channel.event_id, lease->channel.stream_id),
      _ << "while deducing the channel id");
  VAI_ASSIGN_OR_RETURN(auto channel_name, MakeChannelName(selection, channel_id),
                   _ << "while deducing the channel name");
  VAI_ASSIGN_OR_RETURN(auto resp,
                   client->RenewLease(lease->id, channel_name, lease->lessee,
                                      lease->duration));
  lease->acquired_time = absl::Now();
  return absl::OkStatus();
}

absl::Status ReleaseLease(const ClusterSelection& selection,
                          ChannelLease* lease) {
  if (lease == nullptr) {
    return absl::InvalidArgumentError("Given an nullptr to a lease.");
  }
  if (lease->id.empty()) {
    return absl::InvalidArgumentError("Given an empty id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamingControlGrpcClient(selection),
                   _ << "while creating the StreamingControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(
      auto channel_id,
      MakeChannelId(lease->channel.event_id, lease->channel.stream_id),
      _ << "while deducing the channel id");
  VAI_ASSIGN_OR_RETURN(auto channel_name, MakeChannelName(selection, channel_id),
                   _.LogError() << "while deducing the channel name");
  VAI_RETURN_IF_ERROR(
      client->ReleaseLease(lease->id, channel_name, lease->lessee).status())
      << "while making the release RPC";
  lease->acquired_time = absl::Now();
  lease->duration = absl::ZeroDuration();
  return absl::OkStatus();
}

// ----------------------------------------------------------------------------
// (Implementation) Methods for managing Clusters
// ----------------------------------------------------------------------------

absl::Status ValidateClusterSelection(const ClusterSelection& selection) {
  if (selection.cluster_endpoint().empty()) {
    if (selection.service_endpoint().empty()) {
      return absl::InvalidArgumentError("Given an empty service endpoint.");
    }
    if (selection.project_id().empty()) {
      return absl::InvalidArgumentError("Given an empty project_id.");
    }
    if (selection.location_id().empty()) {
      return absl::InvalidArgumentError("Given an empty location_id.");
    }
  }
  return absl::OkStatus();
}

absl::StatusOr<google::longrunning::Operation> CreateClusterAsync(
    const ClusterSelection& selection, const std::string& cluster_id) {
  VAI_ASSIGN_OR_RETURN(auto project_location_name,
                   ProjectLocationNameFrom(selection),
                   _ << "while deducing the project-location name");
  if (cluster_id.empty()) {
    return absl::InvalidArgumentError("Given an empty cluster id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto operation,
                   client->CreateCluster(project_location_name, cluster_id),
                   _ << "while creating a Cluster");
  return operation;
}

absl::StatusOr<std::vector<google::cloud::visionai::v1::Cluster>>
ListClusters(const ClusterSelection& selection) {
  VAI_ASSIGN_OR_RETURN(auto project_location_name,
                   ProjectLocationNameFrom(selection),
                   _ << "while deducing the project-location name");
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto clusters,
                   client->ListClusters(project_location_name, ""),
                   _ << "while listing clusters");

  return clusters;
}

absl::StatusOr<google::cloud::visionai::v1::Cluster> GetCluster(
    const ClusterSelection& selection, const std::string& cluster_id) {
  VAI_ASSIGN_OR_RETURN(auto project_location_name,
                   ProjectLocationNameFrom(selection),
                   _ << "while deducing the project-location name");
  if (cluster_id.empty()) {
    return absl::InvalidArgumentError("Given an empty cluster id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto cluster,
                   client->GetCluster(project_location_name, cluster_id),
                   _ << "while listing clusters");
  return cluster;
}

absl::StatusOr<google::longrunning::Operation> DeleteClusterAsync(
    const ClusterSelection& selection, const std::string& cluster_id) {
  VAI_ASSIGN_OR_RETURN(auto project_location_name,
                   ProjectLocationNameFrom(selection),
                   _ << "while deducing the project-location name");
  if (cluster_id.empty()) {
    return absl::InvalidArgumentError("Given an empty cluster id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(auto operation,
                   client->DeleteCluster(project_location_name, cluster_id),
                   _ << "while deleting a Cluster");
  return operation;
}

absl::StatusOr<std::string> GetClusterEndpoint(
    const ClusterSelection& selection) {
  if (!selection.cluster_endpoint().empty()) {
    return selection.cluster_endpoint();
  }
  VAI_ASSIGN_OR_RETURN(auto project_location_name,
                   ProjectLocationNameFrom(selection),
                   _ << "while deducing the project-location name");
  if (selection.cluster_id().empty()) {
    return absl::InvalidArgumentError("Given an empty cluster_id.");
  }
  VAI_ASSIGN_OR_RETURN(auto client, CreateStreamsControlGrpcClient(selection),
                   _ << "while creating the StreamsControlGrpcClient");
  VAI_ASSIGN_OR_RETURN(
      auto cluster,
      client->GetCluster(project_location_name, selection.cluster_id()),
      _ << "while getting the Cluster resource");
  return cluster.dataplane_service_endpoint();
}

absl::StatusOr<std::string> ClusterNameFrom(const ClusterSelection& selection) {
  VAI_RETURN_IF_ERROR(ValidateClusterSelection(selection));
  if (selection.cluster_id().empty()) {
    return absl::InvalidArgumentError("Given an empty cluster_id.");
  }
  VAI_ASSIGN_OR_RETURN(auto cluster_name, MakeClusterName(selection.project_id(),
                                                      selection.location_id(),
                                                      selection.cluster_id()));
  return cluster_name;
}

}  // namespace visionai
