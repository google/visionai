// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_MODULE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_MODULE_H_

#include <memory>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/streams/framework/capture.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// The CaptureModule is a facade for the Ingester to run a Capture. It contains
// the specific Capture requested, as well as all the environment information
// necessary for it to run.
class CaptureModule {
 public:
  // Construct an instance with the given configurations.
  //
  // Please use the builder methods to configure additional behavior, and run
  // `Prepare` to get a usable instance.
  explicit CaptureModule(const visionai::CaptureConfig&);

  // Attach the output buffer into which to push captured Packets.
  //
  // TODO: Change to Packets.
  CaptureModule& AttachOutput(
      std::shared_ptr<RingBuffer<Packet>> output_buffer);

  // Finalizes the builder configuration and initializes the CaptureModule.
  //
  // This method initializes the CaptureModule itself. It must be called before
  // executing the Capture life-cycle routines.
  absl::Status Prepare();

  // Copy-control members.
  //
  // Movable, but not Copyable.
  ~CaptureModule() = default;
  CaptureModule(CaptureModule&&) = default;
  CaptureModule& operator=(CaptureModule&&) = default;
  CaptureModule(const CaptureModule&) = delete;
  CaptureModule& operator=(const CaptureModule&) = delete;

  // Capture life-cycle routines
  // ---------------------------

  // Phase 1: Inits the Capture.
  absl::Status Init();

  // Phase 2: Runs the Capture.
  absl::Status Run();

  // Phase 3: Cancels the Capture.
  absl::Status Cancel();

 private:
  const CaptureConfig config_;

  std::shared_ptr<RingBuffer<Packet>> capture_output_buffer_ = nullptr;
  std::unique_ptr<CaptureInitContext> capture_init_ctx_ = nullptr;
  std::unique_ptr<CaptureRunContext> capture_run_ctx_ = nullptr;
  std::unique_ptr<Capture> capture_ = nullptr;

  absl::StatusOr<std::unique_ptr<CaptureInitContext>>
  CreateCaptureInitContext();
  absl::StatusOr<std::unique_ptr<CaptureRunContext>> CreateCaptureRunContext();
  absl::StatusOr<std::unique_ptr<Capture>> CreateCapture();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_MODULE_H_
