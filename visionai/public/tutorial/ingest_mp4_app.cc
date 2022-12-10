// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// This binary shows a concrete example for how one might use the C++
// Programming API to send Packets to Vision AI Streams.
//
// It demonstrates how you might send a protobuf of any type of your choosing,
// as this is probably the most flexible way for you to define your custom data
// type.
//
// The stream-id to send to is assume to have been created. However, you may
// also modify this program straightforwardly to create it if it doesn't already
// exist using the "Control" API in streams.h.

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "visionai/public/streams.h"
#include "visionai/util/status/status_macros.h"

// Command line flags.
ABSL_FLAG(std::string, service_endpoint, "",
          "The service endpoint of Vision AI.");
ABSL_FLAG(std::string, project_id, "", "The project-id to send to.");
ABSL_FLAG(std::string, location_id, "", "The location-id to send to.");
ABSL_FLAG(std::string, cluster_id, "", "The cluster-id to send to.");
ABSL_FLAG(std::string, stream_id, "", "The stream-id to send to.");
ABSL_FLAG(std::string, file_name, "", "The name of the MP4 file to ingest.");

namespace visionai {

namespace {

absl::Status ValidateOptions(const ServiceConnectionOptions& options) {
  if (options.service_endpoint.empty()) {
    return absl::InvalidArgumentError("Given an empty service_endpoint.");
  }
  if (options.project_id.empty()) {
    return absl::InvalidArgumentError("Given an empty project_id.");
  }
  if (options.location_id.empty()) {
    return absl::InvalidArgumentError("Given an empty location_id.");
  }
  if (options.cluster_id.empty()) {
    return absl::InvalidArgumentError("Given an empty cluster_id.");
  }
  return absl::OkStatus();
}

absl::StatusOr<ServiceConnectionOptions> OptionsFromCommandline() {
  ServiceConnectionOptions options;
  options.service_endpoint = absl::GetFlag(FLAGS_service_endpoint);
  options.project_id = absl::GetFlag(FLAGS_project_id);
  options.location_id = absl::GetFlag(FLAGS_location_id);
  options.cluster_id = absl::GetFlag(FLAGS_cluster_id);
  VAI_RETURN_IF_ERROR(ValidateOptions(options));
  return options;
}

}  // namespace

absl::Status Run() {
  // Read and validate the commandline options.
  VAI_ASSIGN_OR_RETURN(auto service_connection_options,
                       OptionsFromCommandline());
  std::string stream_id = absl::GetFlag(FLAGS_stream_id);
  if (stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty `stream_id`");
  }
  std::string file_name = absl::GetFlag(FLAGS_file_name);
  if (file_name.empty()) {
    return absl::InvalidArgumentError("Given an empty `file_name`");
  }

  // Run the ingest mp4 function.
  VAI_RETURN_IF_ERROR(
      IngestMp4(service_connection_options, stream_id, file_name));

  return absl::OkStatus();
}

}  // namespace visionai

int main(int argc, char** argv) {
  std::string usage = R"usage(
Usage: ingest_mp4_app [OPTION]

This is an example binary that demonstrates how one may use the
C++ programming API to ingest an MP4 file to Vision AI Streams.

Use --help to list the command line options.

)usage";

  absl::SetProgramUsageMessage(usage);

  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  auto status = visionai::Run();
  if (!status.ok()) {
    LOG(ERROR) << status;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
