// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_SEND_PACKETS_GRPC_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_SEND_PACKETS_GRPC_CLIENT_H_

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streaming_service.grpc.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/notification.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {

// The StreamingSendPacketsGrpcClient is the bare data plane grpc client that
// sends packets to a Channel in the StreamingService.
//
// It assumes that the destination channel is already prepared and all it
// does is handle the packet bussing.
//
// Each instance of StreamingSendPacketsGrpcClient corresponds to one
// SendPackets bidi streaming grpc call.
//
// Preconditions
// -------------
//
// 1. A target dataplane endpoint of the StreamingService must be specified.
//    This is usually available after the user has a cluster is provisioned.
// 2. A channel is already prepared and ready for writing.
//
// Usage Pattern
// -------------
//
// 1. Create an instance of StreamingSendPacketsGrpcClient.
// 2. Repeatedly call Send() until either:
//    a. There is nothing more to send.
//    b. When a non-OK status is returned.
// 3. Destroy the instance of StreamingSendPacketsGrpcClient.
//
// Return Codes
// ------------
//
// In the case that the final Send() call returns non-OK, it may be
// desirable to reconnect and continue sending; e.g. perhaps the network
// temporarily partititioned and might recover fairly quickly. However, not
// all return statuses make sense to be retried and could indicate that the
// destination channel is permanently closed for sending. Here are the
// possible return codes of Send(), what they mean, and whether a subsequent
// reconnection makes sense:
//
// 1. OK (reconnect-yes)
//    This is the normal condition.
//
// 2. CANCELLED (reconnect-yes)
//    This is a cancelled call, often due to local networking issues.
//
// 3. DEADLINE_EXCEEDED (reconnect-yes)
//    This is a server message to force the client to reconnected;
//    e.g. intentionally severing long running rpcs.
//
// 4. UNAVAILABLE (reconnect-yes)
//    This is a temporary server condition, and it indicates that things
//    will become available soon.
//
// 5. FAILED_PRECONDITION (reconnect-maybe)
//    This indicates that a pre-requisite wasn't satisfied. Until that is
//    resolved, it doesn't make sense to reconnect.
//
// 6. Other codes (reconnect-no)
//    These are probably true errors and are considered non-recoverable.
class StreamingSendPacketsGrpcClient {
 public:
  // Options for configuring the packet sender.
  struct Options {
    // The data plane endpoint that hosts the series services.
    //
    // This may be a DNS name or a (ip:port).
    std::string target_address;

    // The cluster resource name.
    std::string cluster_name;

    // The channel to connect to.
    Channel channel;

    // The sender name.
    std::string sender;

    // The maximum duration that the `sender` may be disconnected from the
    // `channel`, after which the server considers the sending be complete.
    absl::Duration lease_term = absl::ZeroDuration();

    // Options to configure the RPC connection.
    ConnectionOptions connection_options;
  };

  // Creates and initializes an instance that is ready for use.
  static absl::StatusOr<std::unique_ptr<StreamingSendPacketsGrpcClient>> Create(
      const Options&);

  // Send the given packet.
  //
  // Prefer to std::move the packet in if the payload is large.
  //
  // This can block the calling thread, but you may call `Cancel` from a
  // separate thread to unblock.
  virtual absl::Status Send(Packet packet);

  // Issue an out of band Cancel request.
  //
  // If the cancel is granted, the thread blocked on `Send` will unblock and
  // receive CANCEL.
  virtual void Cancel();

  virtual ~StreamingSendPacketsGrpcClient();

 protected:
  // Allows the instantiation of the inherited mock class.
  explicit StreamingSendPacketsGrpcClient(const Options&);

 private:
  Options options_;
  std::unique_ptr<google::cloud::visionai::v1::StreamingService::Stub>
      stub_ = nullptr;
  std::unique_ptr<grpc::ClientContext> ctx_ = nullptr;
  std::unique_ptr<grpc::ClientReaderWriter<
      google::cloud::visionai::v1::SendPacketsRequest,
      google::cloud::visionai::v1::SendPacketsResponse>>
      stream_ = nullptr;

  absl::Notification user_cancel_notification_;

  absl::Status ValidateOptions();
  absl::Status Initialize();
  absl::Status PrepareRpcStubs();
  absl::Status OpenRpc();
  absl::Status SendSetupMessage();
  absl::Status CloseRpc(absl::Status* rpc_status);
  absl::Status CloseRpcExpectingErrors();
  bool IsRpcOpen();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_SEND_PACKETS_GRPC_CLIENT_H_
