// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_send_packets_grpc_client.h"

#include <memory>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streaming_service.grpc.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

using google::cloud::visionai::v1::SendPacketsRequest;
using google::cloud::visionai::v1::SendPacketsResponse;
using google::cloud::visionai::v1::StreamingService;

}  // namespace

StreamingSendPacketsGrpcClient::StreamingSendPacketsGrpcClient(
    const Options& options)
    : options_(options) {}

absl::StatusOr<std::unique_ptr<StreamingSendPacketsGrpcClient>>
StreamingSendPacketsGrpcClient::Create(const Options& options) {
  auto streaming_send_packets_grpc_client =
      absl::WrapUnique(new StreamingSendPacketsGrpcClient(options));
  VAI_RETURN_IF_ERROR(streaming_send_packets_grpc_client->Initialize());
  return std::move(streaming_send_packets_grpc_client);
}

// The initialization goes through the following phases:
//
// 1. GRPC stub preparation.
//
//    Besides simply setting up the stub, it also configures the client context
//    to have the correct metadata (e.g. authorization headers, deadlines,
//    etc.).
//
// 2. Open the RPC.
//
//    This is simply making the streaming RPC call itself. From here on, the
//    bidi streaming is active and any custom protocols can now be performed
//    between the client and th server.
//
// 3. Send the setup message.
//
//    This is the control message that must be sent before any packets may
//    follow. It includes information such as the lease and the destination
//    channel information.
//
// After control returns to the caller, the instance is ready for callers to
// repeated issue Send(Packet) calls. We must also ensure that CloseRpc() is
// called on all possible code paths.
absl::Status StreamingSendPacketsGrpcClient::Initialize() {
  VAI_RETURN_IF_ERROR(ValidateOptions()) << "while validating options";
  VAI_RETURN_IF_ERROR(PrepareRpcStubs()) << "while preparing RPC stubs";
  VAI_RETURN_IF_ERROR(OpenRpc()) << "while opening the RPC";
  VAI_RETURN_IF_ERROR(SendSetupMessage()) << "while sending the setup message";
  return absl::OkStatus();
}

absl::Status StreamingSendPacketsGrpcClient::ValidateOptions() {
  if (options_.channel.stream_id.empty()) {
    return absl::InvalidArgumentError("The stream id cannot be empty.");
  }
  if (options_.channel.event_id.empty()) {
    return absl::InvalidArgumentError("The event id cannot be empty.");
  }
  if (options_.sender.empty()) {
    return absl::InvalidArgumentError("The sender cannot be empty.");
  }
  return absl::OkStatus();
}

absl::Status StreamingSendPacketsGrpcClient::PrepareRpcStubs() {
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      options_.target_address, options_.connection_options))
      << "while configuring authorization information";
  stub_ = StreamingService::NewStub(
      CreateChannel(options_.target_address, options_.connection_options));
  ctx_ = CreateClientContext(options_.connection_options);
  return absl::OkStatus();
}

absl::Status StreamingSendPacketsGrpcClient::OpenRpc() {
  if (IsRpcOpen()) {
    return absl::FailedPreconditionError("The RPC is already open.");
  }
  stream_ = stub_->SendPackets(ctx_.get());

  return absl::OkStatus();
}

absl::Status StreamingSendPacketsGrpcClient::SendSetupMessage() {
  SendPacketsRequest request;
  auto request_metadata = request.mutable_metadata();
  VAI_ASSIGN_OR_RETURN(
      auto stream_name,
      MakeStreamName(options_.cluster_name, options_.channel.stream_id),
      _ << "while assembling stream name.");
  VAI_ASSIGN_OR_RETURN(
      auto event_name,
      MakeEventName(options_.cluster_name, options_.channel.event_id),
      _ << "while assembling event name.");
  request_metadata->set_stream(stream_name);
  request_metadata->set_event(event_name);
  VAI_ASSIGN_OR_RETURN(
      auto channel_id,
      MakeChannelId(options_.channel.event_id, options_.channel.stream_id),
      _ << "while assembling channel id.");
  VAI_ASSIGN_OR_RETURN(auto channel_name,
                   MakeChannelName(options_.cluster_name, channel_id),
                   _ << "while assembling channel name.");
  request_metadata->set_series(channel_name);
  request_metadata->set_owner(options_.sender);
  *request_metadata->mutable_lease_term() =
      ToProtoDuration(options_.lease_term);
  if (!stream_->Write(request)) {
    return CloseRpcExpectingErrors();
  }
  return absl::OkStatus();
}

