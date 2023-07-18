// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_JPEG_ENCODER_H
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_JPEG_ENCODER_H

#include <memory>
#include <string>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/tuple_apply.h"

namespace visionai {

struct OutputImageDimension {
  int width = 0;
  int height = 0;
};

// Encodes the raw image arriving as a stream of GstreamerBuffers to JPEG.
//
// TODO(b/231159621): This class is very similar to GstreamerAsyncDecoder class.
// Consider to refactor it in the future.
//
// Call Feed() to give GstreamerBuffer to the encoder. Once the encoder has
// finished encoding the input, it will return a GstreamerBuffer, wrapped in a
// StatusOr, via the callback, and any other data that should be associated with
// that output. The associated data is stored in a queue and returned with the
// GstreamerBuffer within the callback.
//
// NOTE: Only 1 callback may be active at a time, more data will not be returned
// until the previous callback has returned. This is to prevent race conditions.
//
// Example:
//   GstreamerAsyncJpegEncoder<> encoder([](absl::StatusOr<GstreamerBuffer>
//   image) {
//     // Do something with image.
//   });
//   encoder.Feed(input_gstreamer_buffer);
//
// Example:
//   GstreamerAsyncJpegEncoder<int64> encoder([](absl::StatusOr<GstreamerBuffer>
//   image, int64 timestamp) {
//     emit(timestamp, *image);
//   });
//   encoder.Feed(input_gstreamer_buffer, 12345);
//
// Example:
//   GstreamerAsyncJpegEncoder<ProcessContext>
//   encoder([](absl::StatusOr<GstreamerBuffer> image, ProcessContext context) {
//     context.Emit(image);
//   });
//   encoder.Feed(input_gstreamer_buffer, some_context);
//
//
// NOTE: This class is thread-unsafe.
template <class... Args>
class GstreamerAsyncJpegEncoder {
 public:
  using Callback =
      std::function<void(absl::StatusOr<GstreamerBuffer>, Args...)>;

  // Create an instance. The instance is fully initialized when 'Feed' is called
  // for the first time. When a encoded result is ready, the callback will be
  // called. If the queue is full, Feed will block until there is room in the
  // queue, or until feed_timeout is reached.
  //
  // If EOS is reached, the callback will be supplied with a kResourceExhausted
  // error.
  GstreamerAsyncJpegEncoder(Callback callback,
                            absl::StatusOr<OutputImageDimension> dimension =
                                absl::Status(absl::StatusCode::kUnavailable,
                                             "not initialized"),
                            absl::StatusOr<std::string> local_file_path =
                                absl::Status(absl::StatusCode::kUnavailable,
                                             "not initialized"),
                            size_t queue_size = 300,
                            absl::Duration feed_timeout = absl::Seconds(60))
      : callback_(callback),
        dimension_(dimension),
        local_file_path_(local_file_path),
        feed_timeout_(feed_timeout),
        pcqueue_(queue_size) {}

  // Feeds the given GstreamerBuffer into the encoder, which will emit a
  // GstreamerBuffer utilizing the information found in the buffer. Any
  // associated data can also be added.
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
  // `kDeadlineExceeded`:   This indicates that the encoder failed to return a
  //                        frame before the timeout was reached.
  absl::Status Feed(const GstreamerBuffer &gstreamer_buffer, Args...);

  // Signals that no more inputs are to be fed and puts the encoder in an
  // irreversible disabled state. A new GstreamerAsyncJpegEncoder needs to be
  // created to continue encoding.
  void SignalEOS();

  // Blocks until the encoder has completed or if the timeout expires.
  // Returns true if the pipeline has completed; otherwise, false.
  //
  // Signaling EOS is a precursor to completion, so it must be either signaled
  // manually or via the stream.
  bool WaitUntilCompleted(absl::Duration timeout) const;

  // Disable copying.
  GstreamerAsyncJpegEncoder(const GstreamerAsyncJpegEncoder &) = delete;
  GstreamerAsyncJpegEncoder &operator=(const GstreamerAsyncJpegEncoder &) =
      delete;
  GstreamerAsyncJpegEncoder(GstreamerAsyncJpegEncoder &&) = delete;
  GstreamerAsyncJpegEncoder &operator=(GstreamerAsyncJpegEncoder &&) = delete;

