#include "visionai/util/lro/operations_client.h"

#include "google/longrunning/operations.grpc.pb.h"
#include "google/longrunning/operations.pb.h"
#include "absl/strings/str_format.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"

namespace visionai {

namespace {
using google::longrunning::GetOperationRequest;
using google::longrunning::Operation;
using google::longrunning::Operations;
}  // namespace

absl::StatusOr<google::protobuf::Any> OperationsClient::PollOperation(
    const std::string& operation_name,
    const ConnectionOptions::ClientContextOptions& client_context_options,
    ExponentialBackoff& exponential_backoff, absl::Duration deadline) {
  std::unique_ptr<Operations::Stub> stub = Operations::NewStub(channel_);
  if (stub == nullptr) {
    return absl::UnknownError("Failed to create a gRPC stub");
  }
  GetOperationRequest request;
  request.set_name(operation_name);

  absl::Time start_time = absl::Now();
  while (true) {
    Operation response;
    auto client_context = CreateClientContext(client_context_options);
    auto grpc_status =
        stub->GetOperation(client_context.get(), request, &response);

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

    exponential_backoff.Wait();
    if (absl::Now() - start_time > deadline) {
      return absl::DeadlineExceededError("Failed waiting for operation.");
    }
  }
}

}  // namespace visionai
