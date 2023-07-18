// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/resource_util.h"

#include <algorithm>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/util/random_string.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

// Implementation notes:
//
// The current AIS resource hierarchy and url looks like this:
//
// projects/{project-id}/locations/{location-id}/clusters/{cluster-id}
// projects/{project-id}/locations/{location-id}/clusters/{cluster-id}/events/{event-id}
// projects/{project-id}/locations/{location-id}/clusters/{cluster-id}/streams/{stream-id}
//
// TODO(yxyan): This will become channel.
// projects/{project-id}/locations/{location-id}/clusters/{cluster-id}/series/{series-id}
using resource_ids::Application;
using resource_ids::Stream;

constexpr char kResourceIdRegex[] = "^[a-z]([a-z0-9-]{0,61}[a-z0-9])?$";
constexpr char kChannelIdSeparator[] = "--";
constexpr char kResourceIdFirstCharAlphabet[] = "abcdefghijklmnopqrstuvwxyz";
constexpr char kResourceIdSuffixAlphabet[] =
    "0123456789abcdefghijklmnopqrstuvwxyz";

bool IsValidResourceId(const std::string resource_id) {
  static std::regex re(kResourceIdRegex);
  return std::regex_match(resource_id, re);
}

bool IsClusterResource(const ResourceInfoMap& resource_info) {
  return resource_info.contains(kProjectsCollectionId) &&
         resource_info.contains(kLocationsCollectionId) &&
         resource_info.contains(kClustersCollectionId) &&
         resource_info.size() == 3;
}

bool IsClusterSubresource(const ResourceInfoMap& resource_info) {
  return resource_info.contains(kProjectsCollectionId) &&
         resource_info.contains(kLocationsCollectionId) &&
         resource_info.contains(kClustersCollectionId) &&
         resource_info.size() == 4;
}

bool IsStreamResource(const ResourceInfoMap& resource_info) {
  return IsClusterSubresource(resource_info) &&
         resource_info.contains(kStreamsCollectionId);
}

bool IsLeafResource(const ResourceInfoMap& resource_info) {
  return IsClusterSubresource(resource_info) &&
         (resource_info.contains(kEventsCollectionId) ||
          resource_info.contains(kStreamsCollectionId) ||
          resource_info.contains(kChannelsCollectionId));
}

bool IsApplication(const ResourceInfoMap& resource_info) {
  return resource_info.contains(kProjectsCollectionId) &&
         resource_info.contains(kLocationsCollectionId) &&
         resource_info.contains(kApplicationsCollectionId) &&
         resource_info.size() == 3;
}

}  // namespace

// ----------------------------------------------------------------------------
// Methods for standardizing AIS resource names
// ----------------------------------------------------------------------------

absl::StatusOr<std::string> MakeProjectLocationName(
    const std::string& project_id, const std::string& location_id) {
  if (!IsValidResourceId(project_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given project_id (%s) is not valid. Must match %s",
                        project_id, kResourceIdRegex));
  }
  if (!IsValidResourceId(location_id)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given location_id (%s) is not valid. Must match %s", location_id,
        kResourceIdRegex));
  }
  std::vector<std::string> segments;
  segments.push_back(kProjectsCollectionId);
  segments.push_back(project_id);
  segments.push_back(kLocationsCollectionId);
  segments.push_back(location_id);
  return absl::StrJoin(segments, "/");
}

absl::StatusOr<std::string> MakeProjectLocationName(
    const ClusterSelection& selection) {
  return MakeProjectLocationName(selection.project_id(),
                                 selection.location_id());
}

