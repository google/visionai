// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_DECODER_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_DECODER_H_

#include <algorithm>
#include <memory>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "absl/types/variant.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/tuple_apply.h"

namespace visionai {

// Decodes video arriving as a stream of GstreamerBuffers using Gstreamer.
//
// Call Feed() to give GstreamerBuffer to the decoder. Once the decoder has
// finished decoding the input, it will return a RawImage, wrapped in a
// StatusOr, via the callback, and any other data that should be associated with
// that output. The associated data is stored in a queue and returned with the
// RawImage within the callback.
//
// Output frame rate can be controlled by passing `output_period_nanos` (p) to
// the constructor. If `output_period_nanos` is larger than 0, at most one
// `Callback` defined by the user will be called with the corresponding frame,
// for every `p` seconds. However, the number that `receiver_callback` for the
// GstreamerRunner is called will not change.
//
// NOTE: Only 1 callback may be active at a time, more data will not be returned
// until the previous callback has returned. This is to prevent race conditions.
//
// Example:
//   GstreamerAsyncDecoder<> decoder([](absl::StatusOr<RawImage> image) {
//     // Do something with image.
//   });
//   decoder.Feed(input_gstreamer_buffer);
//
// Example:
//   GstreamerAsyncDecoder<int64> decoder([](absl::StatusOr<RawImage> image,
//   int64 timestamp) {
//     emit(timestamp, *image);
//   });
//   decoder.Feed(input_gstreamer_buffer, 12345);
//
// Example:
//   GstreamerAsyncDecoder<ProcessContext> decoder([](absl::StatusOr<RawImage>
//   image, ProcessContext context) {
//     context.Emit(image);
//   });
//   decoder.Feed(input_gstreamer_buffer, some_context);
//
// Example:
//   GstreamerAsyncDecoder<ProcessContext> decoder(
//     [](absl::StatusOr<RawImage> image, ProcessContext context) {
//       context.Emit(image);
//     },
//     /*queue_size =*/300, /*feed_timeout =*/absl::Seconds(60),
//     /* output_period_nanos =*/ 1000000000/6); // 6 fps
//
//   decoder.Feed(input_gstreamer_buffer, some_context);
//
//
// NOTE: This class is thread-unsafe.
template <class... Args>
class GstreamerAsyncDecoder {
 public:
  using RawImageCallback =
      std::function<void(absl::StatusOr<RawImage>, Args...)>;
  using GstreamerBufferCallback =
      std::function<void(absl::StatusOr<GstreamerBuffer>, Args...)>;
  using Callback = absl::variant<RawImageCallback, GstreamerBufferCallback>;

  // Create an instance. The instance is fully initialized when 'Feed' is called
  // for the first time. When a decoded result is ready, the callback will be
  // called. If the queue is full, Feed will block until there is room in the
  // queue, or until feed_timeout is reached.
  //
  // If EOS is reached, the callback will be supplied with a kResourceExhausted
  // error.
  GstreamerAsyncDecoder(Callback callback, size_t queue_size = 300,
                        absl::Duration feed_timeout = absl::Seconds(60),
                        int64_t output_period_nanos = 0)
      : callback_(callback),
        feed_timeout_(feed_timeout),
        pcqueue_(queue_size),
        output_period_nanos_(output_period_nanos) {}

  // Disable copying and moving.
  GstreamerAsyncDecoder(const GstreamerAsyncDecoder&) = delete;
  GstreamerAsyncDecoder& operator=(const GstreamerAsyncDecoder&) = delete;
  GstreamerAsyncDecoder(GstreamerAsyncDecoder&&) = delete;
  GstreamerAsyncDecoder& operator=(GstreamerAsyncDecoder&&) = delete;

  // Feeds the given GstreamerBuffer into the decoder, which will emit a
  // RawImage utilizing the information found in the buffer and information from
  // previously decoded frames. Any associated data can also be added.
  //
  // The first call will be slightly slower than subsequent calls because it
  // will initialize the underlying GstreamerRunner.
  //
  // If the internal queue is full, this function will block until either the
  // queue has free space or `feed_timeout`, as specified in the constructor, is
  // reached.
  //
  // Feeding is protected by a Mutex to avoid buffer for t=1 being associated
  // with data t=2 (if two threads are executing 'feed' simultaneously).
  //
  // Below are error codes that may require special handling:
  // `kResourceExhausted`:  This indicates that EOS (end-of-stream) is
  //                        reached and subsequent calls to Decode will continue
  //                        to return this error.
  // `kFailedPrecondition`: This indicates that initializing the underlying
  //                        GstreamerRunner failed.
  // `kDeadlineExceeded`:   This indicates that the decoder failed to return a
  //                        frame before the timeout was reached.
  absl::Status Feed(const GstreamerBuffer& gstreamer_buffer, Args...);

  // Signals that no more inputs are to be fed and puts the decoder in an
  // irreversible disabled state. A new GStreamerDecoder needs to be created to
  // continue decoding.
  void SignalEOS();

