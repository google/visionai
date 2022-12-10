// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_MOTION_DECODER_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_MOTION_DECODER_H_

#include <memory>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/tuple_apply.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// Decodes H264 video arriving as a stream of `GstreamerBuffers` to
// `MotionVectors` using Gstreamer.
//
// Call `Feed()` to give GstreamerBuffer to the decoder. Once the decoder has
// finished decoding the input, it will return a `MotionVectors` wrapped in a
// StatusOr, via the callback, and any other data that should be associated with
// that output. The associated data is stored in a queue and returned with the
// `MotionVectors` within the callback.
//
// NOTE: Only 1 callback may be active at a time, more data will not be returned
// until the previous callback has returned. This is to prevent race conditions.
//
// Example:
//   GstreamerAsyncMotionDecoder<> decoder([](absl::StatusOr<MotionVectors>
//   mv) {
//     // Do something with mv.
//   });
//   decoder.Feed(input_gstreamer_buffer);
//
// Example:
//   GstreamerAsyncMotionDecoder<int64> decoder([](
//     absl::StatusOr<MotionVectors> mvs, int64 timestamp) {
//       emit(timestamp, *mvs);
//   });
//   decoder.Feed(input_gstreamer_buffer, 12345);
//
// Example:
//   GstreamerAsyncMotionDecoder<ProcessContext> decoder([](
//   absl::StatusOr<MotionVectors> mvs, ProcessContext context) {
//     context.Emit(mvs);
//   });
//   decoder.Feed(input_gstreamer_buffer, some_context);
//
//
// NOTE: This class is thread-unsafe.
template <class... Args>
class GstreamerAsyncMotionDecoder {
 public:
  // Callback function type for `GstreamerAsyncMotionDecoder`.
  using Callback =
      std::function<absl::Status(absl::StatusOr<MotionVectors>, Args...)>;

  // Creates an instance. The instance is fully initialized when `Feed` is
  // called for the first time. When a decode result is ready, the callback will
  // be called. If the queue is full, 'Feed' will block until there is room in
  // the queue, or until feed_timeout is reached.
  //
  // If EOS is reached, the callback will be supplied with a
  // `kResourceExhausted` error.
  GstreamerAsyncMotionDecoder(Callback callback, size_t queue_size = 300,
                              absl::Duration feed_timeout = absl::Seconds(60))
      : callback_(callback),
        feed_timeout_(feed_timeout),
        pcqueue_(queue_size) {}

  // Disables copying and moving
  GstreamerAsyncMotionDecoder(const GstreamerAsyncMotionDecoder&) = delete;
  GstreamerAsyncMotionDecoder& operator=(const GstreamerAsyncMotionDecoder&) =
      delete;
  GstreamerAsyncMotionDecoder(GstreamerAsyncMotionDecoder&&) = delete;
  GstreamerAsyncMotionDecoder& operator=(GstreamerAsyncMotionDecoder&&) =
      delete;

  // Feeds the given `GstreamerBuffer` into the decoder, which emit
  // `MotionVectors` of the current frame. Any associated data can also be
  // added.
  //
  // The first call will be slightly slower than subsequent calls because it
  // initialize the underlying `GstreamerRunner`.
  //
  // If the internal queue is full, this function will block until either the
  // queue has free space or `feed_timeout` is reached.
  //
  // Feeding is protected by a mutex to avoid buffer for t=1 being associated
  // with data t=3 (if two threads are executing `Feed` at the same time).
  //
  // Below are error codes that may require special handling:
  // 1. kResourceExhausted:  This indicates that EOS (end-of-stream) is reached
  //                         and subsequent calls to decode will continue to
  //                         return to this error.
  // 2. kFailedPrecondition: This indicates that initializing the underlying
  //                         GstreamerRunner has failed.
  // 3. kDeadlineExceeded:   This indicates that the decoder failed to return a
  //                         frame before timeout was reached.
  absl::Status Feed(const GstreamerBuffer& gstreamer_buffer, Args...);

  // Signals that no more inputs are to be fed and puts the decoder in an
  // irreversible disabled state. A new GstreamerDecoder needs to be created to
  // continue decoding.
  void SignalEOS();

  // Blocks until the decoder has completed or if the timeout expires.
  // Returns true if the pipeline has completed; otherwise, return false.
  //
  // Signaling EOS is a precursor to completion, so it must be either signaled
  // manually or via the stream.
  bool WaitUntilCompleted(absl::Duration timeout) const;

