// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/testing/grpc/mock_grpc.h"

#include <string>
#include <thread>

#include "gtest/gtest.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"
#include "visionai/testing/grpc/echo.grpc.pb.h"
#include "visionai/testing/grpc/echo.pb.h"
#include "visionai/testing/grpc/mock_echo_service.h"

namespace visionai {
namespace testing {
namespace {

using ::testing::Invoke;

class GrpcMockerTest : public ::testing::Test {
 protected:
  GrpcMockerTest()
      : mock_echo_(),
        mock_echo_service_(mock_echo_.service()),
        echo_stub_(mock_echo_.stub()) {}

  MockGrpcServer<MockEchoService> mock_echo_;
  MockEchoService* mock_echo_service_;
  EchoService::Stub* echo_stub_;
};

TEST_F(GrpcMockerTest, EchoUnary) {
  EchoRequest request;
  EchoResponse response;
  ::grpc::ClientContext context;
  std::string payload = "unary_echo";
  request.set_message(payload);
  EXPECT_CALL(*mock_echo_service_, Echo)
      .WillOnce(Invoke([](::grpc::ServerContext* context,
                          const EchoRequest* request, EchoResponse* response) {
        response->set_message(request->message());
        return ::grpc::Status::OK;
      }));
  EXPECT_TRUE(echo_stub_->Echo(&context, request, &response).ok());
  EXPECT_EQ(payload, response.message());
}

TEST_F(GrpcMockerTest, EchoClientStreaming) {
  int client_write_entries = 3;
  std::string payload = "client_streaming_echo";
  EXPECT_CALL(*mock_echo_service_, ClientStreamingEcho)
      .WillOnce(Invoke([payload, client_write_entries](
                           ::grpc::ServerContext* context,
                           ::grpc::ServerReader<EchoRequest>* reader,
                           EchoResponse* response) {
        int request_count = 0;
        EchoRequest request;
        while (reader->Read(&request)) {
          request_count++;
          EXPECT_EQ(payload, request.message());
        }
        EXPECT_EQ(client_write_entries, request_count);
        response->set_message(request.message());
        return ::grpc::Status::OK;
      }));

  EchoResponse response;
  ::grpc::ClientContext context;
  std::unique_ptr<::grpc::ClientWriter<EchoRequest>> writer(
      echo_stub_->ClientStreamingEcho(&context, &response));

  for (int i = 0; i < client_write_entries; i++) {
    EchoRequest request;
    request.set_message(payload);
    if (!writer->Write(request)) {
      break;
    }
  }
  writer->WritesDone();
  EXPECT_TRUE(writer->Finish().ok());
  EXPECT_EQ(payload, response.message());
}

TEST_F(GrpcMockerTest, EchoServerStreaming) {
  int server_write_entries = 3;
  std::string payload = "server_streaming_echo";
  EXPECT_CALL(*mock_echo_service_, ServerStreamingEcho)
      .WillOnce(
          Invoke([payload, server_write_entries](
                     ::grpc::ServerContext* context, const EchoRequest* request,
                     ::grpc::ServerWriter<EchoResponse>* writer) {
            for (int i = 0; i < server_write_entries; i++) {
              EchoResponse response;
              response.set_message(payload);
              if (!writer->Write(response)) {
                break;
              }
            }
            return ::grpc::Status::OK;
          }));

  EchoRequest request;
  EchoResponse response;
  ::grpc::ClientContext context;
  request.set_message(payload);
  std::unique_ptr<::grpc::ClientReader<EchoResponse>> reader(
      echo_stub_->ServerStreamingEcho(&context, request));

  int response_count = 0;
  while (reader->Read(&response)) {
    response_count++;
    EXPECT_EQ(payload, response.message());
  }
  EXPECT_EQ(server_write_entries, response_count);
  EXPECT_TRUE(reader->Finish().ok());
  EXPECT_EQ(payload, response.message());
}

TEST_F(GrpcMockerTest, EchoBidiStreaming) {
  int client_write_entries = 5;
  int server_write_entries = 3;
  std::string payload = "bidi_streaming_echo";
  EXPECT_CALL(*mock_echo_service_, BidiStreamingEcho)
      .WillOnce(Invoke(
          [payload, client_write_entries, server_write_entries](
              ::grpc::ServerContext* context,
              ::grpc::ServerReaderWriter<EchoResponse, EchoRequest>* stream) {
            EchoRequest request;
            int request_count = 0;
            while (stream->Read(&request)) {
              request_count++;
              EXPECT_EQ(payload, request.message());
            }
            EXPECT_EQ(client_write_entries, request_count);
            for (int i = 0; i < server_write_entries; i++) {
              EchoResponse response;
              response.set_message(payload);
              stream->Write(response);
            }
            return ::grpc::Status::OK;
          }));

  ::grpc::ClientContext context;
  std::shared_ptr<::grpc::ClientReaderWriter<EchoRequest, EchoResponse>> stream(
      echo_stub_->BidiStreamingEcho(&context));

  std::thread writer([stream, client_write_entries, payload]() {
    for (int i = 0; i < client_write_entries; i++) {
      EchoRequest request;
      request.set_message(payload);
      stream->Write(request);
    }
    stream->WritesDone();
  });

  EchoResponse response;
  int response_count = 0;
  while (stream->Read(&response)) {
    response_count++;
    EXPECT_EQ(payload, response.message());
  }
  writer.join();
  EXPECT_EQ(server_write_entries, response_count);
  EXPECT_TRUE(stream->Finish().ok());
  EXPECT_EQ(payload, response.message());
}

// Test that the stub returned by NewStub can be used (unsuccessfully) after the
// server has been destroyed.
TEST_F(GrpcMockerTest, NewStub) {
  EchoRequest request;
  EchoResponse response;
  request.set_message("foobar");

  std::unique_ptr<EchoService::Stub> echo_stub;

  {
    MockGrpcServer<MockEchoService> mock_echo;
    EXPECT_CALL(*mock_echo.service(), Echo)
        .WillOnce(::testing::Return(::grpc::Status::OK));

    echo_stub = mock_echo.NewStub();

    ::grpc::ClientContext context;
    EXPECT_TRUE(echo_stub->Echo(&context, request, &response).ok());
  }

  // The server has now been destroyed.
  ::grpc::ClientContext context_echo;
  EXPECT_THAT(echo_stub->Echo(&context_echo, request, &response),
              ::testing::Not(::testing::status::IsOk()));
}

class GrpcMockerMultipleServicesTest : public ::testing::Test {
 protected:
  GrpcMockerMultipleServicesTest()
      : grpc_mocker_(),
        mock_echo_service_(grpc_mocker_.service<MockEchoService>()),
        mock_foo_service_(grpc_mocker_.service<MockFooService>()),
        stub_echo_(grpc_mocker_.stub<EchoService>()),
        stub_foo_(grpc_mocker_.stub<FooService>()) {}

