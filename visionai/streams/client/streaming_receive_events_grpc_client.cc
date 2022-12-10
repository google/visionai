// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_receive_events_grpc_client.h"

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

using google::cloud::visionai::v1::ReceiveEventsRequest;
using google::cloud::visionai::v1::ReceiveEventsResponse;
using google::cloud::visionai::v1::StreamingService;

}  // namespace

StreamingReceiveEventsGrpcClient::StreamingReceiveEventsGrpcClient(
    const Options& options)
    : options_(options) {}

bool StreamingReceiveEventsGrpcClient::IsRpcOpen() {
  return ctx_ != nullptr && stream_ != nullptr;
}

absl::Status StreamingReceiveEventsGrpcClient::PrepareRpcStubs() {
  if (options_.target_address.empty()) {
    return absl::InvalidArgumentError("Given an empty `target_address`");
  }
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      options_.target_address, options_.advanced.connection_options))
      << "while configuring authorization information";
  stub_ = StreamingService::NewStub(CreateChannel(
      options_.target_address, options_.advanced.connection_options));
  ctx_ = CreateClientContext(options_.advanced.connection_options);
  return absl::OkStatus();
}

absl::Status StreamingReceiveEventsGrpcClient::OpenRpc() {
  if (IsRpcOpen()) {
    return absl::FailedPreconditionError("The RPC is already open.");
  }
  VAI_RETURN_IF_ERROR(PrepareRpcStubs()) << "while preparing RPC stubs";
  stream_ = stub_->ReceiveEvents(ctx_.get());

  // Defensive check.
  if (stream_ == nullptr) {
    return absl::UnknownError("The ClientReaderWriter is nullptr.");
  }

  return absl::OkStatus();
}

absl::Status StreamingReceiveEventsGrpcClient::WriteSetupMessage() {
  ReceiveEventsRequest request;

  // Set the cluster and stream names.
  if (options_.cluster_name.empty()) {
    return absl::InvalidArgumentError("Given an empty `cluster_name`");
  }
  request.mutable_setup_request()->set_cluster(options_.cluster_name);

  if (options_.stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty `stream_id`");
  }
  VAI_ASSIGN_OR_RETURN(auto stream_name,
                   MakeStreamName(options_.cluster_name, options_.stream_id),
                   _ << "while assembling the stream name.");
  request.mutable_setup_request()->set_stream(stream_name);

  // Set receiver identity.
  if (options_.receiver.empty()) {
    return absl::InvalidArgumentError("Given an empty `receiver`");
  }
  request.mutable_setup_request()->set_receiver(options_.receiver);

  // Set the controlled mode.
  if (options_.starting_logical_offset.empty()) {
    return absl::InvalidArgumentError(
        "Given an empty `starting_logical_offset`");
  }
  request.mutable_setup_request()
      ->mutable_controlled_mode()
      ->set_starting_logical_offset(options_.starting_logical_offset);

  if (options_.fallback_starting_offset.empty()) {
    return absl::InvalidArgumentError(
        "Given an empty `fallback_starting_offset`");
  }
  request.mutable_setup_request()
      ->mutable_controlled_mode()
      ->set_fallback_starting_offset(options_.fallback_starting_offset);

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

absl::Status StreamingReceiveEventsGrpcClient::Initialize() {
  VAI_RETURN_IF_ERROR(OpenRpc()) << "while opening the RPC";
  VAI_RETURN_IF_ERROR(WriteSetupMessage()) << "while sending the setup message";
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<StreamingReceiveEventsGrpcClient>>
StreamingReceiveEventsGrpcClient::Create(const Options& options) {
  auto client = absl::WrapUnique(new StreamingReceiveEventsGrpcClient(options));
  VAI_RETURN_IF_ERROR(client->Initialize());
  return std::move(client);
}

bool StreamingReceiveEventsGrpcClient::Read(ReceiveEventsResponse* response) {
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

bool StreamingReceiveEventsGrpcClient::WriteCommit(int64_t offset) {
  ReceiveEventsRequest request;
  request.mutable_commit_request()->set_offset(offset);
  if (!stream_->Write(request)) {
    SetWriteStreamTerminated(true);
    return false;
  } else {
    return true;
  }
}

bool StreamingReceiveEventsGrpcClient::WritesDone() {
  if (!IsWriteStreamTerminated()) {
    writes_done_result_ = stream_->WritesDone();
    SetWriteStreamTerminated(true);
  }
  return writes_done_result_;
}

void StreamingReceiveEventsGrpcClient::Cancel() {
  if (IsRpcOpen()) {
    ctx_->TryCancel();
    SetReadStreamTerminated(true);
    SetWriteStreamTerminated(true);
  }
}

absl::Status StreamingReceiveEventsGrpcClient::Finish(
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

StreamingReceiveEventsGrpcClient::~StreamingReceiveEventsGrpcClient() {
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
