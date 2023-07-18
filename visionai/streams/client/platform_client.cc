#include "visionai/streams/client/platform_client.h"

#include <memory>
#include <string>
#include <utility>

#include "google/cloud/visionai/v1/platform.grpc.pb.h"
#include "google/cloud/visionai/v1/platform.pb.h"
#include "google/longrunning/operations.grpc.pb.h"
#include "google/longrunning/operations.pb.h"
#include "google/protobuf/text_format.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/lro/operations_client.h"
#include "visionai/util/net/exponential_backoff.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
using google::cloud::visionai::v1::AddApplicationStreamInputRequest;
using google::cloud::visionai::v1::AppPlatform;
using google::cloud::visionai::v1::RemoveApplicationStreamInputRequest;

absl::Status AddParentMetadata(
    ConnectionOptions::ClientContextOptions &client_context_options,
    const resource_ids::Application &application) {
  VAI_ASSIGN_OR_RETURN(
      auto parent,
      MakeProjectLocationName(application.project_id, application.location_id));
  client_context_options.mutable_metadata()->insert(
      {kGrpcMetadata, absl::StrFormat("parent=%s", parent)});
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::unique_ptr<PlatformClient>> PlatformClient::Create(
    const std::string &service_address) {
  PlatformClient::Options options;
  return Create(service_address, options, nullptr);
}

absl::StatusOr<std::unique_ptr<PlatformClient>> PlatformClient::Create(
    const std::string &service_address, const PlatformClient::Options &options,
    const std::shared_ptr<OperationsClient> &operations_client) {
  auto client = absl::WrapUnique(new PlatformClient(options));

  client->connection_options_ = options.use_insecure_channel
                                    ? DefaultConnectionOptionsForTest()
                                    : DefaultConnectionOptions();
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      service_address, client->connection_options_))
      << "while configuring authorization information";

  client->channel_ =
      CreateChannel(service_address, client->connection_options_);
  client->operations_client_ =
      operations_client == nullptr
          ? std::make_unique<OperationsClient>(client->channel_)
          : operations_client;
  return std::move(client);
}

absl::Status PlatformClient::AddStreamToApplication(
    const resource_ids::Stream &stream,
    const resource_ids::Application &application) {
  ConnectionOptions::ClientContextOptions client_context_options =
      connection_options_.client_context_options();
  VAI_RETURN_IF_ERROR(AddParentMetadata(client_context_options, application));

  AddApplicationStreamInputRequest request;
  VAI_ASSIGN_OR_RETURN(auto application_name, MakeApplicationName(application));
  VAI_ASSIGN_OR_RETURN(auto stream_name, MakeStreamName(stream));
  request.set_name(application_name);
  request.add_application_stream_inputs()
      ->mutable_stream_with_annotation()
      ->set_stream(stream_name);

  google::longrunning::Operation operation;
  auto stub = AppPlatform::NewStub(channel_);
  VAI_RETURN_IF_ERROR(stub->AddApplicationStreamInput(
      CreateClientContext(client_context_options).get(), request, &operation));
  if (operation.has_error()) {
    return absl::InternalError(absl::StrFormat("Operation failed, response: %s",
                                               operation.DebugString()));
  }
  if (operation.done()) {
    return absl::OkStatus();
  }

  ExponentialBackoff backoff(options_.initial_wait_time, options_.max_wait_time,
                             options_.wait_time_multiplier);
  return operations_client_
      ->PollOperation(operation.name(), client_context_options, backoff,
                      options_.deadline)
      .status();
}

absl::Status PlatformClient::RemoveStreamFromApplication(
    const resource_ids::Stream &stream,
    const resource_ids::Application &application) {
  ConnectionOptions::ClientContextOptions client_context_options =
      connection_options_.client_context_options();
  VAI_RETURN_IF_ERROR(AddParentMetadata(client_context_options, application));

  RemoveApplicationStreamInputRequest request;
  VAI_ASSIGN_OR_RETURN(auto application_name, MakeApplicationName(application));
  VAI_ASSIGN_OR_RETURN(auto stream_name, MakeStreamName(stream));
  request.set_name(application_name);
  request.add_target_stream_inputs()->set_stream(stream_name);

  google::longrunning::Operation operation;
  auto stub = AppPlatform::NewStub(channel_);
  VAI_RETURN_IF_ERROR(stub->RemoveApplicationStreamInput(
      CreateClientContext(client_context_options).get(), request, &operation));
  if (operation.has_error()) {
    return absl::InternalError(absl::StrFormat("Operation failed, response: %s",
                                               operation.DebugString()));
  }
  if (operation.done()) {
    return absl::OkStatus();
  }

  ExponentialBackoff backoff(options_.initial_wait_time, options_.max_wait_time,
                             options_.wait_time_multiplier);
  return operations_client_
      ->PollOperation(operation.name(), client_context_options, backoff,
                      options_.deadline)
      .status();
}

}  // namespace visionai