  MockMultipleServicesGrpcServer<MockEchoService, MockFooService> grpc_mocker_;
  MockEchoService* mock_echo_service_;
  MockFooService* mock_foo_service_;
  EchoService::Stub* stub_echo_;
  FooService::Stub* stub_foo_;
};

TEST_F(GrpcMockerMultipleServicesTest, EchoUnary) {
  EchoRequest request_echo;
  EchoResponse response_echo;
  std::string payload = "unary_echo";
  request_echo.set_message(payload);
  ::grpc::ClientContext context_echo;
  EXPECT_CALL(*mock_echo_service_, Echo)
      .WillOnce(Invoke([](::grpc::ServerContext* context,
                          const EchoRequest* request, EchoResponse* response) {
        response->set_message(request->message());
        return ::grpc::Status::OK;
      }));
  EXPECT_TRUE(stub_echo_->Echo(&context_echo, request_echo, &response_echo).ok());
  EXPECT_EQ(payload, response_echo.message());

  FooRequest request_foo;
  FooResponse response_foo;
  payload = "unary_foo";
  request_foo.set_arg(payload);
  ::grpc::ClientContext context_foo;

  EXPECT_CALL(*mock_foo_service_, Foo)
      .WillOnce(Invoke([](::grpc::ServerContext* context,
                          const FooRequest* request, FooResponse* response) {
        response->set_arg(request->arg());
        return ::grpc::Status::OK;
      }));
  EXPECT_TRUE(stub_foo_->Foo(&context_foo, request_foo, &response_foo).ok());
  EXPECT_EQ(payload, response_foo.arg());
}

// Test that the stub returned by NewStub can be used (unsuccessfully) after the
// server has been destroyed.
TEST_F(GrpcMockerMultipleServicesTest, NewStub) {
  std::unique_ptr<EchoService::Stub> echo_stub;
  EchoRequest request_echo;
  EchoResponse response_echo;

  {
    MockMultipleServicesGrpcServer<MockEchoService> grpc_mocker;
    EXPECT_CALL(*grpc_mocker.service<MockEchoService>(), Echo)
        .WillOnce(::testing::Return(::grpc::Status::OK));

    echo_stub = grpc_mocker.NewStub<EchoService>();

    ::grpc::ClientContext context_echo;
    EXPECT_TRUE(echo_stub->Echo(&context_echo, request_echo, &response_echo).ok());
  }

  // The server has now been destroyed.
  ::grpc::ClientContext context_echo;
  EXPECT_THAT(echo_stub->Echo(&context_echo, request_echo, &response_echo),
              ::testing::Not(::testing::status::IsOk()));
}

}  // namespace
}  // namespace testing
}  // namespace visionai