  ~GstreamerAsyncJpegEncoder() { SignalEOS(); }

 private:
  using AssociatedData = std::tuple<Args...>;

  absl::Status Initialize(const GstreamerBuffer &);

  std::string JpegEncodeString() {
    std::vector<std::string> pipeline_elements;
    if (dimension_.ok()) {
      pipeline_elements.push_back("videoscale");
      pipeline_elements.push_back(
          absl::StrFormat("video/x-raw,width=%d,height=%d",
                          dimension_->width,
                          dimension_->height));
    }

    pipeline_elements.push_back("jpegenc");

    if (local_file_path_.ok()) {
      // Using the tee gst elelemt to branch the data flow.
      // One branch will send the JPEG images to streams service.
      // Another branch will save the JPEG images in local.
      pipeline_elements.push_back("tee name=t");
      pipeline_elements.push_back("queue");
      pipeline_elements.push_back(absl::StrCat("multifilesink location=",
                                               local_file_path_.value(),
                                               " t."));
      pipeline_elements.push_back("queue");
    }
    return absl::StrJoin(pipeline_elements, " ! ");
  }

  static absl::Status EOSStatus() {
    return absl::ResourceExhaustedError("Reached EOS");
  }

  bool eos_signaled_ = false;
  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;

  Callback callback_;

  absl::StatusOr<OutputImageDimension> dimension_;

  // The file directory of the path must be existed,
  // otherwise the gstreamer will throw an error.
  //
  // Sample Format: local_file_path_ = ${local_dir_}/%d.jpg
  // Reference: https://gstreamer.freedesktop.org/documentation/multifile/multifilesink.html?gi-language=c#multifilesink:location
  absl::StatusOr<std::string> local_file_path_;

  absl::Duration feed_timeout_;

  // Holds the data to be associated with the output of decoding. Associated
  // data is wrapped in a unique_ptr to avoid invoking the default constructors
  // of the underlying types in AssocaitedData.
  ProducerConsumerQueue<std::unique_ptr<AssociatedData>> pcqueue_;

  absl::Mutex feed_mutex_;
};

template <class... Args>
absl::Status GstreamerAsyncJpegEncoder<Args...>::Initialize(
    const GstreamerBuffer &gstreamer_buffer) {
  if (dimension_.ok() &&
      (dimension_->width < 0 || dimension_->height < 0)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("The given output image dimension can't be negative "
                        "(width=%d, height=%d)",
                        dimension_->width, dimension_->height));
  }

  // Create a GstreamerRunner with the jpeg encoding pipeline.
  GstreamerRunner::Options gstreamer_runner_options;
  gstreamer_runner_options.appsrc_caps_string = gstreamer_buffer.caps_string();
  gstreamer_runner_options.processing_pipeline_string = JpegEncodeString();

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
    // Trigger the callback, supplying it the associated data as params. For
    // more info on `apply`, look at C++17 std::apply, its a near clone.
    ::visionai::apply(
        [this, &gstreamer_buffer](Args... args) {
          this->callback_(std::move(gstreamer_buffer),
                          std::forward<Args>(args)...);
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
absl::Status GstreamerAsyncJpegEncoder<Args...>::Feed(
    const GstreamerBuffer &gstreamer_buffer, Args... args) {
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
    return absl::DeadlineExceededError("Encoding timeout deadline reached.");
  }

  return absl::OkStatus();
}

template <class... Args>
void GstreamerAsyncJpegEncoder<Args...>::SignalEOS() {
  gstreamer_runner_.reset();
  eos_signaled_ = true;
}

template <class... Args>
bool GstreamerAsyncJpegEncoder<Args...>::WaitUntilCompleted(
    absl::Duration timeout) const {
  if (gstreamer_runner_) {
    return gstreamer_runner_->WaitUntilCompleted(timeout);
  } else {
    return true;
  }
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_GSTREAMER_ASYNC_JPEG_ENCODER_H
