/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_DESCRIPTORS_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_DESCRIPTORS_H_

#include <string>

#include "absl/time/time.h"

namespace visionai {

// The Channel uniquely identifies a streaming destination where writers/readers
// may send/receive data. You may think of it as simply being an
// (event_id, stream_id) tuple, but is fully materialized and has resources
// ready to accept data I/O.
struct Channel {
  // The event id of this channel.
  std::string event_id;

  // The stream id of this channel.
  std::string stream_id;
};

// The type of the channel lease. Either a writer's or a reader's lease.
enum class ChannelLeaseType { kWriters, kReaders };

// The ChannelLease is the client-side descriptor of a channel Lease that
// the google::cloud::visionai::StreamingService has uniquely granted to a
// specific Channel.
struct ChannelLease {
  // The unique id of the lease.
  std::string id;

  // The channel that the lease is granted for.
  Channel channel;

  // The name of the lessee that holds the lease.
  std::string lessee;

  // The timestamp, according to the local clock, at which this lease acquired.
  absl::Time acquired_time;

  // The length of the lease.
  absl::Duration duration;

  // The type of the lease.
  ChannelLeaseType lease_type;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_DESCRIPTORS_H_
