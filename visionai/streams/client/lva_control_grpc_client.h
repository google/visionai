// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_LVA_CONTROL_GRPC_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_LVA_CONTROL_GRPC_CLIENT_H_

#include <memory>
#include <vector>

#include "google/cloud/visionai/v1/lva_resources.pb.h"
#include "google/cloud/visionai/v1/lva_service.grpc.pb.h"
#include "google/longrunning/operations.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

class LvaClient {
 public:
  static absl::StatusOr<std::unique_ptr<LvaClient>> Create(
      const std::string &endpoint);

  // Create an Analysis.
  absl::Status CreateAnalysis(
      const std::string &parent,
      const google::cloud::visionai::v1::Analysis &analysis,
      const std::string &analysis_id);

  absl::StatusOr<std::vector<google::cloud::visionai::v1::Analysis>>
  ListAnalyses(const std::string &parent);

  // Delete an Analysis.
  absl::Status DeleteAnalysis(const std::string &parent,
                              const std::string &analysis_id);

 private:
  absl::StatusOr<google::protobuf::Any> Wait(
      const std::string &parent,
      const google::longrunning::Operation &operation);

  std::string endpoint_;
  std::unique_ptr<google::cloud::visionai::v1::LiveVideoAnalytics::Stub>
      lva_stub_;
};
}  // namespace visionai


#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_LVA_CONTROL_GRPC_CLIENT_H_
