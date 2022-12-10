// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/net/grpc/client_connect.h"

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/support/channel_arguments.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/net/grpc/auth_token.h"
#include "visionai/util/status/status_macros.h"
#include "visionai/util/time_util.h"

namespace visionai {

namespace {

using ::visionai::ConnectionOptions;

// SSL defaults.
constexpr bool kUseInsecureChannelDefault = false;

// Channel defaults.
constexpr int kMaxReceiveMessageSizeDefault = -1;
constexpr int kMaxSendMessageSizeDefault = -1;
constexpr int kKeepAlivePermitWithoutCallsDefault = 1;
constexpr int kHttp2MaxPingsWithoutDataDefault = 2;
constexpr absl::Duration kKeepAliveTime = absl::Seconds(20);
constexpr absl::Duration kKeepAliveTimeout = absl::Seconds(10);

// Client context defaults.
constexpr bool kWaitForReady = false;

// Time duration defaults.
//
// Currently, none are set. Add them here if needed.

// gRPC channel argument keys.
//
// See
// https://github.com/grpc/grpc/blob/master/include/grpc/impl/codegen/grpc_types.h
//
// as well as
// https://github.com/grpc/grpc/blob/master/doc/keepalive.md
constexpr char kGrpcHttp2MaxPingsWithoutDataArg[] =
    "GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA";
constexpr char kGrpcKeepalivePermitWithoutCallsArg[] =
    "GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS";
constexpr char kGrpcKeepaliveTimeArg[] = "GRPC_ARG_KEEPALIVE_TIME_MS";
constexpr char kGrpcKeepaliveTimeoutArg[] = "GRPC_ARG_KEEPALIVE_TIMEOUT_MS";
constexpr char kGrpcFixedReconnectBackoffMsArg[] =
    "grpc.testing.fixed_reconnect_backoff_ms";

constexpr char kGoogleApplicationCredentials[] =
    "GOOGLE_APPLICATION_CREDENTIALS";
constexpr char kAuthorizationHeader[] = "authorization";
constexpr char kVisionAIDataplaneDomainSuffix[] = ".visionai.goog";
constexpr char kOAuthToken[] = "VISIONAI_OAUTH_TOKEN";

// Helper for building channel arguments.
grpc::ChannelArguments ConstructChannelArguments(
    const ConnectionOptions& connection_options) {
  grpc::ChannelArguments channel_arguments;

  channel_arguments.SetMaxReceiveMessageSize(
      connection_options.channel_options().max_receive_message_size());
  channel_arguments.SetMaxSendMessageSize(
      connection_options.channel_options().max_send_message_size());

  if (connection_options.channel_options().has_keepalive_time()) {
    int keep_alive_time_ms =
        ToAbseilDuration(
            connection_options.channel_options().keepalive_time()) /
        absl::Milliseconds(1);
    channel_arguments.SetInt(kGrpcKeepaliveTimeArg, keep_alive_time_ms);
  }

  if (connection_options.channel_options().has_keepalive_timeout()) {
    int keep_alive_timeout_ms =
        ToAbseilDuration(
            connection_options.channel_options().keepalive_timeout()) /
        absl::Milliseconds(1);
    channel_arguments.SetInt(kGrpcKeepaliveTimeoutArg, keep_alive_timeout_ms);
  }

  channel_arguments.SetInt(
      kGrpcKeepalivePermitWithoutCallsArg,
      connection_options.channel_options().keepalive_permit_without_calls());

  channel_arguments.SetInt(
      kGrpcHttp2MaxPingsWithoutDataArg,
      connection_options.channel_options().http2_max_pings_without_data());

  if (connection_options.channel_options().fixed_reconnect_backoff_ms() > 0) {
    channel_arguments.SetInt(
        kGrpcFixedReconnectBackoffMsArg,
        connection_options.channel_options().fixed_reconnect_backoff_ms());
  }

  return channel_arguments;
}

bool ManuallySetToken(absl::string_view target_address) {
  return std::getenv(kOAuthToken) != nullptr ||
         (std::getenv(kGoogleApplicationCredentials) != nullptr &&
          absl::StrContains(target_address, kVisionAIDataplaneDomainSuffix));
}

std::string GetAudience(absl::string_view target_address) {
  if (absl::StrContains(target_address, "autopush.visionai.goog")) {
    return "https://autopush-visionai.sandbox.googleapis.com/";
  } else if (absl::StrContains(target_address, "staging.visionai.goog")) {
    return "https://staging-visionai.sandbox.googleapis.com/";
  } else {
    return "https://visionai.googleapis.com/";
  }
}

}  // namespace

ConnectionOptions DefaultConnectionOptions() {
  ConnectionOptions options;
  options.mutable_ssl_options()->set_use_insecure_channel(
      kUseInsecureChannelDefault);
  options.mutable_channel_options()->set_max_receive_message_size(
      kMaxReceiveMessageSizeDefault);
  options.mutable_channel_options()->set_max_send_message_size(
      kMaxSendMessageSizeDefault);
  options.mutable_channel_options()->set_keepalive_permit_without_calls(
      kKeepAlivePermitWithoutCallsDefault);
  options.mutable_channel_options()->set_http2_max_pings_without_data(
      kHttp2MaxPingsWithoutDataDefault);
  options.mutable_client_context_options()->set_wait_for_ready(kWaitForReady);
  *options.mutable_channel_options()->mutable_keepalive_time() =
      ToProtoDuration(kKeepAliveTime);
  *options.mutable_channel_options()->mutable_keepalive_timeout() =
      ToProtoDuration(kKeepAliveTimeout);
  return options;
}

std::shared_ptr<grpc::Channel> CreateChannel(absl::string_view target_address,
                                             const ConnectionOptions& options) {
  grpc::ChannelArguments channel_arguments = ConstructChannelArguments(options);

  std::shared_ptr<grpc::ChannelCredentials> channel_credentials;
  if (options.ssl_options().use_insecure_channel()) {
    // TODO(yxyan): lint complains about the insecure channel.
    channel_credentials = grpc::InsecureChannelCredentials();  // NOLINT
  } else if (ManuallySetToken(target_address)) {
    // Check if the GOOGLE_APPLICATION_CREDENTIALS env var is set. If it's set,
    // we use the grpc::SslCredenials because the data plane endpoint is
    // different from the service name. The generated JWT token uses the data
    // plane's endpoint as the audience which will be rejected by the Chemist.
    // We manually generates the JWT token from the json key.
    channel_credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
  } else {
    channel_credentials = grpc::GoogleDefaultCredentials();
  }
  if (options.client_context_options().no_lame_channel() &&
      channel_credentials == nullptr) {
    return nullptr;
  }

  return grpc::CreateCustomChannel(std::string(target_address),
                                   channel_credentials, channel_arguments);
}

std::unique_ptr<grpc::ClientContext> CreateClientContext(
    const ConnectionOptions& options) {
  auto ctx = std::make_unique<grpc::ClientContext>();

  ctx->set_wait_for_ready(options.client_context_options().wait_for_ready());

  for (const auto& p : options.client_context_options().metadata()) {
    ctx->AddMetadata(p.first, p.second);
  }

  if (options.client_context_options().has_timeout()) {
    absl::Duration timeout =
        ToAbseilDuration(options.client_context_options().timeout());
    ctx->set_deadline(absl::ToChronoTime(absl::Now() + timeout));
  }

  return ctx;
}

absl::Status SetAuthorizationHeaderFromJsonKey(
    absl::string_view target_address, ConnectionOptions& connection_options) {
  if (!ManuallySetToken(target_address)) {
    return absl::OkStatus();
  }

  // TODO(yxyan): rename the function.
  const char* cred = std::getenv(kOAuthToken);
  std::string token;
  if (cred != nullptr) {
    token = std::string(cred);
  } else {
    const char* cred_path = std::getenv(kGoogleApplicationCredentials);
    // Read json key file.
    std::string file_contents;
    VAI_RETURN_IF_ERROR(GetFileContents(cred_path, &file_contents));

    VAI_ASSIGN_OR_RETURN(token,
                         GetJwt(file_contents, GetAudience(target_address)));
  }

  connection_options.mutable_client_context_options()
      ->mutable_metadata()
      ->insert({kAuthorizationHeader, absl::StrFormat("Bearer %s", token)});
  return absl::OkStatus();
}

}  // namespace visionai