absl::Status StreamingSendPacketsGrpcClient::Send(Packet packet) {
  SendPacketsRequest request;
  *request.mutable_packet() = std::move(packet);
  if (!stream_->Write(request)) {
    return CloseRpcExpectingErrors();
  }
  return absl::OkStatus();
}

void StreamingSendPacketsGrpcClient::Cancel() {
  if (!user_cancel_notification_.HasBeenNotified()) {
    user_cancel_notification_.Notify();
  }
  if (IsRpcOpen()) {
    auto status = CloseRpcExpectingErrors();
  }
}

StreamingSendPacketsGrpcClient::~StreamingSendPacketsGrpcClient() {
  if (IsRpcOpen()) {
    absl::Status rpc_status;
    auto status = CloseRpc(&rpc_status);
    if (!status.ok()) {
      LOG(ERROR)
          << status
          << "while trying to CloseRpc(). Thus, no further details on the "
             "rpc status is available.";
      return;
    }
    if (!rpc_status.ok()) {
      LOG(ERROR) << rpc_status;
    }
    return;
  }
}

bool StreamingSendPacketsGrpcClient::IsRpcOpen() {
  return ctx_ != nullptr && stream_ != nullptr;
}

// This is a convenience function to CloseRpc() when the caller knows that an
// exceptional condition has occured; e.g. when Write() returns false.
absl::Status StreamingSendPacketsGrpcClient::CloseRpcExpectingErrors() {
  absl::Status rpc_status = absl::OkStatus();
  auto status = CloseRpc(&rpc_status);
  if (!status.ok()) {
    return absl::UnknownError(absl::StrFormat(
        "%s. A more detailed reason could not be determined because this is an "
        "error that occured during CloseRpc() itself.",
        status.message()));
  }

  if (rpc_status.ok()) {
    return absl::UnknownError(
        "The RPC was successfully closed and the server returned an OK status. "
        "Are you sure there was an error?");
  }

  return rpc_status;
}

// The counterpart to OpenRpc.
//
// On success, `rpc_status` contains the status of the RPC itself.
// On failure, `rpc_status` will not be modified, and the return status will
// tell why the rpc closing itself didn't succeed.
//
// Note that it is important to distinguish between the status of CloseRpc
// itself vs the status of the RPC, the latter of which can only be known if the
// former succeeded.
absl::Status StreamingSendPacketsGrpcClient::CloseRpc(
    absl::Status* rpc_status) {
  if (!IsRpcOpen()) {
    return absl::FailedPreconditionError("The RPC is already closed.");
  }

  if (!stream_->WritesDone()) {
    LOG(WARNING)
        << "WritesDone() has returned false; but this may not necessarily be a "
           "problem if the RPC was already cancelled or terminated.";
  }
  if (user_cancel_notification_.HasBeenNotified()) {
    ctx_->TryCancel();
  }

  // Since the server has gotten word that the client has completed its writes
  // (WritesDone returned true), it seems that the Read should eventually return
  // false. However, it is not clear how soon "eventually" is, especially if
  // there might be a network partition.
  //
  // TODO: Should this be run in a background thread and we construct a timeout
  // mechanism to be able to move on in the face of excessively long waits?
  SendPacketsResponse resp;
  while (stream_->Read(&resp)) {
    LOG(INFO) << "Reading final messages from the server";
  }

  *rpc_status = ToAbseilStatus(stream_->Finish());
  stream_.reset();
  return absl::OkStatus();
}

}  // namespace visionai
