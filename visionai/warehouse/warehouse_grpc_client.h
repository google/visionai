/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_WAREHOUSE_WAREHOUSE_GRPC_CLIENT_H_
#define VISIONAI_WAREHOUSE_WAREHOUSE_GRPC_CLIENT_H_

#include "absl/status/status.h"
#include "absl/time/clock.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/util/net/grpc/status_util.h"

namespace visionai {

// Media Warehouse GRPC client library.
// This file mirrors Media Warehouse's grpc client
// google3/cloud/ai/mediawarehouse/client/grpc/grpc_client.h
//
// TODO: port all API methods.
// TODO: Uncomment the google3 only annotations after the API launch.
template <class Service>
class WarehouseGrpcClient {
 public:
  WarehouseGrpcClient(int32_t grpc_deadline_seconds);
  ~WarehouseGrpcClient() = default;

  // WarehouseGrpcClient is neither copyable nor movable.
  WarehouseGrpcClient(const WarehouseGrpcClient&) = delete;
  WarehouseGrpcClient& operator=(const WarehouseGrpcClient&) = delete;

  // Initializes a <Service> stub with the given channel.
  // Does not retain ownership of the stub.
  void InitializeStubWithChannel(std::shared_ptr<grpc::Channel> channel);

  // Sets the service stub. Takes ownership of stub.
  void SetStub(std::unique_ptr<typename Service::StubInterface> stub);

  // Sends a Annotation request to the server. Does not take ownership of the
  // argument.
  template <class CreateAnnotationRequest, class Annotation>
  absl::Status CreateAnnotation(const CreateAnnotationRequest& req,
                                Annotation* resp);

  // Sends a Annotation request to the server. Does not take ownership of the
  // argument.
  template <class CreateAnnotationRequest, class Annotation>
  absl::Status CreateAnnotation(typename Service::StubInterface* stub,
                                const CreateAnnotationRequest& req,
                                Annotation* resp);

  // Sends a CreateAsset request to the server. Does not take
  // ownership of the arguments.
  template <class CreateAssetRequest, class Asset>
  absl::Status CreateAsset(const CreateAssetRequest& req, Asset* resp);

  // Sends a CreateAsset request to the server. Does not take
  // ownership of the arguments.
  template <class CreateAssetRequest, class Asset>
  absl::Status CreateAsset(typename Service::StubInterface* stub,
                           const CreateAssetRequest& req, Asset* resp);

  // Sends a ClipAsset request to the server. Does not take
  // ownership of the arguments.
  template <class ClipAssetRequest, class ClipAssetResponse>
  absl::Status ClipAsset(const ClipAssetRequest& req, ClipAssetResponse* resp);

  // Sends a ClipAsset request to the server. Does not take
  // ownership of the arguments.
  template <class ClipAssetRequest, class ClipAssetResponse>
  absl::Status ClipAsset(typename Service::StubInterface* stub,
                         const ClipAssetRequest& req, ClipAssetResponse* resp);

 private:
  // Service stub.
  std::unique_ptr<typename Service::StubInterface> stub_;
  int32_t grpc_deadline_seconds_ = 60;
};

template <class Service>
WarehouseGrpcClient<Service>::WarehouseGrpcClient(int32_t grpc_deadline_seconds)
    : grpc_deadline_seconds_(grpc_deadline_seconds) {}

template <class Service>
void WarehouseGrpcClient<Service>::InitializeStubWithChannel(
    std::shared_ptr<grpc::Channel> channel) {
  SetStub(Service::NewStub(channel));
}

template <class Service>
void WarehouseGrpcClient<Service>::SetStub(
    std::unique_ptr<typename Service::StubInterface> stub) {
  stub_ = std::move(stub);
}

template <class Service>
template <class CreateAnnotationRequest, class Annotation>
absl::Status WarehouseGrpcClient<Service>::CreateAnnotation(
    const CreateAnnotationRequest& req, Annotation* resp) {
  return CreateAnnotation(stub_.get(), req, resp);
}

template <class Service>
template <class CreateAnnotationRequest, class Annotation>
absl::Status WarehouseGrpcClient<Service>::CreateAnnotation(
    typename Service::StubInterface* stub, const CreateAnnotationRequest& req,
    Annotation* resp) {
  if (stub == nullptr) {
    return absl::InvalidArgumentError("stub cannot be null");
  }
  grpc::ClientContext context;
  context.set_deadline(
      std::chrono::system_clock::now() +
      absl::ToChronoSeconds(absl::Seconds(grpc_deadline_seconds_)));
  return visionai::ToAbseilStatus(stub->CreateAnnotation(&context, req, resp));
}

template <class Service>
template <class CreateAssetRequest, class Asset>
absl::Status WarehouseGrpcClient<Service>::CreateAsset(
    const CreateAssetRequest& req, Asset* resp) {
  return CreateAsset(stub_.get(), req, resp);
}

template <class Service>
template <class CreateAssetRequest, class Asset>
absl::Status WarehouseGrpcClient<Service>::CreateAsset(
    typename Service::StubInterface* stub, const CreateAssetRequest& req,
    Asset* resp) {
  if (stub == nullptr) {
    return absl::InvalidArgumentError("stub cannot be null.");
  }
  if (req.parent().empty()) {
    return absl::InvalidArgumentError("`parent` field is required.");
  }
  grpc::ClientContext context;
  context.set_deadline(
      std::chrono::system_clock::now() +
      absl::ToChronoSeconds(absl::Seconds(grpc_deadline_seconds_)));
  return ToAbseilStatus(stub->CreateAsset(&context, req, resp));
}

template <class Service>
template <class ClipAssetRequest, class ClipAssetResponse>
absl::Status WarehouseGrpcClient<Service>::ClipAsset(
    const ClipAssetRequest& req, ClipAssetResponse* resp) {
  return ClipAsset(stub_.get(), req, resp);
}

template <class Service>
template <class ClipAssetRequest, class ClipAssetResponse>
absl::Status WarehouseGrpcClient<Service>::ClipAsset(
    typename Service::StubInterface* stub, const ClipAssetRequest& req,
    ClipAssetResponse* resp) {
  if (stub == nullptr) {
    return absl::InvalidArgumentError("stub cannot be null.");
  }
  if (req.name().empty()) {
    return absl::InvalidArgumentError("Asset `name` field is required.");
  }
  grpc::ClientContext context;
  context.set_deadline(
      std::chrono::system_clock::now() +
      absl::ToChronoSeconds(absl::Seconds(grpc_deadline_seconds_)));
  return ToAbseilStatus(stub->ClipAsset(&context, req, resp));
}

}  // namespace visionai

#endif  // VISIONAI_WAREHOUSE_WAREHOUSE_GRPC_CLIENT_H_
