// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// This binary shows a concrete example for how one might use the C++
// Programming API to receive Packets from Vision AI Streams.
//
// It demonstrates how you might receive protobufs of any type of your choosing.

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/public/streams.h"
#include "visionai/public/tutorial/hello.pb.h"
#include "visionai/util/status/status_macros.h"

// Command line flags.
ABSL_FLAG(std::string, service_endpoint, "",
          "The service endpoint of Vision AI.");
ABSL_FLAG(std::string, project_id, "", "The project-id to receive from.");
ABSL_FLAG(std::string, location_id, "", "The location-id to receive from.");
ABSL_FLAG(std::string, cluster_id, "", "The cluster-id to receive from.");
ABSL_FLAG(std::string, stream_id, "", "The stream-id to receive from.");
ABSL_FLAG(std::string, event_id, "",
          "(Optional) The event-id to receive from.");
ABSL_FLAG(std::string, receiver_id, "", "(Optional) The receiver-id to use.");
ABSL_FLAG(int, timeout_in_seconds, 10,
          "The timeout, in seconds, to wait for an arrival. Non-positive "
          "values correspond to an infinite timeout.");

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
    return absl::InvalidArgumentError("Given an empty stream_id.");
  }
  std::string event_id = absl::GetFlag(FLAGS_event_id);
  std::string receiver_id = absl::GetFlag(FLAGS_receiver_id);

  absl::Duration timeout = absl::InfiniteDuration();
  int timeout_in_seconds = absl::GetFlag(FLAGS_timeout_in_seconds);
  if (timeout_in_seconds > 0) {
    timeout = absl::Seconds(timeout_in_seconds);
  }

  // Configure `StreamReceiver::Options` using settings given in the command
  // line.
  StreamReceiver::Options options;
  options.service_connection_options = service_connection_options;
  options.stream_id = stream_id;
  options.event_id = event_id;
  options.receiver_id = receiver_id;

  // The main receive loop.
  VAI_ASSIGN_OR_RETURN(auto stream_receiver, StreamReceiver::Create(options));
  while (true) {
    Packet p;
    VAI_RETURN_IF_ERROR(stream_receiver->Receive(timeout, &p));
    VAI_ASSIGN_OR_RETURN(auto greeting, FromPacket<Greeting>(std::move(p)));
    LOG(INFO) << greeting.DebugString();
  }

  return absl::OkStatus();
}

}  // namespace visionai

int main(int argc, char** argv) {
  std::string usage = R"usage(
Usage: receiver_app [OPTION]

This is an example binary that receives greetings from Vision AI Streams.

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