absl::StatusOr<std::string> MakeClusterName(const std::string& project_id,
                                            const std::string& location_id,
                                            const std::string& cluster_id) {
  if (!IsValidResourceId(project_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given project_id (%s) is not valid. Must match %s",
                        project_id, kResourceIdRegex));
  }
  if (!IsValidResourceId(location_id)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given location_id (%s) is not valid. Must match %s", location_id,
        kResourceIdRegex));
  }
  if (!IsValidResourceId(cluster_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given cluster_id (%s) is not valid. Must match %s",
                        cluster_id, kResourceIdRegex));
  }
  std::vector<std::string> segments;
  segments.push_back(kProjectsCollectionId);
  segments.push_back(project_id);
  segments.push_back(kLocationsCollectionId);
  segments.push_back(location_id);
  segments.push_back(kClustersCollectionId);
  segments.push_back(cluster_id);
  return absl::StrJoin(segments, "/");
}

absl::StatusOr<std::string> MakeClusterName(const ClusterSelection& selection) {
  return MakeClusterName(selection.project_id(), selection.location_id(),
                         selection.cluster_id());
}

absl::StatusOr<std::string> MakeEventName(const std::string& cluster_name,
                                          const std::string& event_id) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(cluster_name),
                   _.LogError() << "Failed to parse the cluster name.");
  if (!IsClusterResource(resource_info)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given cluster name is not valid: %s", cluster_name));
  }
  if (!IsValidResourceId(event_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given event_id (%s) is not valid. Must match %s",
                        event_id, kResourceIdRegex));
  }
  std::vector<std::string> segments;
  segments.push_back(cluster_name);
  segments.push_back(kEventsCollectionId);
  segments.push_back(event_id);
  return absl::StrJoin(segments, "/");
}

absl::StatusOr<std::string> MakeEventName(const ClusterSelection& selection,
                                          const std::string& event_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, MakeClusterName(selection));
  return MakeEventName(cluster_name, event_id);
}

absl::StatusOr<std::string> MakeStreamName(const std::string& cluster_name,
                                           const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(cluster_name),
                   _.LogError() << "Failed to parse the cluster name.");
  if (!IsClusterResource(resource_info)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given cluster name is not valid: %s", cluster_name));
  }
  if (!IsValidResourceId(stream_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given stream_id (%s) is not valid. Must match %s",
                        stream_id, kResourceIdRegex));
  }
  std::vector<std::string> segments;
  segments.push_back(cluster_name);
  segments.push_back(kStreamsCollectionId);
  segments.push_back(stream_id);
  return absl::StrJoin(segments, "/");
}

absl::StatusOr<std::string> MakeStreamName(const ClusterSelection& selection,
                                           const std::string& stream_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, MakeClusterName(selection));
  return MakeStreamName(cluster_name, stream_id);
}

absl::StatusOr<std::string> MakeStreamName(const Stream& stream) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name,
                   MakeClusterName(stream.project_id, stream.location_id,
                                   stream.cluster_id));
  return MakeStreamName(cluster_name, stream.stream_id);
}

absl::StatusOr<std::string> MakeChannelName(const std::string& cluster_name,
                                            const std::string& channel_id) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(cluster_name),
                   _.LogError() << "Failed to parse the cluster name.");
  if (!IsClusterResource(resource_info)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given cluster name is not valid: %s", cluster_name));
  }
  if (!IsValidResourceId(channel_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given channel_id (%s) is not valid. Must match %s",
                        channel_id, kResourceIdRegex));
  }
  std::vector<std::string> segments;
  segments.push_back(cluster_name);
  segments.push_back(kChannelsCollectionId);
  segments.push_back(channel_id);
  return absl::StrJoin(segments, "/");
}

absl::StatusOr<std::string> MakeChannelName(const ClusterSelection& selection,
                                            const std::string& channel_id) {
  VAI_ASSIGN_OR_RETURN(auto cluster_name, MakeClusterName(selection));
  return MakeChannelName(cluster_name, channel_id);
}

absl::StatusOr<std::string> MakeApplicationName(
    const Application& application) {
  VAI_ASSIGN_OR_RETURN(
      auto project_location_name,
      MakeProjectLocationName(application.project_id, application.location_id));
  std::vector<std::string> segments;
  segments.push_back(project_location_name);
  segments.push_back(kApplicationsCollectionId);
  segments.push_back(application.application_id);
  return absl::StrJoin(segments, "/");
}

