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
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "visionai/public/streams.h"
#include "visionai/public/tutorial/hello.pb.h"
#include "visionai/util/status/status_macros.h"

// Command line flags.
ABSL_FLAG(std::string, service_endpoint, "",
          "The service endpoint of Vision AI.");
ABSL_FLAG(std::string, project_id, "", "The project-id to send to.");
ABSL_FLAG(std::string, location_id, "", "The location-id to send to.");
ABSL_FLAG(std::string, cluster_id, "", "The cluster-id to send to.");
ABSL_FLAG(std::string, stream_id, "", "The stream-id to send to.");
ABSL_FLAG(std::string, event_id, "", "(Optional) The event-id to send to.");
ABSL_FLAG(std::string, sender_id, "", "(Optional) The sender-id to use.");

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
  std::string sender_id = absl::GetFlag(FLAGS_sender_id);

  // Configure `StreamSender::Options` using settings given in the command line.
  StreamSender::Options options;
  options.service_connection_options = service_connection_options;
  options.stream_id = stream_id;
  options.event_id = event_id;
  options.sender_id = sender_id;

  // Create a `StreamSender` instance.
  VAI_ASSIGN_OR_RETURN(auto stream_sender, StreamSender::Create(options));

  // The main send loop. For this example, we just send the same message at one
  // second intervals. In a real use case, you might get the Packets from a
  // local data source.
  for (int i = 0;; ++i) {
    Greeting greeting;
    greeting.set_iterations(i);
    greeting.set_greeting("hello!");
    VAI_ASSIGN_OR_RETURN(auto packet, ToPacket(std::move(greeting)));
    VAI_RETURN_IF_ERROR(stream_sender->Send(std::move(packet)));
    absl::SleepFor(absl::Seconds(1));
    if (i % 10 == 9) {
      LOG(INFO) << absl::StrFormat("Successfully sent %d greetings", i + 1);
    }
  }

  return absl::OkStatus();
}

}  // namespace visionai

int main(int argc, char** argv) {
  std::string usage = R"usage(
Usage: sender_app [OPTION]

This is an example binary that demonstrates how one may use the
C++ programming API to send Packets to Vision AI Streams.

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
