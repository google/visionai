// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// This binary is the top level entry point to run an ingester.

#include <iostream>

#include "glog/logging.h"
#include "google/protobuf/text_format.h"
#include "absl/flags/flag.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "visionai/algorithms/media/util/register_plugins_for_sdk.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/ingester.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
using ::google::protobuf::TextFormat;
}  // namespace

absl::Status Run(absl::string_view config_file) {
  VAI_RETURN_IF_ERROR(RegisterGstPluginsForSDK());

  std::string contents;
  VAI_RETURN_IF_ERROR(GetFileContents(config_file, &contents));
  IngesterConfig config;
  if (!config.ParseFromString(contents) &&
      !TextFormat::ParseFromString(contents, &config)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Could not parse the config file \"%s\" as an "
                        "IngesterConfig protobuf message",
                        config_file));
  }
  Ingester ingester(config);
  VAI_RETURN_IF_ERROR(ingester.Prepare());
  VAI_RETURN_IF_ERROR(ingester.Run());
  return absl::OkStatus();
}

}  // namespace visionai

int main(int argc, char* argv[]) {
  std::string usage = R"usage(
Usage: ingester_app [OPTION] INGESTER_CONFIG_FILE

Reads the INGESTER_CONFIG_FILE (a visionai::IngesterConfig protobuf),
and runs the ingester.

)usage";

  absl::SetProgramUsageMessage(usage);
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);

  if (argc <= 1) {
    std::cerr << usage;
    return EXIT_FAILURE;
  }

  auto status = visionai::Run(argv[1]);
  if (!status.ok()) {
    LOG(ERROR) << status;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
