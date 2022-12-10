// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// resource_util.h
// -----------------------------------------------------------------------------
//
// This header defines methods that unify resource manipulation throughout C++
// code. Use these methods to parse/generate resource names and ids. If a
// method that you need isn't present, please add to this file rather than
// proliferating one-offs.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_RESOURCE_UTIL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_RESOURCE_UTIL_H_

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "visionai/proto/cluster_selection.pb.h"

namespace visionai {

constexpr char kProjectsCollectionId[] = "projects";
constexpr char kLocationsCollectionId[] = "locations";
constexpr char kClustersCollectionId[] = "clusters";
constexpr char kEventsCollectionId[] = "events";
constexpr char kStreamsCollectionId[] = "streams";
constexpr char kChannelsCollectionId[] = "series";

// ResourceInfoMap is the type mapping from the collection id to the resource
// id.
using ResourceInfoMap = absl::flat_hash_map<std::string, std::string>;

// ----------------------------------------------------------------------------
// Methods for standardizing AIS resource names
// ----------------------------------------------------------------------------
//
// For resource name assembly, there are typically two forms:
// 1. One overload that directly accepts individual resource-id segments.
// 2. One overload that accepts a ClusterSelection and the final resource-id.
//
// For the form that accepts a ClusterSelection, it will deduce the intermediate
// resource-ids.

// Return the resource name of the project-location.
absl::StatusOr<std::string> MakeProjectLocationName(
    const std::string& project_id, const std::string& location_id);
absl::StatusOr<std::string> MakeProjectLocationName(
    const ClusterSelection& selection);

// Return the resource name of the cluster with the given resource-id segments.
absl::StatusOr<std::string> MakeClusterName(const std::string& project_id,
                                            const std::string& location_id,
                                            const std::string& cluster_id);
absl::StatusOr<std::string> MakeClusterName(const ClusterSelection& selection);

// Return the resource name of the event given a cluster name and event-id.
absl::StatusOr<std::string> MakeEventName(const std::string& cluster_name,
                                          const std::string& event_id);
absl::StatusOr<std::string> MakeEventName(const ClusterSelection& selection,
                                          const std::string& event_id);

// Return the resource name of the stream given a cluster name and stream-id.
absl::StatusOr<std::string> MakeStreamName(const std::string& cluster_name,
                                           const std::string& stream_id);
absl::StatusOr<std::string> MakeStreamName(const ClusterSelection& selection,
                                           const std::string& stream_id);

// Return the resource name of the channel given a cluster name and channel-id.
absl::StatusOr<std::string> MakeChannelName(const std::string& cluster_name,
                                            const std::string& channel_id);
absl::StatusOr<std::string> MakeChannelName(const ClusterSelection& selection,
                                            const std::string& channel_id);

// Given a cluster name, return a vector containing its resource-ids, ordered by
// their rank.
//
// Example: Given projects/p-1/locations/l-1/clusters/c-1,
// returns the vector { p-1, l-1, c-1 }.
absl::StatusOr<std::vector<std::string>> ParseClusterName(
    const std::string& cluster_name);

// Given a steram name, return a vector containing its resource-ids, ordered by
// their rank.
//
// Example: Given projects/p-1/locations/l-1/clusters/c-1/streams/s-1
// returns the vector { p-1, l-1, c-1, s-1 }.
absl::StatusOr<std::vector<std::string>> ParseStreamName(
    const std::string& stream_name);

// Generate a unique event id.
//
// TODO: This should be assigned by the server.
//       It isn't unique at the moment.
std::string NewEventId();

// Generate the canonical name for a channel bound to an (event_id, stream_id).
absl::StatusOr<std::string> MakeChannelId(const std::string& event_id,
                                          const std::string& stream_id);

// Extract the event-id from the given channel-id.
absl::StatusOr<std::string> EventIdFromChannelId(const std::string& channel_id);

// Extract the stream-id from the given channel-id.
absl::StatusOr<std::string> StreamIdFromChannelId(
    const std::string& channel_id);

// ----------------------------------------------------------------------------
// General helper methods.
// ----------------------------------------------------------------------------

// Given the resource name of an event, stream, or channel, return the
// cluster resource name it belongs to.
absl::StatusOr<std::string> GetClusterName(const std::string&);

// Generate a random resource id.
absl::StatusOr<std::string> RandomResourceId(size_t length);

// Given two resource names, specifying either streams or events, return whether
// they belong to the same cluster.
absl::StatusOr<bool> InSameCluster(const std::string&, const std::string&);

// Given a resource name, return the name of its parent resource.
absl::StatusOr<std::string> ResourceParentName(
    const std::string& resource_name);

// Given a resource name, return the resource id.
absl::StatusOr<std::string> ResourceId(const std::string& resource_name);

// ParseResourceName parses the resource name and returns the map mapping from
// the collection id to the resource id.
absl::StatusOr<ResourceInfoMap> ParseResourceName(
    const std::string& resource_name);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_RESOURCE_UTIL_H_
