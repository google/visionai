// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/event_sink.h"

#include <functional>
#include <memory>

#include "glog/logging.h"
#include "absl/cleanup/cleanup.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/streams/constants.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

EventSink::EventSink(const Options& options) : options_(options) {}

absl::StatusOr<std::shared_ptr<EventSink>> EventSink::Create(
    const Options& options) {
  auto sink = std::make_shared<EventSink>(options);
  VAI_RETURN_IF_ERROR(sink->Initialize()) << "while initializing an EventSink";
  return sink;
}

absl::Status EventSink::Initialize() {
  size_t write_buffer_capacity = kDefaultEventSinkWriteBufferCapacity;
  if (options_.write_buffer_capacity > 0) {
    write_buffer_capacity = options_.write_buffer_capacity;
  }
  write_buffer_ =
      std::make_shared<ProducerConsumerQueue<Packet>>(write_buffer_capacity);

  // Create an EventWriter.
  VAI_ASSIGN_OR_RETURN(auto writer,
                   EventWriterRegistry::Global()->CreateEventWriter(
                       options_.event_writer_config.name()),
                   _ << "while creating an EventWriter");
  VAI_ASSIGN_OR_RETURN(
      auto init_ctx,
      EventWriterInitContext::Create(options_.event_writer_config),
      _ << "while attempting to create the EventWriterInitContext");

  // Start the write worker.
  auto ctx = std::make_shared<MainContext>();
  ctx->writer = std::move(writer);
  ctx->init_ctx = std::move(init_ctx);
  ctx->event_id = options_.event_id;
  worker_ = std::make_unique<streams_internal::Worker>();
  VAI_RETURN_IF_ERROR(worker_->Work([this, ctx]() {
    auto status = this->WriterMain(*ctx);
    return status;
  })) << "while starting the writer worker";

  return absl::OkStatus();
}

absl::Status EventSink::WriterMain(const MainContext& ctx) {
  // Create and open the event for writing.
  VAI_RETURN_IF_ERROR(ctx.writer->Init(ctx.init_ctx.get()))
      << "while initializing the EventWriter";
  VAI_RETURN_IF_ERROR(ctx.writer->Open(ctx.event_id))
      << "while opening event \"" << ctx.event_id << "\"";
  auto event_writer_closer = absl::MakeCleanup([&ctx] {
    auto s = ctx.writer->Close();
    if (!s.ok()) {
      LOG(WARNING) << "The EventWriter did not successfully Close: " << s;
    }
  });

  // Main write loop.
  while (!close_signal_.HasBeenNotified()) {
    Packet p;
    if (!write_buffer_->TryPop(
            p,
            absl::Milliseconds(kDefaultEventSinkFinalizationTimeoutMs / 2))) {
      continue;
    }
    // TODO: Currently, any error just ends the event.
    //       We may want to distinguish between different conditions.
    VAI_RETURN_IF_ERROR(ctx.writer->Write(std::move(p)))
        << "during an EventWrite write";
  }

  // Write any lingering packets.
  Packet p;
  while (write_buffer_->TryPop(p)) {
    VAI_RETURN_IF_ERROR(ctx.writer->Write(std::move(p)))
        << "during final EventWrite writes";
  }
  return absl::OkStatus();
}

void EventSink::Close() {
  worker_->Cancel([this]() {
    if (!close_signal_.HasBeenNotified()) {
      close_signal_.Notify();
    }
    return absl::OkStatus();
  });

  absl::Status return_status;
  auto s = worker_->GetReturnStatus(
      absl::Milliseconds(kDefaultEventSinkFinalizationTimeoutMs),
      &return_status);
  if (!s.ok()) {
    LOG(WARNING) << "The EventSink's write worker may still be running.";
  }
}

absl::Status EventSink::Write(Packet p) {
  if (close_signal_.HasBeenNotified()) {
    return absl::FailedPreconditionError(
        "The EventSink is closed for writing.");
  }
  if (worker_->IsDone()) {
    absl::Status return_status;
    auto s = worker_->GetReturnStatus(absl::ZeroDuration(), &return_status);
    if (!s.ok()) {
      return absl::InternalError(
          "EventSink worker stopped but couldn't get a status.");
    }
    if (return_status.ok()) {
      return absl::UnknownError(
          "The EventSink worker unexpectedly returned OK.");
    }
    return return_status;
  }
  auto p_ptr = std::make_unique<Packet>(std::move(p));
  if (!write_buffer_->TryPush(p_ptr)) {
    return absl::UnavailableError(
        absl::StrFormat("The write buffer is currently full (capacity = %d).",
                        write_buffer_->capacity()));
  }
  return absl::OkStatus();
}

EventSink::~EventSink() { Close(); }

}  // namespace visionai
