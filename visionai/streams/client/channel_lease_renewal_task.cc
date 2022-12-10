// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/channel_lease_renewal_task.h"

#include <memory>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

ChannelLeaseRenewalTask::ChannelLeaseRenewalTask(const Options& options)
    : options_(options) {}

absl::StatusOr<std::unique_ptr<ChannelLeaseRenewalTask>>
ChannelLeaseRenewalTask::Create(const Options& options) {
  VAI_RETURN_IF_ERROR(ValidateClusterSelection(options.cluster_selection));
  if (options.channel.event_id.empty()) {
    return absl::InvalidArgumentError(
        "The `event_id` of the channel must be specified.");
  }
  if (options.channel.stream_id.empty()) {
    return absl::InvalidArgumentError(
        "The `stream_id` of the channel must be specified.");
  }
  if (options.lessee.empty()) {
    return absl::InvalidArgumentError("The lessee must be specified.");
  }
  return std::make_unique<ChannelLeaseRenewalTask>(options);
}

void ChannelLeaseRenewalTask::UpdateLease(const ChannelLease& lease) {
  absl::MutexLock lock(&lease_mu_);
  lease_ = lease;
}

absl::StatusOr<ChannelLease> ChannelLeaseRenewalTask::GetLease(
    absl::Duration timeout) {
  if (!lease_acquired_notification_.WaitForNotificationWithTimeout(timeout)) {
    return absl::DeadlineExceededError("The lease has not been acquired yet.");
  }
  absl::MutexLock l(&lease_mu_);
  return lease_;
}

absl::Status ChannelLeaseRenewalTask::Run() {
  // Acquire the lease.
  LeaseOptions lease_options;
  lease_options.lessee = options_.lessee;
  auto grace_period = options_.grace_period == absl::ZeroDuration()
                          ? kDefaultLeaseDuration
                          : options_.grace_period;
  auto renewal_period = grace_period / 4;
  lease_options.duration = grace_period + renewal_period;
  lease_options.channel = options_.channel;
  lease_options.lease_type = options_.lease_type;
  VAI_ASSIGN_OR_RETURN(
      auto lease,
      AcquireChannelLease(options_.cluster_selection, lease_options),
      _ << "while acquiring the lease");
  UpdateLease(lease);
  lease_acquired_notification_.Notify();

  // Keep renewing until told to stop.
  while (!cancel_notification_.WaitForNotificationWithTimeout(renewal_period)) {
    auto status = RenewLease(options_.cluster_selection, &lease);
    if (!status.ok()) {
      // TODO: We can have more informative diagnostics here.
      LOG(WARNING)
          << status
          << " while renewing the lease. Will retry in the next period.";
    }
    UpdateLease(lease);
  }
  return absl::CancelledError("Received a cancellation request.");
}

void ChannelLeaseRenewalTask::Cancel() { cancel_notification_.Notify(); }

}  // namespace visionai
