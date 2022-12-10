// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/lva_control_grpc_client.h"

#include <memory>
#include <utility>

#include "google/cloud/visionai/v1/lva.pb.h"
#include "google/cloud/visionai/v1/lva_resources.pb.h"
#include "google/cloud/visionai/v1/lva_service.grpc.pb.h"
#include "google/cloud/visionai/v1/lva_service.pb.h"
#include "google/longrunning/operations.grpc.pb.h"
#include "google/protobuf/text_format.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/util/net/exponential_backoff.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
absl::StatusOr<std::unique_ptr<LvaClient>> LvaClient::Create(
    const std::string &endpoint) {
  auto client = absl::WrapUnique(new LvaClient());
  auto channel = CreateChannel(endpoint, DefaultConnectionOptions());
  client->endpoint_ = endpoint;
  client->lva_stub_ =
      google::cloud::visionai::v1::LiveVideoAnalytics::NewStub(channel);
  return std::move(client);
}

absl::Status LvaClient::CreateAnalysis(
    const std::string &parent,
    const google::cloud::visionai::v1::Analysis &analysis,
    const std::string &analysis_id) {
  auto context = CreateClientContext(DefaultConnectionOptions());
  context->AddMetadata("x-goog-request-params",
                       absl::StrFormat("parent=%s", parent));
  google::cloud::visionai::v1::CreateAnalysisRequest request;
  request.set_parent(parent);
  request.set_analysis_id(analysis_id);
  *request.mutable_analysis() = analysis;
  google::longrunning::Operation operation;
  VAI_RETURN_IF_ERROR(
      lva_stub_->CreateAnalysis(context.get(), request, &operation));
  return Wait(parent, operation).status();
}

absl::Status LvaClient::DeleteAnalysis(const std::string &parent,
                                       const std::string &analysis_id) {
  auto context = CreateClientContext(DefaultConnectionOptions());
  context->AddMetadata("x-goog-request-params",
                       absl::StrFormat("parent=%s", parent));
  google::cloud::visionai::v1::DeleteAnalysisRequest request;
  request.set_name(absl::StrFormat("%s/analyses/%s", parent, analysis_id));
  google::longrunning::Operation operation;
  VAI_RETURN_IF_ERROR(
      lva_stub_->DeleteAnalysis(context.get(), request, &operation));
  return Wait(parent, operation).status();
}

absl::StatusOr<std::vector<google::cloud::visionai::v1::Analysis>>
LvaClient::ListAnalyses(const std::string &parent) {
  auto context = CreateClientContext(DefaultConnectionOptions());
  context->AddMetadata("x-goog-request-params",
                       absl::StrFormat("parent=%s", parent));
  google::cloud::visionai::v1::ListAnalysesRequest request;
  google::cloud::visionai::v1::ListAnalysesResponse response;

  request.set_parent(parent);
  VAI_RETURN_IF_ERROR(lva_stub_->ListAnalyses(context.get(), request, &response));
  return std::vector<google::cloud::visionai::v1::Analysis>{
      response.analyses().begin(), response.analyses().end()};
}

absl::StatusOr<google::protobuf::Any> LvaClient::Wait(
    const std::string &parent,
    const google::longrunning::Operation &operation) {
  if (operation.has_error()) {
    return absl::InternalError(absl::StrFormat("Operation failed, response: %s",
                                               operation.DebugString()));
  }
  if (operation.done()) {
    return operation.response();
  }
  auto channel = CreateChannel(endpoint_, DefaultConnectionOptions());
  if (channel == nullptr) {
    return absl::UnknownError("Failed to create a gRPC channel");
  }
  std::unique_ptr<google::longrunning::Operations::Stub> stub =
      google::longrunning::Operations::NewStub(channel);
  if (stub == nullptr) {
    return absl::UnknownError("Failed to create a gRPC stub");
  }

  // Gives around 30 minutes for a 20 trial limit.
  ExponentialBackoff exponential_backoff(absl::Seconds(2), absl::Minutes(2),
                                         2.0f);
  int remaining_trials = 20;
  do {
    google::longrunning::GetOperationRequest request;
    request.set_name(operation.name());
    google::longrunning::Operation response;
    auto context = CreateClientContext(DefaultConnectionOptions());
    context->AddMetadata("x-goog-request-params",
                         absl::StrFormat("parent=%s", parent));

    auto grpc_status = stub->GetOperation(context.get(), request, &response);
    if (!grpc_status.ok()) {
      return ToAbseilStatus(grpc_status);
    }
    if (response.has_error()) {
      return absl::InternalError(absl::StrFormat(
          "Operation failed, response: %s", response.DebugString()));
    }
    if (response.done()) {
      return response.response();
    }
    if (--remaining_trials <= 0) {
      break;
    }
    exponential_backoff.Wait();
  } while (true);
  return absl::DeadlineExceededError("Failed waiting for operation.");
}

}  // namespace visionai
