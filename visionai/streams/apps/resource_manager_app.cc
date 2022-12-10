// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <cstdlib>

#include "glog/logging.h"
#include "google/protobuf/text_format.h"
#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/util/json_util.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/flags.h"
#include "visionai/streams/client/control.h"
#include "visionai/util/status/status_macros.h"

ABSL_FLAG(std::string, op_code, "", "The management operation to perform.");
ABSL_FLAG(std::string, mwh_asset_name, "",
          "The mwh asset name for enabling exports.");

namespace {

#define GET_STRING_FLAG_FUNC(FuncName, flag)                              \
  absl::StatusOr<std::string> FuncName() {                                \
    std::string flag_value = absl::GetFlag(FLAGS_##flag);                 \
    if (flag_value.empty()) {                                             \
      return absl::InvalidArgumentError(absl::StrFormat(                  \
          "Given an empty string for command line flag \"%s\".", #flag)); \
    }                                                                     \
    return flag_value;                                                    \
  }

GET_STRING_FLAG_FUNC(GetServiceEndpoint, service_endpoint)
GET_STRING_FLAG_FUNC(GetProjectId, project_id)
GET_STRING_FLAG_FUNC(GetLocationId, location_id)
GET_STRING_FLAG_FUNC(GetClusterId, cluster_id)
GET_STRING_FLAG_FUNC(GetStreamId, stream_id)
GET_STRING_FLAG_FUNC(GetEventId, event_id)

GET_STRING_FLAG_FUNC(GetMwhAssetName, mwh_asset_name)

#undef GET_STRING_FLAG_FUNC

absl::StatusOr<visionai::ClusterSelection> ClusterSelectionFromCommandLine() {
  VAI_ASSIGN_OR_RETURN(auto service_endpoint, GetServiceEndpoint(),
                   _ << "while getting the service endpoint");
  VAI_ASSIGN_OR_RETURN(auto project_id, GetProjectId(),
                   _ << "while getting the project id");
  VAI_ASSIGN_OR_RETURN(auto location_id, GetLocationId(),
                   _ << "while getting the location id");
  visionai::ClusterSelection selection;
  selection.set_service_endpoint(service_endpoint);
  selection.set_project_id(project_id);
  selection.set_location_id(location_id);
  auto cluster_id = GetClusterId();
  if (cluster_id.ok()) {
    selection.set_cluster_id(*cluster_id);
  }
  return selection;
}

absl::Status CreateCluster() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto cluster_id, GetClusterId(),
                   _ << "while getting the cluster id");
  VAI_ASSIGN_OR_RETURN(auto lro,
                   visionai::CreateClusterAsync(selection, cluster_id),
                   _ << "while creating the cluster asynchronously");
  LOG(INFO) << absl::StrFormat(
      "Cluster creation initiated for \"%s\". It will show up when you "
      "get(list) cluster(s) when it is done. You may also check the "
      "following operation id for more information:\n%s",
      cluster_id, lro.name());
  return absl::OkStatus();
}

absl::Status GetCluster() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto cluster_id, GetClusterId(),
                   _ << "while getting the cluster id");
  VAI_ASSIGN_OR_RETURN(auto cluster, visionai::GetCluster(selection, cluster_id),
                   _ << "while getting the cluster");
  std::string output;
  auto status = google::protobuf::util::MessageToJsonString(cluster, &output);
  if (!status.ok()) {
    return absl::Status(absl::StatusCode::kInvalidArgument,
                        "error MessageToJsonString");
  }
  LOG(INFO) << output;
  return absl::OkStatus();
}

absl::Status ListClusters() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto clusters, visionai::ListClusters(selection),
                   _ << "while listing clusters");
  LOG(INFO) << absl::StrFormat("List %d cluster%s.", clusters.size(),
                               clusters.size() < 2 ? "" : "s");
  for (const auto& cluster : clusters) {
    std::string output;
    auto status = google::protobuf::util::MessageToJsonString(cluster, &output);
    if (!status.ok()) {
      return absl::Status(absl::StatusCode::kInvalidArgument,
                          "error MessageToJsonString");
    }
    LOG(INFO) << output << "\n";
  }
  return absl::OkStatus();
}

absl::Status DeleteCluster() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto cluster_id, GetClusterId(),
                   _ << "while getting the cluster id");
  VAI_ASSIGN_OR_RETURN(auto lro,
                   visionai::DeleteClusterAsync(selection, cluster_id),
                   _ << "while deleting the cluster asynchronously");
  LOG(INFO) << absl::StrFormat(
      "Cluster deletion initiated for \"%s\". You may check the "
      "following operation id for more information:\n%s",
      cluster_id, lro.name());
  return absl::OkStatus();
}

absl::Status CreateStream() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_RETURN_IF_ERROR(visionai::CreateStream(selection, stream_id))
      << "while creating the stream";
  return absl::OkStatus();
}

absl::Status GetStream() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_ASSIGN_OR_RETURN(auto stream, visionai::GetStream(selection, stream_id),
                   _ << "while getting the stream");
  std::string output;
  auto status = google::protobuf::util::MessageToJsonString(stream, &output);
  if (!status.ok()) {
    return absl::Status(absl::StatusCode::kInvalidArgument,
                        "error MessageToJsonString");
  }

  LOG(INFO) << output;
  return absl::OkStatus();
}

