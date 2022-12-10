// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMS_CONTROL_GRPC_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMS_CONTROL_GRPC_CLIENT_H_

#include "google/cloud/visionai/v1/streams_service.grpc.pb.h"
#include "absl/status/statusor.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"

namespace visionai {

// The StreamsControlGrpcClient is the bare grpc client to the streams service.
//
// It deals mostly with elementary grpc and AIP's API conventions while
// deferring service specific conventions to the caller; e.g. the resource
// name construction (resource hierarchy understanding) and any other more
// service specific business logic.
class StreamsControlGrpcClient {
 public:
  // Options for specifying the connection to the Streams service.
  struct Options {
    // The endpoint of streams service.
    //
    // This may be a DNS name or a (ip:port).
    std::string target_address;

    // Options to configure the RPC connection.
    ConnectionOptions connection_options;
  };

  // Create a ready to use client.
  //
  // Use this to get new instances rather than using constructors.
  static absl::StatusOr<std::unique_ptr<StreamsControlGrpcClient>> Create(
      const Options &options);

  virtual ~StreamsControlGrpcClient() = default;

  // --------------------------------------------------------------------------
  // RPC wrappers for controlling Streams
  // --------------------------------------------------------------------------

  // Get the information about a Stream.
  absl::StatusOr<google::cloud::visionai::v1::Stream> GetStream(
      const std::string &parent, const std::string &stream_id);

  // Get the information about Streams meeting a filter criteria.
  //
  // Setting `filter` to empty performs no filtering.
  absl::StatusOr<std::vector<google::cloud::visionai::v1::Stream>> ListStreams(
      const std::string &parent, const std::string &filter);

  // Create a Stream.
  absl::StatusOr<google::longrunning::Operation> CreateStream(
      const std::string &parent, const std::string &stream_id);

  // Update a Stream.
  //
  // The updated stream will be those given in `stream` with the specified
  // `field_mask` applied.
  absl::StatusOr<google::longrunning::Operation> UpdateStream(
      const std::string &parent,
      const google::cloud::visionai::v1::Stream &stream,
      const google::protobuf::FieldMask &field_mask);
  absl::StatusOr<google::longrunning::Operation> DeleteStream(
      const std::string &parent, const std::string &stream_id);

  // --------------------------------------------------------------------------
  // RPC wrappers for controlling Events
  // --------------------------------------------------------------------------

  // Get the information about a Event.
  absl::StatusOr<google::cloud::visionai::v1::Event> GetEvent(
      const std::string &parent, const std::string &event_id);

  // Get the information about Events meeting a filter criteria.
  //
  // Setting `filter` to empty performs no filtering.
  absl::StatusOr<std::vector<google::cloud::visionai::v1::Event>> ListEvents(
      const std::string &parent, const std::string &filter);

  // Create an Event.
  absl::StatusOr<google::longrunning::Operation> CreateEvent(
      const std::string &parent, const std::string &event_id);

  // Delete an Event.
  absl::StatusOr<google::longrunning::Operation> DeleteEvent(
      const std::string &parent, const std::string &event_id);

  // --------------------------------------------------------------------------
  // RPC wrappers for controlling Series
  // --------------------------------------------------------------------------
  //
  // TODO: Change these to Channels.

  // Get the information about a Series.
  absl::StatusOr<google::cloud::visionai::v1::Series> GetSeries(
      const std::string &parent, const std::string &series_id);

  // Get the information about Series meeting a filter criteria.
  //
  // Setting `filter` to empty performs no filtering.
  absl::StatusOr<std::vector<google::cloud::visionai::v1::Series>> ListSeries(
      const std::string &parent, const std::string &filter);

  // Create a Series.
  absl::StatusOr<google::longrunning::Operation> CreateSeries(
      const std::string &parent, const std::string &series_id,
      const std::string &stream_id, const std::string &event_id);

  // Delete a Series.
  absl::StatusOr<google::longrunning::Operation> DeleteSeries(
      const std::string &parent, const std::string &series_id);

  // --------------------------------------------------------------------------
  // RPC wrappers for controlling Clusters
  // --------------------------------------------------------------------------

  // Create a Cluster.
  absl::StatusOr<google::longrunning::Operation> CreateCluster(
      const std::string &parent, const std::string &cluster_id);

  // Get the information about a Cluster.
  absl::StatusOr<google::cloud::visionai::v1::Cluster> GetCluster(
      const std::string &parent, const std::string &cluster_id);

  // Get the information about Clusters meeting a filter criteria.
  //
  // Setting `filter` to empty performs no filtering.
  absl::StatusOr<std::vector<google::cloud::visionai::v1::Cluster>>
  ListClusters(const std::string &parent, const std::string &filter);

  // Delete a Cluster.
  absl::StatusOr<google::longrunning::Operation> DeleteCluster(
      const std::string &parent, const std::string &cluster_id);

  // --------------------------------------------------------------------------
  // RPC wrappers for LRO handling.
  // --------------------------------------------------------------------------

  // Wait for the given LRO to complete.
  absl::StatusOr<google::protobuf::Any> Wait(
      const std::string &parent,
      const google::longrunning::Operation &operation);

  // --------------------------------------------------------------------------
  // StreamsControlGrpcClient is neither copyable nor movable.
  StreamsControlGrpcClient(const StreamsControlGrpcClient &) = delete;
  StreamsControlGrpcClient &operator=(const StreamsControlGrpcClient &) =
      delete;

 private:
  StreamsControlGrpcClient() = default;
  Options options_;
  std::unique_ptr<google::cloud::visionai::v1::StreamsService::Stub>
      streams_stub_;
};
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMS_CONTROL_GRPC_CLIENT_H_
