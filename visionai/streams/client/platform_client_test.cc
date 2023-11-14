// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/client/platform_client.h"

#include <memory>

#include "google/cloud/visionai/v1/platform.grpc.pb.h"
#include "google/cloud/visionai/v1/platform.pb.h"
#include "google/longrunning/operations.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/time/time.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/lro/mock_operations_client.h"

namespace visionai {
namespace testing {
namespace {

using ::google::cloud::visionai::v1::AddApplicationStreamInputRequest;
using ::google::cloud::visionai::v1::RemoveApplicationStreamInputRequest;
using ::google::longrunning::Operation;
using ::testing::HasSubstr;
using ::testing::Invoke;

constexpr char kStreamName[] =
    "projects/p1/locations/l2/clusters/c3/streams/s4";
constexpr char kAppName[] = "projects/p1/locations/l2/applications/a3";

class MockPlatformService
    : public ::google::cloud::visionai::v1::AppPlatform::Service {
 public:
  using ServiceType = ::google::cloud::visionai::v1::AppPlatform;
  GRPC_UNARY_MOCK(AddApplicationStreamInput, AddApplicationStreamInputRequest,
                  Operation);
  GRPC_UNARY_MOCK(RemoveApplicationStreamInput,
                  RemoveApplicationStreamInputRequest, Operation);
};

class PlatformClientTest : public ::testing::Test {
 protected:
  PlatformClientTest()
      : mock_platform_server_(),
        mock_platform_service_(mock_platform_server_.service()),
        local_credentials_address_(
            mock_platform_server_.local_credentials_server_address()) {}

  MockGrpcServer<MockPlatformService> mock_platform_server_;
  MockPlatformService* mock_platform_service_;

  std::string local_credentials_address_;
};

TEST_F(PlatformClientTest, AddStreamSuccess) {
  std::string operation_name = "operation_add_stream";

  VAI_ASSERT_OK_AND_ASSIGN(auto stream, ParseStreamNameStructured(kStreamName));
  VAI_ASSERT_OK_AND_ASSIGN(auto application,
                       ParseApplicationNameStructured(kAppName));

  std::shared_ptr<MockOperationsClient> mock_operations_client =
      std::make_shared<MockOperationsClient>();

  EXPECT_CALL(*mock_platform_service_, AddApplicationStreamInput)
      .Times(1)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const AddApplicationStreamInputRequest* request,
                           Operation* response) {
        EXPECT_EQ(request->name(), kAppName);
        EXPECT_EQ(request->application_stream_inputs_size(), 1);
        EXPECT_EQ(request->application_stream_inputs(0)
                      .stream_with_annotation()
                      .stream(),
                  kStreamName);
        response->set_done(false);
        response->set_name(operation_name);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_operations_client, PollOperation)
      .Times(1)
      .WillOnce(Invoke([&](const std::string& name,
                           const ConnectionOptions::ClientContextOptions&
                               client_context_options,
                           ExponentialBackoff& exponential_backoff,
                           absl::Duration deadline) {
        EXPECT_EQ(name, operation_name);
        EXPECT_TRUE(client_context_options.metadata().contains(kGrpcMetadata));
        return google::protobuf::Any();
      }));

  PlatformClient::Options options;
  options.use_insecure_channel = true;
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       PlatformClient::Create(local_credentials_address_,
                                              options, mock_operations_client));

  EXPECT_TRUE(control_client->AddStreamToApplication(stream, application).ok());
}

TEST_F(PlatformClientTest, AddStreamImmediateFailure) {
  std::string operation_name = "operation_add_stream";

  VAI_ASSERT_OK_AND_ASSIGN(auto stream, ParseStreamNameStructured(kStreamName));
  VAI_ASSERT_OK_AND_ASSIGN(auto application,
                       ParseApplicationNameStructured(kAppName));

  std::shared_ptr<MockOperationsClient> mock_operations_client =
      std::make_shared<MockOperationsClient>();

  EXPECT_CALL(*mock_platform_service_, AddApplicationStreamInput)
      .Times(1)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const AddApplicationStreamInputRequest* request,
                           Operation* response) {
        response->set_done(true);
        response->set_name(operation_name);
        response->mutable_error()->set_code(3);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_operations_client, PollOperation).Times(0);

  PlatformClient::Options options;
  options.use_insecure_channel = true;
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       PlatformClient::Create(local_credentials_address_,
                                              options, mock_operations_client));
  EXPECT_THAT(control_client->AddStreamToApplication(stream, application),
              StatusIs(absl::StatusCode::kInternal,
                       HasSubstr("Operation failed, response: ")));
}