absl::Status ListStreams() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto streams, visionai::ListStreams(selection),
                   _ << "while listing streams");
  LOG(INFO) << absl::StrFormat("List %d stream%s.", streams.size(),
                               streams.size() < 2 ? "" : "s");
  for (const auto& stream : streams) {
    std::string output;
    auto status = google::protobuf::util::MessageToJsonString(stream, &output);
    if (!status.ok()) {
      return absl::Status(absl::StatusCode::kInvalidArgument,
                          "error MessageToJsonString");
    }
    LOG(INFO) << output << "\n";
  }
  return absl::OkStatus();
}

absl::Status DeleteStream() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_RETURN_IF_ERROR(visionai::DeleteStream(selection, stream_id))
      << "while deleting the stream";
  return absl::OkStatus();
}

absl::Status GetEvent() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto event_id, GetEventId(),
                   _ << "while getting the event id");
  VAI_ASSIGN_OR_RETURN(auto event, visionai::GetEvent(selection, event_id),
                   _ << "while getting the event");
  std::string output;
  auto status = google::protobuf::util::MessageToJsonString(event, &output);
  if (!status.ok()) {
    return absl::Status(absl::StatusCode::kInvalidArgument,
                        "error MessageToJsonString");
  }
  LOG(INFO) << output;
  return absl::OkStatus();
}

absl::Status ListEvents() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto events, visionai::ListEvents(selection),
                   _ << "while listing events");
  LOG(INFO) << absl::StrFormat("List %d event%s.", events.size(),
                               events.size() < 2 ? "" : "s");
  for (const auto& event : events) {
    std::string output;
    auto status = google::protobuf::util::MessageToJsonString(event, &output);
    if (!status.ok()) {
      return absl::Status(absl::StatusCode::kInvalidArgument,
                          "error MessageToJsonString");
    }
    LOG(INFO) << output << "\n";
  }
  return absl::OkStatus();
}

absl::Status EnableMwhExporter() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_ASSIGN_OR_RETURN(auto mwh_asset_name, GetMwhAssetName(),
                   _ << "while getting the mwh asset name");
  VAI_RETURN_IF_ERROR(
      visionai::EnableMwhExporter(selection, stream_id, mwh_asset_name))
      << "while enabling the mwh exporter";
  return absl::OkStatus();
}

absl::Status DisableMwhExporter() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_RETURN_IF_ERROR(visionai::DisableMwhExporter(selection, stream_id))
      << "while disabling the mwh exporter";
  return absl::OkStatus();
}

absl::Status EnableHlsPlayback() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_RETURN_IF_ERROR(visionai::EnableHlsPlayback(selection, stream_id))
      << "while enabling the hls playback";
  return absl::OkStatus();
}

absl::Status DisableHlsPlayback() {
  VAI_ASSIGN_OR_RETURN(auto selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  VAI_RETURN_IF_ERROR(visionai::DisableHlsPlayback(selection, stream_id))
      << "while disabling the hls playback";
  return absl::OkStatus();
}

absl::flat_hash_map<std::string, std::function<absl::Status()>>
CreateCallbackMap() {
  absl::flat_hash_map<std::string, std::function<absl::Status()>> callbacks;
  callbacks["CreateCluster"] = CreateCluster;
  callbacks["GetCluster"] = GetCluster;
  callbacks["ListClusters"] = ListClusters;
  callbacks["DeleteCluster"] = DeleteCluster;
  callbacks["CreateStream"] = CreateStream;
  callbacks["GetStream"] = GetStream;
  callbacks["ListStreams"] = ListStreams;
  callbacks["DeleteStream"] = DeleteStream;
  callbacks["GetEvent"] = GetEvent;
  callbacks["ListEvents"] = ListEvents;
  callbacks["EnableMwhExporter"] = EnableMwhExporter;
  callbacks["DisableMwhExporter"] = DisableMwhExporter;
  callbacks["EnableHlsPlayback"] = EnableHlsPlayback;
  callbacks["DisableHlsPlayback"] = DisableHlsPlayback;
  return callbacks;
}

}  // namespace

int main(int argc, char** argv) {
  std::string usage = R"usage(
Usage: resource_manager_app [OPTION]

Perform Streams API resource management tasks.

)usage";

  absl::SetProgramUsageMessage(usage);
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  std::string op_code = absl::GetFlag(FLAGS_op_code);
  auto callbacks = CreateCallbackMap();

  auto it = callbacks.find(op_code);
  if (it == callbacks.end()) {
    std::vector<std::string> op_codes;
    for (const auto& p : callbacks) {
      op_codes.push_back(p.first);
    }
    std::string op_codes_string = absl::StrJoin(op_codes, "\n");
    LOG(ERROR) << absl::StrFormat(
        "Got an unrecognized op code: \"%s\". Possibilities are: \n%s", op_code,
        op_codes_string);
    return EXIT_FAILURE;
  }

  absl::Status return_status = (it->second)();
  if (!return_status.ok()) {
    LOG(ERROR) << return_status;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
