// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_RECEIVE_PACKETS_GRPC_V1_CLIENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_RECEIVE_PACKETS_GRPC_V1_CLIENT_H_

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streaming_service.grpc.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/net/grpc/client_connect.h"

namespace visionai {

// `StreamingReceivePacketsGrpcV1Client` makes it easier for users to
// participate directly in the application level protocol of
// `StreamingService`'s `ReceivePackets` bi-directional streaming RPC.
//
// It takes care of all the basic grpc and API call conventions, performs the
// initial setup/handshake, and returns to the caller only when it is ready to
// simply `Read` and `Write` messages.
//
// Typical usage:
//
//   Step 1: Configure the options for the client and create an instance.
//
//    This is mostly about specifying where the endpoint is, and any
//    customiaztion for specific receiveing modes.
//    options for specific receiving modes.
//
//    ```
//     StreamingReceivePacketsGrpcV1Client::Options options;
//     VAI_ASSIGN_OR_RETURN(auto client,
//                      StreamingReceivePacketsGrpcV1Client::Create(options));
//    ```
//
//   Step 2: Read and write messages any number of times. They may be
//           interleaved as well as concurrent.
//
//    ```
//     ReceivePacketsResponse response;
//     if (client->Read(&response)) {
//       // Success! Do something with the response.
//     } else {
//       // If `Read` returns false, then that means that the server has no more
//       // messages to send and further calls to `Read` will return false.
//     }
//
//     if (client->WriteCommit(offset)) {
//       // The message has been successfully sent to the server.
//       // A commit message is just an example of a write.
//     } else {
//       // The message did not successfully send. While you may retry,
//       // it is more likely that you will succeed if you terminate the entire
//       instance and try again.
//     }
//    ```
//
//   Step 3: Cleanly shutdown. It must be done in this order:
//    a. Explicitly close the write stream.
//    b. If `Read` has not yet returned false, read until it does.
//       If `Read` blocks for too long at this stage, you may issue a
//       `Cancel` to unblock; however, this will result in a CANCEL in
//       the final RPC status, and will obscure any server side feedback.
//    c. Get the final status of the RPC.
//
//    ```
//     client->WritesDone();
//
//     while (client->Read(&response)) ;
//
//     auto final_rpc_status = client->Finish();
//     LOG(INFO) << final_rpc_status;
//    ```
class StreamingReceivePacketsGrpcV1Client {
 public:
  // Options for configuring the receiver client.
  struct Options {
    // The data plane endpoint that hosts the series services.
    //
    // This may be a DNS name or a (ip:port).
    std::string target_address;

    // Cluster resource name.
    std::string cluster_name;

    // The channel to connect to.
    Channel channel;

    // The receiver name.
    //
    // This must match the owner of the lease identified by `lease_id`.
    std::string receiver;

    // The maximum duration that the `receiver` may be disconnected from the
    // `channel`, after which the server considers the receiving be complete.
    absl::Duration lease_term = absl::ZeroDuration();

    // The mode used to receive data.
    //
    // This is either "eager" or "controlled".
    std::string receive_mode = "eager";

    // Advanced options.
    struct Advanced {
      // The duration of silence before which a server heartbeat is expected.
      //
      // A system default will be chosen if not set to a finite positive value.
      absl::Duration heartbeat_interval;

      // The grace period after which the server will severe the RPC after it
      // issues the final writes done request.
      //
      // The server will choose a default if not set to a finite positive value.
      absl::Duration writes_done_grace_period;

      // Options to configure the RPC connection.
      ConnectionOptions connection_options = DefaultConnectionOptions();

      // The options specific to "controlled" mode.
      struct ControlledModeOptions {
        // This is the where the reader will begin its reads.
        //
        // "begin": Start from the earliest available message.
        // "most-recent": Start from the latest available message.
        // "end": Start from future messages.
        // "stored": Start just after the last commit point, restoring progress.
        std::string starting_logical_offset = "stored";

        // This is the where the reader will begin its reads if no checkpoint is
        // available. Valid values are:
        //
        // "begin": Start from the earliest available message.
        // "end": Start from future messages.
        std::string fallback_starting_offset = "begin";
      };
      ControlledModeOptions controlled_mode_options;
    };

    // Specific advanced settings.
    //
    // This typically does not need to be configured.
    Advanced advanced;
  };

  // Creates and initializes an instance that is ready for use.
  //
  // All of the setup and handshakes would have completed successfully. If not,
  // an error indicating what went wrong will be returned.
  static absl::StatusOr<std::unique_ptr<StreamingReceivePacketsGrpcV1Client>>
  Create(const Options&);

