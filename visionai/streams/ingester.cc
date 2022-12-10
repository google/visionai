// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/ingester.h"

#include <memory>
#include <thread>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "visionai/streams/capture_module.h"
#include "visionai/streams/constants.h"
#include "visionai/streams/depositor_module.h"
#include "visionai/streams/event_manager.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/ring_buffer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// ----------------------------------------------------------------------------
// Ingester Implementation
// ----------------------------------------------------------------------------
//
// The ingester consists of several modules:
//
// 1. Capture Module: This module contains the context and the specific capture
//    logic requested by the Ingester user. Its responsibility is to capture
//    data from some external source.
//
// 2. Filter Module: This module contains the context and the specific filter
//    logic requested by the Ingester user. Its responsibility is to filter, or
//    otherwise transform the captured data, acquired from the capture module.
//    It also marks the output elements to indicate the event each datum belongs
//    to.
//
// 3. Depositor Module: This module deposites (writes) the filter module's
//    result into some external storage requested by the Ingester user.
//
// The ingester owns and manages the following dynamic resources:
//
// 1. Workers: For each module, a worker is used to run its main loop.
//
// 2. Buffers: Between each worker is a buffer used to communicate results.
//
// The ingester runs and manages the dataflow in the following phases:
//
// 1. PREPARE: Create the specific modules requested and setup the buffers
//    between them. It will also initialze the module themselves. By the end of
//    this phase, all of the modules and buffers are properly initialized and
//    ready for the data to flow.
//
// 2. RUN: The dataflow runs in this phase. There are actually two subphases:
//
//    A. INIT: In this phase, the specific logic used to perform the main work
//       of the module is initialized; i.e. this is where the custom user
//       initialition logic like `Capture::Init` is run.
//
//    B. WORK: In this phase, the workers are actually dispatched and are
//       running the module in the background; i.e. this is where the custom
//       user logic like `Capture::Run` is run. The main thread of the ingester
//       just checks on the progress of the workers periodically.
//
// 3. DONE: In this phase, the dataflow has stopped. This can happen if the
//    source has completed triggering all parties to complete (normal success),
//    if the user has requested a cancellation (cancelled, but nonetheless
//    successful), or if a non-recoverable error has occured (failure).

Ingester::Ingester(const IngesterConfig& config) : config_(config) {}

Ingester::~Ingester() = default;

absl::Status Ingester::Prepare() {
  VAI_RETURN_IF_ERROR(CreateModules()) << "while creating modules";
  VAI_RETURN_IF_ERROR(CreateAndAttachInterModuleBuffers())
      << "while creating and attaching inter-module buffers";
  VAI_RETURN_IF_ERROR(CreateAndAttachEventManager())
      << "while creating and attaching the event manager";
  VAI_RETURN_IF_ERROR(PrepareModules()) << "while initializing modules";
  return absl::OkStatus();
}

absl::Status Ingester::Run() {
  VAI_RETURN_IF_ERROR(StartModules()) << "while starting modules";

  auto ingester_sleep_period =
      absl::Milliseconds(kDefaultIngesterSleepPeriodMs);
  if (config_.parameters().sleep_period_ms() > 0) {
    ingester_sleep_period =
        absl::Milliseconds(config_.parameters().sleep_period_ms());
  }
  while (!is_cancelled_.HasBeenNotified()) {
    if (capture_worker_->IsDone() || filter_worker_->IsDone() ||
        depositor_worker_->IsDone()) {
      break;
    }
    is_cancelled_.WaitForNotificationWithTimeout(ingester_sleep_period);
  }
  VAI_RETURN_IF_ERROR(StopModules()) << "while stopping modules";
  return absl::OkStatus();
}

absl::Status Ingester::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

absl::Status Ingester::CreateModules() {
  capture_module_ = std::make_shared<CaptureModule>(config_.capture_config());
  filter_module_ = std::make_shared<FilterModule>(config_.filter_config());
  depositor_module_ =
      std::make_shared<DepositorModule>(config_.depositor_config());
  return absl::OkStatus();
}

absl::Status Ingester::CreateAndAttachInterModuleBuffers() {
  int capture_output_buffer_capacity =
      config_.parameters().capture_output_buffer_capacity();
  if (capture_output_buffer_capacity <= 0) {
    capture_output_buffer_capacity = kDefaultCaptureOutputBufferCapacity;
  }
  capture_output_buffer_ =
      std::make_shared<RingBuffer<Packet>>(capture_output_buffer_capacity);
  capture_module_->AttachOutput(capture_output_buffer_);

  int filter_output_buffer_capacity =
      config_.parameters().filter_output_buffer_capacity();
  if (filter_output_buffer_capacity <= 0) {
    filter_output_buffer_capacity = kDefaultFilterOutputBufferCapacity;
  }
  filter_output_buffer_ =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(
          filter_output_buffer_capacity);
  filter_module_->AttachInput(capture_output_buffer_);
  filter_module_->AttachOutput(filter_output_buffer_);

  depositor_module_->AttachInput(filter_output_buffer_);

  return absl::OkStatus();
}

