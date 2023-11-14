// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_CONTROL_GRPC_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_CONTROL_GRPC_CLIENT_H_

#include "google/cloud/visionai/v1/streaming_service.grpc.pb.h"
#include "google/cloud/visionai/v1/streaming_service.pb.h"
#include "absl/status/statusor.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"

namespace visionai {

// The StreamingControlGrpcClient is the bare control grpc client to the
// StreamingService.
//
// It deals with elementary grpc and AIP's API conventions for interacting with
// the StreamingService while deferring service specific details to the caller.
class StreamingControlGrpcClient {
 public:
  // Options for specifying the connection to the Streaming service.
  struct Options {
    // The endpoint of streams service.
    //
    // This may be a DNS name or a (ip:port).
    std::string target_address;

    // Options to configure the RPC connection.
    ConnectionOptions connection_options;
  };

  // Create a ready to use client.
  //
  // Use this to get new instances rather than using constructors.
  static absl::StatusOr<std::unique_ptr<StreamingControlGrpcClient>> Create(
      const Options &options);

  // --------------------------------------------------------------------------
  // RPC wrappers for managing Leases
  // --------------------------------------------------------------------------

  // Acquire a writer's Lease.
  absl::StatusOr<google::cloud::visionai::v1::Lease> AcquireWritersLease(
      const std::string &channel_name, const std::string &lessee,
      absl::Duration term);

  // Acquire a reader's Lease.
  absl::StatusOr<google::cloud::visionai::v1::Lease> AcquireReadersLease(
      const std::string &channel_name, const std::string &lessee,
      absl::Duration term);

  // Renew a Lease.
  absl::StatusOr<google::cloud::visionai::v1::Lease> RenewLease(
      const std::string &lease_id, const std::string &channel_name,
      const std::string &lessee, absl::Duration term);

  // Release a Lease.
  absl::StatusOr<google::cloud::visionai::v1::ReleaseLeaseResponse>
  ReleaseLease(const std::string &lease_id, const std::string &channel_name,
               const std::string &lessee);

  // StreamingControlGrpcClient is neither copyable nor movable.
  StreamingControlGrpcClient(const StreamingControlGrpcClient &) = delete;
  StreamingControlGrpcClient &operator=(const StreamingControlGrpcClient &) =
      delete;
  virtual ~StreamingControlGrpcClient() = default;

 private:
  StreamingControlGrpcClient() = default;
  Options options_;
  std::unique_ptr<google::cloud::visionai::v1::StreamingService::Stub>
      streams_stub_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_CONTROL_GRPC_CLIENT_H_
