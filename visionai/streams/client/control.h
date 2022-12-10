// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// control.h
// -----------------------------------------------------------------------------
//
// This header defines methods to elementary methods used to issue control
// directives and operations to the Anaheim Streams Service.
//
// It abstracts Anaheim Streams into a system with relatively few primitives.
// All network call details and protocol conventions are handled in the
// implementation, and the user may think of this header as the API surface of
// all things control related.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CONTROL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CONTROL_H_

#include <string>

#include "google/cloud/visionai/v1/common.pb.h"
#include "google/cloud/visionai/v1/streams_resources.pb.h"
#include "google/longrunning/operations.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"

namespace visionai {

// ----------------------------------------------------------------------------
// Methods for managing Streams, Events, and Channels.
// ----------------------------------------------------------------------------

// Create a stream with the given stream-id.
absl::Status CreateStream(const ClusterSelection& selection,
                          const std::string& stream_id);
// Get a stream.
//
// Returns information about a specific stream.
absl::StatusOr<google::cloud::visionai::v1::Stream> GetStream(
    const ClusterSelection& selection, const std::string& stream_id);

// List streams.
//
// Returns information about all streams.
absl::StatusOr<std::vector<google::cloud::visionai::v1::Stream>> ListStreams(
    const ClusterSelection& selection);

// Delete a stream with the given stream-id.
absl::Status DeleteStream(const ClusterSelection& selection,
                          const std::string& stream_id);

// Check if a stream with the given stream-id exists.
absl::Status CheckStreamExists(const ClusterSelection& selection,
                               const std::string& stream_id);

// Create an event with the given event-id.
absl::Status CreateEvent(const ClusterSelection& selection,
                         const std::string& event_id);

// Create an event with the given event-id if it does not already exist.
absl::Status CreateEventIfNotExist(const ClusterSelection& selection,
                                   const std::string& event_id);

// Get an event.
//
// Returns information about a specific event.
absl::StatusOr<google::cloud::visionai::v1::Event> GetEvent(
    const ClusterSelection& selection, const std::string& event_id);

// List events.
//
// Returns information about all events.
absl::StatusOr<std::vector<google::cloud::visionai::v1::Event>> ListEvents(
    const ClusterSelection& selection);

// Bind the given stream to the given event.
//
// If it has already been bound, then it simply returns success.
absl::Status Bind(const ClusterSelection& selection,
                  const std::string& event_id, const std::string& stream_id);

// Streams feature enablement.
//
// Enable HLS playback.
absl::Status EnableHlsPlayback(const ClusterSelection& selection,
                               const std::string& stream_id);

// Disable HLS playback.
absl::Status DisableHlsPlayback(const ClusterSelection& selection,
                                const std::string& stream_id);

// Enable the media warehouse exporter.
absl::Status EnableMwhExporter(const ClusterSelection& selection,
                               const std::string& stream_id,
                               const std::string& mwh_asset_name);

// Disable the media warehouse exporter.
absl::Status DisableMwhExporter(const ClusterSelection& selection,
                                const std::string& stream_id);

// ----------------------------------------------------------------------------
// Methods for managing leases.
// ----------------------------------------------------------------------------

// Acquire a lease on the channel in the cluster specified by
// `cluster_selection`. Use `options` to specify the channel and lease terms.
//
// On success, the descriptor of the acquired lease will be returned.
// This descriptor must be presented for further operations on the lease.
struct LeaseOptions {
  // The name of the lessee.
  std::string lessee;

  // The duration that the lease is active without additional renewels.
  absl::Duration duration;

  // The channel for which a lease is sought.
  Channel channel;

  // The type of lease to acquire.
  ChannelLeaseType lease_type;
};
absl::StatusOr<ChannelLease> AcquireChannelLease(
    const ClusterSelection& selection, const LeaseOptions& options);

// Renew the given channel lease.
//
// The new lease will have the same duration as the original, but the
// count down will be reset.
//
// On success, `lease` will be changed to reflect the new lease terms.
// Otherwise, `lease` will remain unchanged.
absl::Status RenewLease(const ClusterSelection& selection, ChannelLease* lease);

// Release the presented channel lease.
//
// On success, the caller would have successfully relinquished its share on the
// channel, and `lease` will have a zero duration. Further operations must
// proceed through a new lease.
// Otherwise, `lease` will remain unchanged.
absl::Status ReleaseLease(const ClusterSelection& selection,
                          ChannelLease* lease);

// ----------------------------------------------------------------------------
// Methods for managing Clusters
// ----------------------------------------------------------------------------

// Create a cluster (asynchronously).
//
// This creates the cluster asynchronously and returns an LRO.
// The caller can use the LRO to check on its status later.
absl::StatusOr<google::longrunning::Operation> CreateClusterAsync(
    const ClusterSelection& selection, const std::string& cluster_id);

// List clusters.
//
// Returns information about all clusters.
absl::StatusOr<std::vector<google::cloud::visionai::v1::Cluster>> ListClusters(
    const ClusterSelection& selection);

// Get a cluster.
//
// Returns information about a specific cluster.
absl::StatusOr<google::cloud::visionai::v1::Cluster> GetCluster(
    const ClusterSelection& selection, const std::string& cluster_id);

// Delete a cluster (asynchronously).
absl::StatusOr<google::longrunning::Operation> DeleteClusterAsync(
    const ClusterSelection& selection, const std::string& cluster_id);

// Check that the given ClusterSelection is in its proper format.
absl::Status ValidateClusterSelection(const ClusterSelection& selection);

// Get the ingress endpoint to the given selection-id.
absl::StatusOr<std::string> GetClusterEndpoint(
    const ClusterSelection& selection);

// Get the cluster name from the cluster selection.
absl::StatusOr<std::string> ClusterNameFrom(const ClusterSelection& selection);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CONTROL_H_