absl::Status Ingester::CreateAndAttachEventManager() {
  EventManager::Options options;
  options.config = config_.event_writer_config();
  options.ingest_policy = config_.ingest_policy();
  event_manager_ = std::make_unique<EventManager>(options);
  filter_module_->AttachEventManager(event_manager_);
  depositor_module_->AttachEventManager(event_manager_);
  return absl::OkStatus();
}

absl::Status Ingester::PrepareModules() {
  VAI_RETURN_IF_ERROR(capture_module_->Prepare())
      << "while preparing the CaptureModule";
  VAI_RETURN_IF_ERROR(filter_module_->Prepare())
      << "while preparing the FilterModule";
  VAI_RETURN_IF_ERROR(depositor_module_->Prepare())
      << "while preparing the DepositorModule";
  return absl::OkStatus();
}

absl::Status Ingester::StartModules() {
  depositor_worker_ = std::make_unique<streams_internal::Worker>();
  VAI_RETURN_IF_ERROR(depositor_worker_->Work([this]() {
    return depositor_module_->Run();
  })) << "while putting the depositor worker to work";

  VAI_RETURN_IF_ERROR(filter_module_->Init());
  filter_worker_ = std::make_unique<streams_internal::Worker>();
  VAI_RETURN_IF_ERROR(filter_worker_->Work([this]() {
    return filter_module_->Run();
  })) << "while putting the filter worker to work";

  VAI_RETURN_IF_ERROR(capture_module_->Init());
  capture_worker_ = std::make_unique<streams_internal::Worker>();
  VAI_RETURN_IF_ERROR(capture_worker_->Work([this]() {
    return capture_module_->Run();
  })) << "while putting the capture worker to work";
  return absl::OkStatus();
}

absl::Status Ingester::StopModules() {
  auto capture_worker_finalize_timeout =
      absl::Milliseconds(kDefaultCaptureWorkerFinalizeTimeoutMs);
  if (config_.parameters().capture_worker_finalize_timeout_ms() > 0) {
    capture_worker_finalize_timeout = absl::Milliseconds(
        config_.parameters().capture_worker_finalize_timeout_ms());
  }
  auto filter_worker_finalize_timeout =
      absl::Milliseconds(kDefaultFilterWorkerFinalizeTimeoutMs);
  if (config_.parameters().filter_worker_finalize_timeout_ms() > 0) {
    filter_worker_finalize_timeout = absl::Milliseconds(
        config_.parameters().filter_worker_finalize_timeout_ms());
  }
  auto depositor_worker_finalize_timeout =
      absl::Milliseconds(kDefaultDepositorWorkerFinalizeTimeoutMs);
  if (config_.parameters().depositor_worker_finalize_timeout_ms() > 0) {
    depositor_worker_finalize_timeout = absl::Milliseconds(
        config_.parameters().depositor_worker_finalize_timeout_ms());
  }

  std::vector<StopContext> stop_contexts(3);
  stop_contexts[0].name = "capture";
  stop_contexts[0].worker = capture_worker_.get();
  stop_contexts[0].finalize_timeout = capture_worker_finalize_timeout;
  stop_contexts[0].task_canceller = [this]() {
    return capture_module_->Cancel();
  };
  stop_contexts[1].name = "filter";
  stop_contexts[1].worker = filter_worker_.get();
  stop_contexts[1].finalize_timeout = filter_worker_finalize_timeout;
  stop_contexts[1].task_canceller = [this]() {
    return filter_module_->Cancel();
  };
  stop_contexts[2].name = "depositor";
  stop_contexts[2].worker = depositor_worker_.get();
  stop_contexts[2].finalize_timeout = depositor_worker_finalize_timeout;
  stop_contexts[2].task_canceller = [this]() {
    return depositor_module_->Cancel();
  };

  absl::Status status = absl::OkStatus();
  for (auto& ctx : stop_contexts) {
    if (!ctx.worker->IsDone()) {
      ctx.worker->Cancel(ctx.task_canceller);
    }
    auto s = ctx.worker->GetReturnStatus(ctx.finalize_timeout,
                                         &ctx.worker_return_status);
    if (!s.ok()) {
      LOG(WARNING) << absl::StrFormat(
          "The %s did not stop within the given timeout "
          "and may still be working.",
          ctx.name);
    }
    if (!ctx.worker_return_status.ok()) {
      LOG(ERROR) << ctx.worker_return_status;
      status = absl::UnknownError(
          "The dataflow encounterd an error. See logs for more details.");
    }
  }
  return status;
}

}  // namespace visionai