absl::StatusOr<std::vector<std::string>> ParseClusterName(
    const std::string& cluster_name) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(cluster_name),
                   _.LogError() << "Failed to parse the cluster name.");
  if (!IsClusterResource(resource_info)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given cluster name is not valid: %s", cluster_name));
  }
  return std::vector<std::string>({resource_info[kProjectsCollectionId],
                                   resource_info[kLocationsCollectionId],
                                   resource_info[kClustersCollectionId]});
}

absl::StatusOr<std::vector<std::string>> ParseStreamName(
    const std::string& stream_name) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(stream_name),
                   _.LogError() << "Failed to parse the stream name.");
  if (!IsStreamResource(resource_info)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given stream name is not valid: %s", stream_name));
  }
  return std::vector<std::string>({resource_info[kProjectsCollectionId],
                                   resource_info[kLocationsCollectionId],
                                   resource_info[kClustersCollectionId],
                                   resource_info[kStreamsCollectionId]});
}

absl::StatusOr<Stream> ParseStreamNameStructured(
    const std::string& stream_name) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(stream_name),
                   _.LogError() << "Failed to parse the stream name.");
  if (!IsStreamResource(resource_info)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given stream name is not valid: %s", stream_name));
  }
  return Stream{.project_id = resource_info[kProjectsCollectionId],
                .location_id = resource_info[kLocationsCollectionId],
                .cluster_id = resource_info[kClustersCollectionId],
                .stream_id = resource_info[kStreamsCollectionId]};
}

absl::StatusOr<Application> ParseApplicationNameStructured(
    const std::string& application_name) {
  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(application_name),
                   _.LogError() << "Failed to parse the stream name.");
  if (!IsApplication(resource_info)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given application name is not valid: %s", application_name));
  }
  return Application{
      .project_id = resource_info[kProjectsCollectionId],
      .location_id = resource_info[kLocationsCollectionId],
      .application_id = resource_info[kApplicationsCollectionId]};
}

std::string NewEventId() {
  return absl::StrFormat("ev-%d", absl::ToUnixNanos(absl::Now()));
}

absl::StatusOr<std::string> MakeChannelId(const std::string& event_id,
                                          const std::string& stream_id) {
  if (!IsValidResourceId(event_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given event_id (%s) is not valid. Must match %s",
                        event_id, kResourceIdRegex));
  }
  if (!IsValidResourceId(stream_id)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given stream_id (%s) is not valid. Must match %s",
                        stream_id, kResourceIdRegex));
  }

  // TODO: This currently doesn't enforce the 65 char limit.
  return event_id + kChannelIdSeparator + stream_id;
}

absl::StatusOr<std::string> EventIdFromChannelId(
    const std::string& channel_id) {
  std::vector<absl::string_view> components =
      absl::StrSplit(channel_id, kChannelIdSeparator);
  if (components.size() != 2) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Given an invalid channel_id (%s).", channel_id));
  }
  return std::string(components[0]);
}

absl::StatusOr<std::string> StreamIdFromChannelId(
    const std::string& channel_id) {
  std::vector<absl::string_view> components =
      absl::StrSplit(channel_id, kChannelIdSeparator);
  if (components.size() != 2) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Given an invalid channel_id (%s).", channel_id));
  }
  return std::string(components[1]);
}

// ----------------------------------------------------------------------------
// General helper methods.
// ----------------------------------------------------------------------------

absl::StatusOr<std::string> GetClusterName(const std::string& resource_name) {
  if (resource_name.empty()) {
    return absl::InvalidArgumentError("Given an empty resource name.");
  }

  VAI_ASSIGN_OR_RETURN(auto resource_info, ParseResourceName(resource_name),
                   _.LogError() << "Failed to parse the given resource name.");

  if (!IsLeafResource(resource_info)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given resource name (%s) is not an event or stream.",
        resource_name));
  }

  return MakeClusterName(resource_info[kProjectsCollectionId],
                         resource_info[kLocationsCollectionId],
                         resource_info[kClustersCollectionId]);
}

