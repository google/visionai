/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_CLIENT_CONNECT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_CLIENT_CONNECT_H_

#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"

namespace visionai {

// Get a ConnectionOptions initialized to reasonable defaults for Packet
// streaming use cases.
//
// See the .cc file for actual default values.
::visionai::ConnectionOptions
DefaultConnectionOptions();

// Get a ConnectionOptions initialized to reasonable defaults for unit test.
::visionai::ConnectionOptions DefaultConnectionOptionsForTest();

// Create a grpc channel from the given connection options.
//
// On failure, a "lame" channel is returned (one on which all operations fail).
// This is the same convention as in grpc.
std::shared_ptr<::grpc::Channel> CreateChannel(
    absl::string_view target_address,
    const ::visionai::ConnectionOptions&
        options);

// Create a client context from the given connection options.
//
// This always succeeds.
std::unique_ptr<::grpc::ClientContext> CreateClientContext(
    const ::visionai::ConnectionOptions&
        options);

// Create a client context from the given client context options.
//
// This always succeeds.
std::unique_ptr<::grpc::ClientContext> CreateClientContext(
    const ::visionai::ConnectionOptions::ClientContextOptions&
        client_context_options);

// Set the authorization header from the json key if necessary. The function
// will check if the GOOGLE_APPLICATION_CREDENTIALS is set and the target
// address has the visionai.goog suffix. This function is expected to be called
// by SeriesServer client and EventWatcher client only.
absl::Status SetAuthorizationHeaderFromJsonKey(
    absl::string_view target_address,
    ::visionai::ConnectionOptions&
        connection_options);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_UTIL_NET_GRPC_CLIENT_CONNECT_H_
