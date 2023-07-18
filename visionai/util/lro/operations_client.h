#ifndef THIRD_PARTY_VISIONAI_UTIL_LRO_OPERATIONS_CLIENT_H_
#define THIRD_PARTY_VISIONAI_UTIL_LRO_OPERATIONS_CLIENT_H_

#include "google/longrunning/operations.grpc.pb.h"
#include "google/longrunning/operations.pb.h"
#include "absl/status/statusor.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/util/net/exponential_backoff.h"

namespace visionai {

// OperationsClient issues grpc requests to check the long running operation
// status.
// An instance of the OperationsClient uses a single grpc channel and can create
// multiple stubs from this channel.
class OperationsClient {
 public:
  OperationsClient(const std::shared_ptr<grpc::Channel>& channel)
      : channel_(channel) {}

  virtual ~OperationsClient() = default;

  // Polls the operation repetitively with exponential backoff.
  // Returns when the operation is done, or the operation failed with error,
  // or the deadline exceeds.
  //
  // This function call uses a single stub repetitively.
  virtual absl::StatusOr<google::protobuf::Any> PollOperation(
      const std::string& operation_name,
      const ConnectionOptions::ClientContextOptions& client_context_options,
      ExponentialBackoff& exponential_backoff, absl::Duration deadline);

 private:
  std::shared_ptr<grpc::Channel> channel_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_LRO_OPERATIONS_CLIENT_H_