absl::StatusOr<std::string> RandomResourceId(size_t length) {
  if (length < 1) {
    return absl::InvalidArgumentError("Resource ids cannot have length 0.");
  }
  return RandomString(1, kResourceIdFirstCharAlphabet) +
         RandomString(length - 1, kResourceIdSuffixAlphabet);
}

absl::StatusOr<bool> InSameCluster(const std::string& name1,
                                   const std::string& name2) {
  VAI_ASSIGN_OR_RETURN(auto cluster1, GetClusterName(name1),
                   _.LogError() << "Failed to get cluster name of " << name1);
  VAI_ASSIGN_OR_RETURN(auto cluster2, GetClusterName(name2),
                   _.LogError() << "Failed to get cluster name of " << name2);
  return cluster1 == cluster2;
}

// TODO: Clean this up. Should try to validate for valid resource-ids.
absl::StatusOr<std::string> ResourceParentName(
    const std::string& resource_name) {
  if (resource_name.empty()) {
    return absl::InvalidArgumentError("Given an empty resource name.");
  }

  std::vector<absl::string_view> segments =
      absl::StrSplit(resource_name, '/', absl::SkipWhitespace());
  if (segments.size() < 3) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The given resource name \"%s\" does not have a parent.",
        resource_name));
  }
  segments.pop_back();
  segments.pop_back();

  return absl::StrJoin(segments, "/");
}

// TODO: Clean this up. Should try to weed out stuff like ///test-stream//.
absl::StatusOr<std::string> ResourceId(const std::string& resource_name) {
  if (resource_name.empty()) {
    return absl::InvalidArgumentError("Given an empty resource name.");
  }

  std::vector<absl::string_view> segments =
      absl::StrSplit(resource_name, "/", absl::SkipWhitespace());
  return std::string(segments.back());
}

// ParseResourceName parses the resource name and returns the map mapping from
// the collection id to the resource id.
absl::StatusOr<ResourceInfoMap> ParseResourceName(
    const std::string& resource_name) {
  std::vector<absl::string_view> segments = absl::StrSplit(resource_name, '/');
  if (segments.size() % 2 == 1) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given resource name must consists of pairs of "
                        "collection-id/resource-id."));
  }
  if (segments.size() > 8) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Resource names in AIS has at most 4 collection identifiers."));
  }

  ResourceInfoMap resource_info;
  std::vector<std::string> collection_ids = {kProjectsCollectionId,
                                             kLocationsCollectionId};
  for (size_t i = 0; i < segments.size() / 2; ++i) {
    std::string collection_id = std::string(segments[2 * i]);
    std::string resource_id = std::string(segments[2 * i + 1]);

    if (!IsValidResourceId(resource_id)) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Given an invalid resource-id for the collection %s: %s",
          collection_id, resource_id));
    }

    if (i < 2) {
      if (collection_id != collection_ids[i]) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "The collection-id at level %d must be %s but is given: %s", i,
            collection_ids[i], collection_id));
      }
    } else if (i == 2) {
      if (collection_id != kClustersCollectionId &&
          collection_id != kApplicationsCollectionId) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "The third collection-id must be either \"clusters\" "
            "or \"applications\", but was given \"%s\"",
            collection_id));
      }
    } else {
      if (collection_id != kEventsCollectionId &&
          collection_id != kStreamsCollectionId &&
          collection_id != kChannelsCollectionId) {
        return absl::InvalidArgumentError(
            absl::StrFormat("The last collection-id must be either \"events\", "
                            "\"streams\", or \"series\" but was given \"%s\"",
                            collection_id));
      }
    }
    resource_info[collection_id] = resource_id;
  }
  return resource_info;
}

}  // namespace visionai
