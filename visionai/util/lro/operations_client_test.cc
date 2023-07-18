#include "visionai/util/lro/operations_client.h"

#include "google/longrunning/operations.grpc.pb.h"
#include "google/longrunning/operations.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/time.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/status.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/net/exponential_backoff.h"
#include "visionai/util/net/grpc/client_connect.h"

namespace visionai {
namespace testing {
namespace {

using ::google::longrunning::GetOperationRequest;
using ::google::longrunning::Operation;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::Return;

constexpr char kOperationName[] = "operation-1";

class MockOperationService
    : public ::google::longrunning::Operations::Service {
 public:
  using ServiceType = ::google::longrunning::Operations;
  GRPC_UNARY_MOCK(GetOperation, GetOperationRequest, Operation);
};

class OperationsClientTest : public ::testing::Test {
 protected:
  OperationsClientTest()
      : mock_operations_server(),
        mock_operations_service_(mock_operations_server.service()),
        local_credentials_address_(
            mock_operations_server.local_credentials_server_address()) {}

  MockGrpcServer<MockOperationService> mock_operations_server;
  MockOperationService* mock_operations_service_;

  std::string local_credentials_address_;
};

TEST_F(OperationsClientTest, PollOperationDoneTest) {
  auto conn_opts = DefaultConnectionOptionsForTest();
  auto channel = CreateChannel(local_credentials_address_, conn_opts);
  auto client_context = CreateClientContext(conn_opts);

  EXPECT_CALL(*mock_operations_service_, GetOperation)
      .Times(1)
      .WillOnce(
          Invoke([](::grpc::ServerContext* context,
                    const GetOperationRequest* request, Operation* response) {
            response->set_done(true);
            return grpc::Status::OK;
          }));

  OperationsClient client(channel);
  ExponentialBackoff backoff(absl::Seconds(1), absl::Seconds(10), 2.0f);
  ASSERT_OK(client.PollOperation(kOperationName,
                                 conn_opts.client_context_options(), backoff,
                                 absl::Seconds(10)));
}

TEST_F(OperationsClientTest, PollOperationErrorTest) {
  auto conn_opts = DefaultConnectionOptionsForTest();
  auto channel = CreateChannel(local_credentials_address_, conn_opts);
  auto client_context = CreateClientContext(conn_opts);

  EXPECT_CALL(*mock_operations_service_, GetOperation)
      .Times(1)
      .WillOnce(
          Invoke([](::grpc::ServerContext* context,
                    const GetOperationRequest* request, Operation* response) {
            response->set_done(true);
            response->mutable_error()->set_message("internal error");
            return grpc::Status::OK;
          }));

  OperationsClient client(channel);
  ExponentialBackoff backoff(absl::Seconds(1), absl::Seconds(10), 2.0f);
  EXPECT_THAT(
      client.PollOperation(kOperationName, conn_opts.client_context_options(),
                           backoff, absl::Seconds(10)),
      StatusIs(absl::StatusCode::kInternal,
               HasSubstr("Operation failed, response: ")));
}

TEST_F(OperationsClientTest, PollOperationGrpcErrorTest) {
  auto conn_opts = DefaultConnectionOptionsForTest();
  auto channel = CreateChannel(local_credentials_address_, conn_opts);
  auto client_context = CreateClientContext(conn_opts);

  EXPECT_CALL(*mock_operations_service_, GetOperation)
      .Times(1)
      .WillOnce(
          Invoke([](::grpc::ServerContext* context,
                    const GetOperationRequest* request, Operation* response) {
            return grpc::Status(grpc::StatusCode::UNAVAILABLE,
                                "Server unavailable");
          }));

  OperationsClient client(channel);
  ExponentialBackoff backoff(absl::Seconds(1), absl::Seconds(10), 2.0f);
  EXPECT_THAT(
      client.PollOperation(kOperationName, conn_opts.client_context_options(),
                           backoff, absl::Seconds(10)),
      StatusIs(absl::StatusCode::kUnavailable));
}

TEST_F(OperationsClientTest, PollOperationTimeoutTest) {
  auto conn_opts = DefaultConnectionOptionsForTest();
  auto channel = CreateChannel(local_credentials_address_, conn_opts);
  auto client_context = CreateClientContext(conn_opts);

  EXPECT_CALL(*mock_operations_service_, GetOperation)
      .Times(::testing::AtLeast(2))
      .WillRepeatedly(Return(grpc::Status::OK));

  OperationsClient client(channel);
  ExponentialBackoff backoff(absl::Milliseconds(100), absl::Seconds(2), 1.0f);
  EXPECT_THAT(
      client.PollOperation(kOperationName, conn_opts.client_context_options(),
                           backoff, absl::Milliseconds(500)),
      StatusIs(absl::StatusCode::kDeadlineExceeded,
               HasSubstr("Failed waiting for operation.")));
}

}  // namespace
}  // namespace testing
}  // namespace visionai
