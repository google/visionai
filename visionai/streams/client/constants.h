// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_STREAMS_CLIENT_CONSTANTS_H_
#define VISIONAI_STREAMS_CLIENT_CONSTANTS_H_

#include <string>

#include "absl/time/time.h"

namespace visionai {

// Default lease duration.
constexpr absl::Duration kDefaultLeaseDuration = absl::Minutes(5);

// Default lease renewal period.
constexpr absl::Duration kDefaultLeaseRenewalPeriod = absl::Minutes(1);

// Default finalization timeout for lease workers.
constexpr absl::Duration kDefaultLeaseWorkerFinalizationTimeout =
    absl::Milliseconds(200);

// Default finalization timeout for grpc send workers.
constexpr absl::Duration kDefaultGrpcSendWorkerFinalizationTimeout =
    absl::Seconds(10);

// Default finalization timeout for grpc receiver workers.
constexpr absl::Duration kDefaultGrpcReceiveWorkerFinalizationTimeout =
    absl::Seconds(10);

// Default timeout for popping elements from a pcqueue buffer.
constexpr absl::Duration kDefaultQueuePopTimeout = absl::Seconds(1);

// Default receive buffer size in the PacketReceiver.
constexpr size_t kDefaultReceiveBufferSize = 200;

// Default queue size in the event sampler.
constexpr int kDefaultEventSamplerQueueSize = 50;

// Default server heartbeat interval.
constexpr absl::Duration kDefaultServerHeartbeatInterval = absl::Minutes(1);

// Default heartbeat grace period.
constexpr absl::Duration kDefaultHeartbeatGracePeriod = absl::Seconds(20);

// Err Message Prefix for PacketReceiver.
constexpr char kPacketReceiverErrMsgPrefix[] = "[PacketReceiver]";

}  // namespace visionai

#endif  // VISIONAI_STREAMS_CLIENT_CONSTANTS_H_