  ~GstreamerAsyncMotionDecoder() {
    SignalEOS();
    WaitUntilCompleted(absl::Seconds(5));
  }

 private:
  using AssociatedData = std::tuple<Args...>;

  absl::Status Initialize(const GstreamerBuffer&);

  static std::string MotionDecodeString() { return "avdec_h264 debug-mv=true"; }

  static absl::Status EOSStatus() {
    return absl::ResourceExhaustedError("Reached EOS");
  }

  bool eos_signaled_ = false;
  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;

  Callback callback_;

  absl::Duration feed_timeout_;

  // Holds the data to be associated with the output of decoding. Associated
  // data is wrapped in a unique_ptr to avoid invoking the default constructors
  // of the underlying types in AssociatedData.
  ProducerConsumerQueue<std::unique_ptr<AssociatedData>> pcqueue_;

  absl::Mutex feed_mutex_;
};

template <class... Args>
absl::Status GstreamerAsyncMotionDecoder<Args...>::Initialize(
    const GstreamerBuffer& gstreamer_buffer) {
  // Create a GstreamerRunner with a generic decoding pipeline
  GstreamerRunner::Options gstreamer_runner_options;
  gstreamer_runner_options.appsrc_caps_string = gstreamer_buffer.caps_string();
  gstreamer_runner_options.processing_pipeline_string = MotionDecodeString();

  gstreamer_runner_options.receiver_callback =
      [this](GstreamerBuffer gstreamer_buffer) -> absl::Status {
    // Block until data is available.
    std::unique_ptr<AssociatedData> data;
    this->pcqueue_.Pop(data);
    if (data == nullptr) {
      absl::Status status = absl::InternalError("Associated data is a nullptr");
      LOG(ERROR) << status;
      return status;
    }

    // Trigger the callback, supply with the associated data as params.
    // For more info on apply, look at C++27 std::apply as this is a near clone.
    VAI_RETURN_IF_ERROR(::visionai::apply(
        [this, &gstreamer_buffer](Args... args) -> absl::Status {
          return this->callback_(ToMotionVectors(std::move(gstreamer_buffer)),
                                 std::forward<Args>(args)...);
        },
        *data));
    return absl::OkStatus();
  };

  auto gstreamer_runner_statusor =
      GstreamerRunner::Create(gstreamer_runner_options);
  if (!gstreamer_runner_statusor.ok()) {
    LOG(ERROR) << gstreamer_runner_statusor.status();
    return absl::FailedPreconditionError(
        "Failed to create the GstreamerRunner");
  }
  gstreamer_runner_ = std::move(*gstreamer_runner_statusor);

  return absl::OkStatus();
}

template <class... Args>
absl::Status GstreamerAsyncMotionDecoder<Args...>::Feed(
    const GstreamerBuffer& gstreamer_buffer, Args... args) {
  absl::MutexLock lock(&feed_mutex_);

  if (eos_signaled_) {
    return EOSStatus();
  }

  if (gstreamer_runner_ == nullptr) {
    absl::Status status = Initialize(gstreamer_buffer);
    if (!status.ok()) {
      return status;
    }
  }

  absl::Status feed_status = gstreamer_runner_->Feed(gstreamer_buffer);
  if (absl::IsFailedPrecondition(feed_status)) {
    return EOSStatus();
  } else if (!feed_status.ok()) {
    return feed_status;
  }

  auto associated_data = std::make_unique<AssociatedData>(args...);
  auto associated_data_ptr = std::make_unique<std::unique_ptr<AssociatedData>>(
      std::move(associated_data));
  if (!pcqueue_.TryPush(associated_data_ptr, feed_timeout_)) {
    // Timeout exceeded.
    return absl::DeadlineExceededError("Decoding timeout deadline reached.");
  }

  return absl::OkStatus();
}

template <class... Args>
void GstreamerAsyncMotionDecoder<Args...>::SignalEOS() {
  if (gstreamer_runner_) {
    gstreamer_runner_->SignalEOS();
    eos_signaled_ = true;
  }
}

template <class... Args>
bool GstreamerAsyncMotionDecoder<Args...>::WaitUntilCompleted(
    absl::Duration timeout) const {
  if (gstreamer_runner_) {
    return gstreamer_runner_->WaitUntilCompleted(timeout);
  } else {
    return true;
  }
}

}  // namespace visionai
#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_MOTION_DECODER_H_
