/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_WAREHOUSE_WAREHOUSE_STREAMING_GRPC_CLIENT_H_
#define VISIONAI_WAREHOUSE_WAREHOUSE_STREAMING_GRPC_CLIENT_H_

#include <memory>

#include "absl/status/status.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"

namespace visionai {

template <class Service, class Request, class Response>
class WarehouseStreamingGrpcClient {
 public:
  WarehouseStreamingGrpcClient(
      ::visionai::ConnectionOptions
          connection_options);
  virtual ~WarehouseStreamingGrpcClient() = default;
  WarehouseStreamingGrpcClient(const WarehouseStreamingGrpcClient&) = delete;
  WarehouseStreamingGrpcClient& operator=(const WarehouseStreamingGrpcClient&) =
      delete;

  // Initializes a <Service> stub with the given channel.
  // Does not retain ownership of the stub.
  void Initialize(std::shared_ptr<grpc::Channel> channel);

  // Reads a response from the stream.
  absl::Status GetResponse(Response* resp);

  // Writes request to the stream.
  absl::Status SendRequest(const Request& req);

  // Closes the stream.
  absl::Status Finish();

  // Tells backend not to expect any more writes.
  void WritesDone() { stream_->WritesDone(); }

 private:
  std::unique_ptr<typename Service::Stub> stub_ = nullptr;
  std::unique_ptr<grpc::ClientContext> ctx_ = nullptr;
  std::unique_ptr<grpc::ClientReaderWriter<Request, Response>> stream_ =
      nullptr;
};

template <class Service, class Request, class Response>
WarehouseStreamingGrpcClient<Service, Request, Response>::
    WarehouseStreamingGrpcClient(
        ::visionai::ConnectionOptions
            connection_options) {
  ctx_ = visionai::CreateClientContext(connection_options);
}

template <class Service, class Request, class Response>
void WarehouseStreamingGrpcClient<Service, Request, Response>::Initialize(
    std::shared_ptr<grpc::Channel> channel) {
  stub_ = Service::NewStub(channel);
  stream_ = stub_->IngestAsset(ctx_.get());
}

template <class Service, class Request, class Response>
absl::Status
WarehouseStreamingGrpcClient<Service, Request, Response>::GetResponse(
    Response* resp) {
  if (!stream_->Read(resp)) {
    return absl::InternalError("Stream read operation failed.");
  }
  return absl::OkStatus();
}

template <class Service, class Request, class Response>
absl::Status
WarehouseStreamingGrpcClient<Service, Request, Response>::SendRequest(
    const Request& req) {
  if (!stream_->Write(req)) {
    return absl::InternalError("Stream write operation failed.");
  }
  return absl::OkStatus();
}

template <class Service, class Request, class Response>
absl::Status
WarehouseStreamingGrpcClient<Service, Request, Response>::Finish() {
  return visionai::ToAbseilStatus(stream_->Finish());
}

}  // namespace visionai

#endif  // VISIONAI_WAREHOUSE_WAREHOUSE_STREAMING_GRPC_CLIENT_H_
