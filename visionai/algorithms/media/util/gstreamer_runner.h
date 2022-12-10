// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_GSTREAMER_RUNNER_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_GSTREAMER_RUNNER_H_

#include <atomic>
#include <functional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "visionai/types/gstreamer_buffer.h"

namespace visionai {

// This class runs an arbitrary gstreamer pipeline.
//
// It also supports the ability to directly feed/fetch in-memory
// data to/from gstreamer.
//
// Please see the GstreamerRunner::Options for how to configure it.
class GstreamerRunner {
 public:
  using ReceiverCallback = std::function<absl::Status(GstreamerBuffer)>;

  // Options for configuring the gstreamer runner.
  struct Options {
    // REQUIRED: The gstreamer pipeline string to run.
    //
    // This is the same string that you would normally feed to gst-launch:
    // gst-launch <processing-pipeline-string>
    std::string processing_pipeline_string;

    // OPTIONAL: If non-empty, an appsrc will be prepended to the processing
    // pipeline with caps set to this string.
    std::string appsrc_caps_string;

    // OPTIONAL: If non-empty, an appsink will be appended after the main
    // processing pipeline to deliver the result through the given callback.
    // If the callback has been called, it won't be called again until the
    // previous callback has returned. This is to avoid returning outputs out of
    // order.
    ReceiverCallback receiver_callback;

    // ----------------------------------------------
    // System configurations. Power users only.

    // Value of "sync" for appsink.
    bool appsink_sync = false;

    // Value of "do_timestamps" for appsrc.
    // Only set true if the pts of the buffer should be determined by the time
    // it enters the gstreamer pipeline.
    bool appsrc_do_timestamps = true;
  };

  // Create and run a gstreamer pipeline.
  static absl::StatusOr<std::unique_ptr<GstreamerRunner>> Create(
      const Options&);

  // Feed a GstreamerBuffer object for processing.
  //
  // This is available only if you enable it in the Options.
  absl::Status Feed(const GstreamerBuffer&) const;

  // Returns true if the pipeline has completed; otherwise, false.
  bool IsCompleted() const;

  // Blocks until the pipeline has completed or if the timeout expires.
  // Returns true if the pipeline has completed; otherwise, false.
  bool WaitUntilCompleted(absl::Duration timeout) const;

  // Send EOS. A call to this will eventually put the runner into a 'Completed'
  // state, which means subsequent calls to Feed may fail.
  void SignalEOS();

  // Copy-control members.
  //
  // Please use the GstreamerRunner::Create rather than constructors directly.
  GstreamerRunner();
  ~GstreamerRunner();
  GstreamerRunner(const GstreamerRunner&) = delete;
  GstreamerRunner& operator=(const GstreamerRunner&) = delete;
  GstreamerRunner(GstreamerRunner&&) = delete;
  GstreamerRunner& operator=(GstreamerRunner&&) = delete;

 private:
  class GstreamerRunnerImpl;
  std::unique_ptr<GstreamerRunnerImpl> gstreamer_runner_impl_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_MEDIA_UTIL_GSTREAMER_RUNNER_H_