  // Read a message from the server.
  //
  // Blocks until one of the following occurs:
  // 1. The next message has been stored into `response`, returning true.
  // 2. All messages for this grpc session have been read, returning false.
  //
  // Important: The message exhaustion in case 2 is in regards to this specific
  // grpc session, not to the Anaheim Stream itself. To detect that the Anaheim
  // Stream has no more data in the future, the RPC itself must return with an
  // OUT_OF_RANGE. See `Finish` below.
  virtual bool Read(
      google::cloud::visionai::v1::ReceivePacketsResponse* response);

  // Write to the server to update the read checkpoint to `offset`.
  //
  // Blocks until one of the following occurs:
  // 1. The server accepts write, returning true.
  // 2. The grpc stream is closed and the write has failed, returning false.
  virtual bool WriteCommit(int64_t offset);

  // Close the grpc write stream and signals to the server that writes are done.
  //
  // Blocks until all pending writes are complete.
  //
  // Returns true if all writes were successful, false otherwise. In either
  // case, the write stream is closed and the server signaled.
  virtual bool WritesDone();

  // This will unilaterally close both the read and write streams by a client
  // side cancellation. All `Read`s and `Write`s will unblock, and `Finish` may
  // be called to receive CANCELLED.
  //
  // Use this for a client side cancellation.
  //
  // Note that this will obscure any feedback from the server, so this should
  // not be used as a substitute for correctly participating in the grpc bidi
  // application level protocol.
  virtual void Cancel();

  // Call this to complete the grpc session and to get the final status of the
  // RPC. This final status will tell whether the nature of the termination of
  // the current grpc session; e.g. an OUT_OF_RANGE for the Anaheim Stream, a
  // regular maintenance severance, etc.
  //
  // This function may only be called when *both* of the following conditions
  // are met:
  //
  // 1. The read grpc stream has been closed.
  //
  //    The caller may satisfy this condition in several ways:
  //    a. Call `Read` until `false` is returned. This is a clean closure.
  //    b. Call `Cancel`. This is an abrupt termination, but useful as a last
  //       resort if the client cannot complete the protocol cleanly.
  //
  // 2. The write grpc stream has been closed.
  //
  //    The caller may satisfy this condition in several ways:
  //    a. Call `WritesDone`. This is a clean closure.
  //    b. Encountering `false` calling any of the `Write` methods.
  //    c. Call `Cancel`. This is an abrupt termination, but useful as a last
  //       resort if the client cannot complete the protocol cleanly.
  //
  // On success, returns OK and `rpc_status` will contain the final RPC status.
  // Otherwise, returns the reason for why the call failed.
  virtual absl::Status Finish(absl::Status* rpc_status);

  // The destructor will terminate the rpc session, and will do so forcefully
  // if the caller hasn't already closed the stream cleanly.
  //
  // If `Finish` has not been called, then it will be called after all streams
  // have closed. The final RPC status will be logged.
  virtual ~StreamingReceivePacketsGrpcV1Client();

  // --------------------------------------------------------------------------
  // Implementation below

 private:
  explicit StreamingReceivePacketsGrpcV1Client(const Options&);

  Options options_;

  std::unique_ptr<google::cloud::visionai::v1::StreamingService::Stub>
      stub_ = nullptr;
  std::unique_ptr<grpc::ClientContext> ctx_ = nullptr;
  std::unique_ptr<grpc::ClientReaderWriter<
      google::cloud::visionai::v1::ReceivePacketsRequest,
      google::cloud::visionai::v1::ReceivePacketsResponse>>
      stream_ = nullptr;

  absl::Status Initialize();
  absl::Status OpenRpc();
  absl::Status WriteSetupMessage();
  bool IsRpcOpen();
  absl::Status PrepareRpcStubs();

  // Flags and access methods to decide whether the read/write streams have
  // terminated.

  mutable absl::Mutex read_stream_terminated_mu_;
  bool read_stream_terminated_ = false;

  mutable absl::Mutex write_stream_terminated_mu_;
  bool write_stream_terminated_ = false;
  bool writes_done_result_ = false;

  bool IsReadStreamTerminated() {
    absl::MutexLock lock(&read_stream_terminated_mu_);
    return read_stream_terminated_;
  }

  void SetReadStreamTerminated(bool state) {
    absl::MutexLock lock(&read_stream_terminated_mu_);
    read_stream_terminated_ = state;
  }

  bool IsWriteStreamTerminated() {
    absl::MutexLock lock(&write_stream_terminated_mu_);
    return write_stream_terminated_;
  }

  void SetWriteStreamTerminated(bool state) {
    absl::MutexLock lock(&write_stream_terminated_mu_);
    write_stream_terminated_ = state;
  }
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_STREAMING_RECEIVE_PACKETS_GRPC_V1_CLIENT_H_
