// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PACKET_SENDER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PACKET_SENDER_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"

namespace visionai {

// The PacketSender sends packets to a Channel.
//
// Preconditions
// -------------
//
// 1. A ClusterSelection must be spcified.
//    This is usually possible after the user has a cluster provisioned.
//
// 2. A Channel must have already been created and ready to accept data.
//    This is usually the direct responsibility of the PacketSender user or of
//    a collaborating system such as the `EventManager`.
//
// Usage Pattern
// -------------
//
// 1. Create an instance of a PacketSender.
//
// 2. Repeatedly call Send() until either:
//    a. There is nothing more to send.
//    b. When a non-OK status is returned.
//
// 3. Destroy the instance of the PacketSender.
//
// Step 3 is required when a non-OK status is returned. If a retry/reconnect is
// desired, you must create a new instance to do so.
class PacketSender {
 public:
  // Options for configuring the packet sender.
  struct Options {
    // The cluster to connect to.
    ClusterSelection cluster_selection;

    // The channel where data is to be sent.
    Channel channel;

    // The name of the sender.
    std::string sender;

    // The amount of time that another PacketSender instance is allowed to
    // reconnect under the same `sender` after an active instance disconnects.
    absl::Duration grace_period = absl::ZeroDuration();
  };

  // Creates and initializes an instance that is ready for use.
  static absl::StatusOr<std::unique_ptr<PacketSender>> Create(const Options &);

  // Send the given packet.
  //
  // The first overload will block for at most `grace_period`.
  // The second overload will block for at most min(`timeout`, `grace_period`).
  //
  // Return Codes:
  //
  // 1. OK: On success.
  //
  // 2. CANCELLED: The client timeout expires;
  //    i.e. min(`timeout`, `grace_period`).
  //    Retrying with a new PacketSender instance may succeed.
  //
  // 3. Other codes are possible. These may or may not succeed on a retry.
  //
  // There could be more than one error, but we return just one to the caller.
  // The error returned will be no weaker than all other detected errors.
  virtual absl::Status Send(Packet packet);
  virtual absl::Status Send(Packet packet, absl::Duration timeout);

  virtual ~PacketSender();
  PacketSender(const PacketSender &) = delete;
  PacketSender &operator=(const PacketSender &) = delete;

 protected:
  explicit PacketSender(const Options &);

 private:
  const Options options_;

  struct WorkContext {
    std::string task_name;
    std::function<absl::Status(void)> task;
    std::function<absl::Status(void)> canceller;
    absl::Duration finalize_timeout;
    std::unique_ptr<streams_internal::Worker> worker;
  };
  std::vector<std::unique_ptr<WorkContext>> work_contexts_;

  void MaybeUpdateStrongestErrorStatus(const absl::Status &candidate);
  absl::Status GetStrongestErrorStatus();
  absl::Mutex strongest_error_status_mu_;
  absl::Status strongest_error_status_;

  absl::Status Initialize();
  absl::Status ValidateOptions();
  absl::Status BuildWorkContexts();
  absl::Status StartWork();
  void CancelWork();

  absl::StatusOr<std::unique_ptr<WorkContext>> BuildGrpcSenderWorkContext();
  class GrpcSenderTask;
  std::unique_ptr<GrpcSenderTask> grpc_sender_task_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PACKET_SENDER_H_