TEST_F(PlatformClientTest, AddStreamImmediateDone) {
  std::string operation_name = "operation_add_stream";

  VAI_ASSERT_OK_AND_ASSIGN(auto stream, ParseStreamNameStructured(kStreamName));
  VAI_ASSERT_OK_AND_ASSIGN(auto application,
                       ParseApplicationNameStructured(kAppName));

  std::shared_ptr<MockOperationsClient> mock_operations_client =
      std::make_shared<MockOperationsClient>();

  EXPECT_CALL(*mock_platform_service_, AddApplicationStreamInput)
      .Times(1)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const AddApplicationStreamInputRequest* request,
                           Operation* response) {
        response->set_done(true);
        response->set_name(operation_name);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_operations_client, PollOperation).Times(0);

  PlatformClient::Options options;
  options.use_insecure_channel = true;
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       PlatformClient::Create(local_credentials_address_,
                                              options, mock_operations_client));

  EXPECT_TRUE(control_client->AddStreamToApplication(stream, application).ok());
}

TEST_F(PlatformClientTest, RemoveStreamSuccess) {
  std::string operation_name = "operation_remove_stream";

  VAI_ASSERT_OK_AND_ASSIGN(auto stream, ParseStreamNameStructured(kStreamName));
  VAI_ASSERT_OK_AND_ASSIGN(auto application,
                       ParseApplicationNameStructured(kAppName));

  std::shared_ptr<MockOperationsClient> mock_operations_client =
      std::make_shared<MockOperationsClient>();

  EXPECT_CALL(*mock_platform_service_, RemoveApplicationStreamInput)
      .Times(1)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const RemoveApplicationStreamInputRequest* request,
                           Operation* response) {
        EXPECT_EQ(request->name(), kAppName);
        EXPECT_EQ(request->target_stream_inputs_size(), 1);
        EXPECT_EQ(request->target_stream_inputs(0).stream(), kStreamName);
        response->set_done(false);
        response->set_name(operation_name);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_operations_client, PollOperation)
      .Times(1)
      .WillOnce(Invoke([&](const std::string& name,
                           const ConnectionOptions::ClientContextOptions&
                               client_context_options,
                           ExponentialBackoff& exponential_backoff,
                           absl::Duration deadline) {
        EXPECT_EQ(name, operation_name);
        EXPECT_TRUE(client_context_options.metadata().contains(kGrpcMetadata));
        return google::protobuf::Any();
      }));

  PlatformClient::Options options;
  options.use_insecure_channel = true;
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       PlatformClient::Create(local_credentials_address_,
                                              options, mock_operations_client));

  EXPECT_TRUE(
      control_client->RemoveStreamFromApplication(stream, application).ok());
}

TEST_F(PlatformClientTest, RemoveStreamImmediateFailure) {
  std::string operation_name = "operation_remove_stream";

  VAI_ASSERT_OK_AND_ASSIGN(auto stream, ParseStreamNameStructured(kStreamName));
  VAI_ASSERT_OK_AND_ASSIGN(auto application,
                       ParseApplicationNameStructured(kAppName));

  std::shared_ptr<MockOperationsClient> mock_operations_client =
      std::make_shared<MockOperationsClient>();

  EXPECT_CALL(*mock_platform_service_, RemoveApplicationStreamInput)
      .Times(1)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const RemoveApplicationStreamInputRequest* request,
                           Operation* response) {
        response->set_done(true);
        response->set_name(operation_name);
        response->mutable_error()->set_code(3);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_operations_client, PollOperation).Times(0);

  PlatformClient::Options options;
  options.use_insecure_channel = true;
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       PlatformClient::Create(local_credentials_address_,
                                              options, mock_operations_client));
  EXPECT_THAT(control_client->RemoveStreamFromApplication(stream, application),
              StatusIs(absl::StatusCode::kInternal,
                       HasSubstr("Operation failed, response: ")));
}

TEST_F(PlatformClientTest, RemoveStreamImmediateDone) {
  std::string operation_name = "operation_remove_stream";

  VAI_ASSERT_OK_AND_ASSIGN(auto stream, ParseStreamNameStructured(kStreamName));
  VAI_ASSERT_OK_AND_ASSIGN(auto application,
                       ParseApplicationNameStructured(kAppName));

  std::shared_ptr<MockOperationsClient> mock_operations_client =
      std::make_shared<MockOperationsClient>();

  EXPECT_CALL(*mock_platform_service_, RemoveApplicationStreamInput)
      .Times(1)
      .WillOnce(Invoke([&](::grpc::ServerContext* context,
                           const RemoveApplicationStreamInputRequest* request,
                           Operation* response) {
        response->set_done(true);
        response->set_name(operation_name);
        return grpc::Status::OK;
      }));
  EXPECT_CALL(*mock_operations_client, PollOperation).Times(0);

  PlatformClient::Options options;
  options.use_insecure_channel = true;
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       PlatformClient::Create(local_credentials_address_,
                                              options, mock_operations_client));

  EXPECT_TRUE(
      control_client->RemoveStreamFromApplication(stream, application).ok());
}
}  // namespace

}  // namespace testing
}  // namespace visionai