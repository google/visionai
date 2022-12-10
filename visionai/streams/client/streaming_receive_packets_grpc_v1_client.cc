// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_receive_packets_grpc_v1_client.h"

#include <thread>
#include <utility>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

using google::cloud::visionai::v1::ReceivePacketsRequest;
using google::cloud::visionai::v1::ReceivePacketsResponse;
using google::cloud::visionai::v1::RequestMetadata;
using google::cloud::visionai::v1::StreamingService;

absl::StatusOr<RequestMetadata> MakeRequestMetadata(
    const StreamingReceivePacketsGrpcV1Client::Options& options) {
  RequestMetadata request_metadata;

  // Set the stream.
  if (options.cluster_name.empty()) {
    return absl::InvalidArgumentError("Given an empty `cluster_name`");
  }
  if (options.channel.stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty `stream_id`");
  }
  VAI_ASSIGN_OR_RETURN(
      auto stream_name,
      MakeStreamName(options.cluster_name, options.channel.stream_id),
      _ << "while assembling stream name.");
  request_metadata.set_stream(stream_name);

  // Set the event.
  if (options.channel.event_id.empty()) {
    return absl::InvalidArgumentError("Given an empty `event_id`");
  }
  VAI_ASSIGN_OR_RETURN(
      auto event_name,
      MakeEventName(options.cluster_name, options.channel.event_id),
      _ << "while assembling event name.");
  request_metadata.set_event(event_name);

  // Set the series.
  VAI_ASSIGN_OR_RETURN(
      auto channel_id,
      MakeChannelId(options.channel.event_id, options.channel.stream_id),
      _ << "while assembling channel id");
  VAI_ASSIGN_OR_RETURN(auto channel_name,
                   MakeChannelName(options.cluster_name, channel_id),
                   _ << "while assembling channel name");
  request_metadata.set_series(channel_name);

  if (options.receiver.empty()) {
    return absl::InvalidArgumentError(
        "Given an empty `receiver` for the lease owner.");
  }
  request_metadata.set_owner(options.receiver);
  *request_metadata.mutable_lease_term() = ToProtoDuration(options.lease_term);

  return request_metadata;
}

}  // namespace

StreamingReceivePacketsGrpcV1Client::StreamingReceivePacketsGrpcV1Client(
    const Options& options)
    : options_(options) {}

bool StreamingReceivePacketsGrpcV1Client::IsRpcOpen() {
  return ctx_ != nullptr && stream_ != nullptr;
}

absl::Status StreamingReceivePacketsGrpcV1Client::PrepareRpcStubs() {
  if (options_.target_address.empty()) {
    return absl::InvalidArgumentError("Given an empty `target_address`.");
  }
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      options_.target_address, options_.advanced.connection_options))
      << "while configuring authorization information";
  stub_ = StreamingService::NewStub(CreateChannel(
      options_.target_address, options_.advanced.connection_options));
  ctx_ = CreateClientContext(options_.advanced.connection_options);
  return absl::OkStatus();
}

absl::Status StreamingReceivePacketsGrpcV1Client::OpenRpc() {
  if (IsRpcOpen()) {
    return absl::FailedPreconditionError("The RPC is already open.");
  }
  VAI_RETURN_IF_ERROR(PrepareRpcStubs()) << "while preparing RPC stubs";
  stream_ = stub_->ReceivePackets(ctx_.get());

  // Defensive check.
  if (stream_ == nullptr) {
    return absl::UnknownError("The ClientReaderWriter is nullptr.");
  }

  return absl::OkStatus();
}

