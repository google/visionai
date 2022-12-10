// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CHANNEL_LEASE_RENEWAL_TASK_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CHANNEL_LEASE_RENEWAL_TASK_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"

namespace visionai {

// The ChannelLeaseRenewalTask may be run by a thread to perform the acquisition
// and regular renewal of a Lease.
class ChannelLeaseRenewalTask {
 public:
  // Options for configuring the packet sender.
  struct Options {
    // The cluster to connect to.
    ClusterSelection cluster_selection;

    // The channel on which to hold the lease.
    Channel channel;

    // The owner associated with the lease.
    std::string lessee;

    // The type of channel lease to acquire.
    ChannelLeaseType lease_type;

    // The amount of time that another ChannelLeaseRenewalTask instance is
    // allowed to reconnect under the same `lessee` after an active instance
    // disconnects.
    //
    // A system default will be chosen if set to absl::ZeroDuration().
    absl::Duration grace_period = absl::ZeroDuration();
  };

  // Creates and initializes an instance that is ready for use.
  static absl::StatusOr<std::unique_ptr<ChannelLeaseRenewalTask>> Create(
      const Options &);

  // Run the task.
  //
  // The calling thread will be blocked until it returns.
  absl::Status Run();

  // Request the task to be cancelled.
  //
  // The thread blocked on `Run` will eventually return with CANCELLED.
  void Cancel();

  // Get the currently acquired lease.
  //
  // Blocks the calling thread until the lease is acquired or up to `timeout`.
  absl::StatusOr<ChannelLease> GetLease(absl::Duration timeout);

  explicit ChannelLeaseRenewalTask(const Options &options);
  ~ChannelLeaseRenewalTask() = default;
  ChannelLeaseRenewalTask(const ChannelLeaseRenewalTask &) = delete;
  ChannelLeaseRenewalTask &operator=(const ChannelLeaseRenewalTask &) = delete;

 private:
  const Options options_;
  absl::Notification cancel_notification_;

  absl::Mutex lease_mu_;
  ChannelLease lease_;
  absl::Notification lease_acquired_notification_;

  void UpdateLease(const ChannelLease &lease);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_CHANNEL_LEASE_RENEWAL_TASK_H_
