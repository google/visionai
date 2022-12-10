// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_control_grpc_client.h"

#include <string>

#include "glog/logging.h"
#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
using ::google::cloud::visionai::v1::AcquireLeaseRequest;
using ::google::cloud::visionai::v1::Lease;
using ::google::cloud::visionai::v1::ReleaseLeaseRequest;
using ::google::cloud::visionai::v1::ReleaseLeaseResponse;
using ::google::cloud::visionai::v1::RenewLeaseRequest;
}  // namespace

absl::StatusOr<std::unique_ptr<StreamingControlGrpcClient>>
StreamingControlGrpcClient::Create(const Options &options) {
  auto client = absl::WrapUnique(new StreamingControlGrpcClient());
  client->options_ = options;
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      client->options_.target_address, client->options_.connection_options))
      << "while configuring authorization information";
  auto channel = CreateChannel(client->options_.target_address,
                               client->options_.connection_options);
  client->streams_stub_ =
      google::cloud::visionai::v1::StreamingService::NewStub(channel);
  return std::move(client);
}

absl::StatusOr<Lease> StreamingControlGrpcClient::AcquireWritersLease(
    const std::string &channel_name, const std::string &lessee,
    absl::Duration term) {
  auto context = CreateClientContext(options_.connection_options);
  AcquireLeaseRequest req;
  req.set_lease_type(google::cloud::visionai::v1::LEASE_TYPE_WRITER);
  req.set_owner(lessee);
  req.set_series(channel_name);
  *req.mutable_term() = ToProtoDuration(term);
  Lease resp;
  VAI_RETURN_IF_ERROR(streams_stub_->AcquireLease(context.get(), req, &resp));
  return resp;
}

absl::StatusOr<Lease> StreamingControlGrpcClient::AcquireReadersLease(
    const std::string &channel_name, const std::string &lessee,
    absl::Duration term) {
  auto context = CreateClientContext(options_.connection_options);
  AcquireLeaseRequest req;
  req.set_lease_type(google::cloud::visionai::v1::LEASE_TYPE_READER);
  req.set_owner(lessee);
  req.set_series(channel_name);
  *req.mutable_term() = ToProtoDuration(term);
  Lease resp;
  VAI_RETURN_IF_ERROR(streams_stub_->AcquireLease(context.get(), req, &resp));
  return resp;
}

absl::StatusOr<Lease> StreamingControlGrpcClient::RenewLease(
    const std::string &lease_id, const std::string &channel_name,
    const std::string &lessee, absl::Duration term) {
  auto context = CreateClientContext(options_.connection_options);
  RenewLeaseRequest req;
  req.set_owner(lessee);
  req.set_series(channel_name);
  req.set_id(lease_id);
  *req.mutable_term() = ToProtoDuration(term);
  Lease resp;
  VAI_RETURN_IF_ERROR(streams_stub_->RenewLease(context.get(), req, &resp));
  return resp;
}

absl::StatusOr<ReleaseLeaseResponse> StreamingControlGrpcClient::ReleaseLease(
    const std::string &lease_id, const std::string &channel_name,
    const std::string &lessee) {
  auto context = CreateClientContext(options_.connection_options);
  ReleaseLeaseRequest req;
  req.set_owner(lessee);
  req.set_series(channel_name);
  req.set_id(lease_id);
  ReleaseLeaseResponse resp;
  VAI_RETURN_IF_ERROR(streams_stub_->ReleaseLease(context.get(), req, &resp));
  return resp;
}

}  // namespace visionai