  // Blocks until the decoder has completed or if the timeout expires.
  // Returns true if the pipeline has completed; otherwise, false.
  //
  // Signaling EOS is a precursor to completion, so it must be either signaled
  // manually or via the stream.
  bool WaitUntilCompleted(absl::Duration timeout) const;

  ~GstreamerAsyncDecoder() {
    SignalEOS();
    WaitUntilCompleted(absl::Seconds(5));
  }

 private:
  using AssociatedData = std::tuple<Args...>;

  absl::Status Initialize(const GstreamerBuffer&);

  static std::string GenericDecodeString() {
    return "decodebin ! videoconvert ! video/x-raw,format=RGB";
  }

  static absl::Status EOSStatus() {
    return absl::ResourceExhaustedError("Reached EOS");
  }

  bool eos_signaled_ = false;
  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;

  Callback callback_;

  absl::Duration feed_timeout_;

  // Holds the data to be associated with the output of decoding. Associated
  // data is wrapped in a unique_ptr to avoid invoking the default constructors
  // of the underlying types in AssocaitedData.
  ProducerConsumerQueue<std::unique_ptr<AssociatedData>> pcqueue_;

  absl::Mutex feed_mutex_;

  // The next frame to emit will be the earliest frame inside the timestamp
  // range [`start_pts_nanos_`, `start_pts_nanos_` + `output_period_nanos_`].
  // After a frame is emitted, `start_pts_nanos_` will be incremented by
  // `output_period_nanos_` so that the output window moves by
  // `output_period_nanos_`.
  int64_t output_period_nanos_ = 0;
  int64_t start_pts_nanos_ = -1;
};

template <class... Args>
absl::Status GstreamerAsyncDecoder<Args...>::Initialize(
    const GstreamerBuffer& gstreamer_buffer) {
  // Create a GstreamerRunner with a generic decoding pipeline.
  GstreamerRunner::Options gstreamer_runner_options;
  gstreamer_runner_options.appsrc_caps_string = gstreamer_buffer.caps_string();
  gstreamer_runner_options.processing_pipeline_string = GenericDecodeString();

  if (output_period_nanos_ < 0) {
    return absl::InvalidArgumentError(absl::StrCat(
        "The output period in nanoseconds must have non-negative values. Got ",
        output_period_nanos_, " instead."));
  }

  gstreamer_runner_options.receiver_callback =
      [this](GstreamerBuffer gstreamer_buffer) -> absl::Status {
    // Block until data is available.
    std::unique_ptr<AssociatedData> data;
    this->pcqueue_.Pop(data);

    // The output frame rate is limited by `output_period_nanos_`.
    if (this->output_period_nanos_ > 0) {
      int64_t cur_pts = gstreamer_buffer.get_pts();
      if (this->start_pts_nanos_ == -1) {
        this->start_pts_nanos_ = cur_pts;
      }
      if (cur_pts < this->start_pts_nanos_) {
        // Drops a frame if it is before `start_pts_nanos_`.
        return absl::OkStatus();
      } else if (cur_pts >=
                 this->start_pts_nanos_ + this->output_period_nanos_) {
        // Moves the output window to include the current frame if the frame is
        // after the output window.
        this->start_pts_nanos_ =
            this->start_pts_nanos_ + (cur_pts - this->start_pts_nanos_) /
                                         this->output_period_nanos_ *
                                         this->output_period_nanos_;
      }
      // Now we have a frame inside the output window. Then we move the output
      // window by `output_period_nanos_`.
      this->start_pts_nanos_ += this->output_period_nanos_;
    }

    if (data == nullptr) {
      absl::Status status = absl::InternalError("Associated data is a nullptr");
      LOG(ERROR) << status;
      return status;
    }
    // Trigger the callback, supplying it the associated data as params. For
    // more info on `apply`, look at C++17 std::apply, its a near clone.
    ::visionai::apply(
        [this, &gstreamer_buffer](Args... args) {
          if (absl::holds_alternative<RawImageCallback>(this->callback_)) {
            absl::get<RawImageCallback>(this->callback_)(
                ToRawImage(std::move(gstreamer_buffer)),
                std::forward<Args>(args)...);
          } else {
            absl::get<GstreamerBufferCallback>(this->callback_)(
                std::move(gstreamer_buffer), std::forward<Args>(args)...);
          }
        },
        *data);
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
absl::Status GstreamerAsyncDecoder<Args...>::Feed(
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
void GstreamerAsyncDecoder<Args...>::SignalEOS() {
  if (gstreamer_runner_) {
    gstreamer_runner_->SignalEOS();
    eos_signaled_ = true;
  }
}

template <class... Args>
bool GstreamerAsyncDecoder<Args...>::WaitUntilCompleted(
    absl::Duration timeout) const {
  if (gstreamer_runner_) {
    return gstreamer_runner_->WaitUntilCompleted(timeout);
  } else {
    return true;
  }
}

}  // namespace visionai
#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_DECODER_H_
