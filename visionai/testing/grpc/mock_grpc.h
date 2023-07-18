// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_TESTING_GRPC_MOCK_GRPC_H_
#define THIRD_PARTY_VISIONAI_TESTING_GRPC_MOCK_GRPC_H_

#include <memory>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "absl/strings/str_format.h"
#include "include/grpcpp/create_channel.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/security/credentials.h"
#include "include/grpcpp/security/server_credentials.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"
#include "visionai/util/tuple/for_each.h"

namespace visionai {
namespace testing {

// A library for easily creating the gRPC server and stubs with the mock
// service.
template <typename ServiceImpl>
class MockGrpcServer {
 public:
  using ServiceType = typename ServiceImpl::ServiceType;
  using ServiceStub = typename ServiceType::Stub;

  // Initializes the gRPC server.
  MockGrpcServer() : server_(BuildServer()), stub_(NewStub()) {}

  ~MockGrpcServer() {
    server_->Shutdown();
    server_->Wait();
  }

  // Provides access to the listening server address for users that choose to
  // create their own channels/stubs using LOCAL_TCP credentials.
  const std::string& local_credentials_server_address() const {
    return local_server_address_;
  }

  // Provides access to the mock service address for setting EXPECT_CALL
  // expectations on the mock.
  ServiceImpl* service() { return &service_; }

  // Provides access to a stub which is connected to the mock server using local
  // credentials. The pointer's lifetime is tied to this MockGrpcServer
  // instance.
  ServiceStub* stub() { return stub_.get(); }

  // Creates a stub that communicates with the mock server using local
  // credentials.
  std::unique_ptr<ServiceStub> NewStub() {
    return ServiceType::NewStub(::grpc::CreateChannel(
        local_server_address_,
        ::grpc::experimental::LocalCredentials(LOCAL_TCP)));
  }

 private:
  std::unique_ptr<::grpc::Server> BuildServer() {
    ::grpc::ServerBuilder builder;
    int port;
    auto server =
        builder
            .AddListeningPort(
                "localhost:0",
                ::grpc::experimental::LocalServerCredentials(LOCAL_TCP), &port)
            .RegisterService(&service_)
            .BuildAndStart();
    local_server_address_ = absl::StrFormat("localhost:%d", port);
    return server;
  }

  // This server address is suitable for communicating with the server via
  // local credentials.
  std::string local_server_address_;

  ServiceImpl service_;
  const std::unique_ptr<::grpc::Server> server_;
  const std::unique_ptr<ServiceStub> stub_;
};

template <class... ServicesImpl>
class MockMultipleServicesGrpcServer {
 public:
  // Initializes the gRPC server with the given LOAS credentials and also local
  // credentials.
  MockMultipleServicesGrpcServer()
      : server_(BuildServer()), stubs_(CreateStubs()) {}

  ~MockMultipleServicesGrpcServer() {
    server_->Shutdown();
    server_->Wait();
  }

  // Provides access to the listening server address for users that choose to
  // create their own channels/stubs using LOCAL_TCP credentials.
  const std::string& local_credentials_server_address() const {
    return local_server_address_;
  }

  // Provides access to the mock service address for setting EXPECT_CALL
  // expectations on the mock.
  template <class Service>
  Service* service() {
    return &std::get<Service>(services_);
  }

  // Provides access to a stub which is connected to the mock server. The
  // pointer's lifetime is tied to this MockGrpcServer instance.
  template <class Service>
  typename Service::Stub* stub() {
    return std::get<std::unique_ptr<typename Service::Stub>>(stubs_).get();
  }

  // Creates a stub that communicates with the specified mock service.
  template <class Service,
            typename = typename std::enable_if<absl::disjunction<std::is_same<
                Service, typename ServicesImpl::ServiceType>...>::value>::type>
  std::unique_ptr<typename Service::Stub> NewStub() {
    return Service::NewStub(::grpc::CreateChannel(
        local_server_address_,
        ::grpc::experimental::LocalCredentials(LOCAL_TCP)));
  }

 private:
  using ServiceTuple = std::tuple<ServicesImpl...>;
  using StubsTuple =
      std::tuple<std::unique_ptr<typename ServicesImpl::ServiceType::Stub>...>;

  std::unique_ptr<::grpc::Server> BuildServer() {
    ::grpc::ServerBuilder builder;
    visionai::for_each(
        [&builder](::grpc::Service& s) { builder.RegisterService(&s); },
        services_);
    int port;
    auto server =
        builder
            .AddListeningPort(
                "localhost:0",
                ::grpc::experimental::LocalServerCredentials(LOCAL_TCP), &port)
            .BuildAndStart();
    local_server_address_ = absl::StrFormat("localhost:%d", port);
    return server;
  }

  StubsTuple CreateStubs() {
    return std::make_tuple(NewStub<typename ServicesImpl::ServiceType>()...);
  }

  // This server address is suitable for communicating with the server via
  // local credentials.
  std::string local_server_address_;

  ServiceTuple services_;
  const std::unique_ptr<::grpc::Server> server_;
  const StubsTuple stubs_;
};

#define GRPC_UNARY_MOCK(method, request, response) \
  MOCK_METHOD(::grpc::Status, method,              \
              (::grpc::ServerContext*, const request*, response*))

#define GRPC_CLIENT_STREAMING_MOCK(method, request, response) \
  MOCK_METHOD(                                                \
      ::grpc::Status, method,                                 \
      (::grpc::ServerContext*, ::grpc::ServerReader<request>*, response*))

#define GRPC_SERVER_STREAMING_MOCK(method, request, response) \
  MOCK_METHOD(::grpc::Status, method,                         \
              (::grpc::ServerContext*, const request*,        \
               ::grpc::ServerWriter<response>*))

#define GRPC_BIDI_STREAMING_MOCK(method, request, response) \
  MOCK_METHOD(::grpc::Status, method,                       \
              (::grpc::ServerContext*,                      \
               (::grpc::ServerReaderWriter<response, request>*)))

}  // namespace testing
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TESTING_GRPC_MOCK_GRPC_H_