absl::Status StreamingReceivePacketsGrpcV1Client::WriteSetupMessage() {
  ReceivePacketsRequest request;

  // Set the `request_metadata`.
  VAI_ASSIGN_OR_RETURN(*request.mutable_setup_request()->mutable_metadata(),
                   MakeRequestMetadata(options_),
                   _ << "while assembling the `RequestMetadata`");

  // Set receiver identity.
  if (options_.receiver.empty()) {
    return absl::InvalidArgumentError("Given an empty `receiver`.");
  }
  request.mutable_setup_request()->set_receiver(options_.receiver);

  // Set the receive mode.
  if (options_.receive_mode == "eager") {
    request.mutable_setup_request()->mutable_eager_receive_mode();
  } else if (options_.receive_mode == "controlled") {
    request.mutable_setup_request()
        ->mutable_controlled_receive_mode()
        ->set_starting_logical_offset(
            options_.advanced.controlled_mode_options.starting_logical_offset);
    request.mutable_setup_request()
        ->mutable_controlled_receive_mode()
        ->set_fallback_starting_offset(
            options_.advanced.controlled_mode_options.fallback_starting_offset);
  } else {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Given an unknown receive mode \"%s\".", options_.receive_mode));
  }

  // Set the heartbeat interval expected of the server.
  *request.mutable_setup_request()->mutable_heartbeat_interval() =
      ToProtoDuration(kDefaultServerHeartbeatInterval);
  if (options_.advanced.heartbeat_interval > absl::ZeroDuration() &&
      options_.advanced.heartbeat_interval < absl::InfiniteDuration()) {
    *request.mutable_setup_request()->mutable_heartbeat_interval() =
        ToProtoDuration(options_.advanced.heartbeat_interval);
  }

  // Set the writes done grace period if given.
  if (options_.advanced.writes_done_grace_period > absl::ZeroDuration() &&
      options_.advanced.writes_done_grace_period < absl::InfiniteDuration()) {
    *request.mutable_setup_request()->mutable_writes_done_grace_period() =
        ToProtoDuration(options_.advanced.writes_done_grace_period);
  }

  // Write the first message, which is the setup request.
  //
  // If this fails at this stage, then signal this to the caller to hard destroy
  // and start over.
  if (!stream_->Write(request)) {
    return absl::UnknownError(
        "Failed to `Write` the setup request. Please destroy this instance and "
        "try again.");
  }

  return absl::OkStatus();
}

absl::Status StreamingReceivePacketsGrpcV1Client::Initialize() {
  VAI_RETURN_IF_ERROR(OpenRpc()) << "while opening the RPC";
  VAI_RETURN_IF_ERROR(WriteSetupMessage()) << "while sending the setup message";
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<StreamingReceivePacketsGrpcV1Client>>
StreamingReceivePacketsGrpcV1Client::Create(const Options& options) {
  auto packet_receiver =
      absl::WrapUnique(new StreamingReceivePacketsGrpcV1Client(options));
  VAI_RETURN_IF_ERROR(packet_receiver->Initialize());
  return std::move(packet_receiver);
}

bool StreamingReceivePacketsGrpcV1Client::Read(
    ReceivePacketsResponse* response) {
  if (IsReadStreamTerminated()) {
    return false;
  } else {
    if (!stream_->Read(response)) {
      SetReadStreamTerminated(true);
      return false;
    } else {
      return true;
    }
  }
}

bool StreamingReceivePacketsGrpcV1Client::WriteCommit(int64_t offset) {
  ReceivePacketsRequest request;
  request.mutable_commit_request()->set_offset(offset);
  if (!stream_->Write(request)) {
    SetWriteStreamTerminated(true);
    return false;
  } else {
    return true;
  }
}

bool StreamingReceivePacketsGrpcV1Client::WritesDone() {
  if (!IsWriteStreamTerminated()) {
    writes_done_result_ = stream_->WritesDone();
    SetWriteStreamTerminated(true);
  }
  return writes_done_result_;
}

void StreamingReceivePacketsGrpcV1Client::Cancel() {
  if (IsRpcOpen()) {
    ctx_->TryCancel();
    SetReadStreamTerminated(true);
    SetWriteStreamTerminated(true);
  }
}

absl::Status StreamingReceivePacketsGrpcV1Client::Finish(
    absl::Status* rpc_status) {
  if (!IsRpcOpen()) {
    return absl::FailedPreconditionError("The RPC is already finished.");
  }

  if (rpc_status == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to `rpc_status`");
  }

  if (!IsWriteStreamTerminated()) {
    return absl::FailedPreconditionError(
        "The write channel is still open. Was `WritesDone` called?");
  }

  if (!IsReadStreamTerminated()) {
    return absl::FailedPreconditionError(
        "The read channel is still open. Was `Read` called until `false` "
        "was returned?");
  }

  *rpc_status = ToAbseilStatus(stream_->Finish());
  stream_.reset();

  return absl::OkStatus();
}

StreamingReceivePacketsGrpcV1Client::~StreamingReceivePacketsGrpcV1Client() {
  if (IsRpcOpen()) {
    // If either channel is hasn't been closed, then forcefully cancel.
    if (!IsReadStreamTerminated() || !IsWriteStreamTerminated()) {
      Cancel();
    }

    // Get the final RPC status and log if there is an abnormality.
    absl::Status rpc_status;
    auto status = Finish(&rpc_status);
    if (!status.ok()) {
      LOG(ERROR) << "Failed to `Finish` the RPC: " << status;
    } else {
      if (!rpc_status.ok()) {
        LOG(INFO) << "Got a non-OK final RPC status: " << rpc_status;
      }
    }
  }
}

}  // namespace visionai
