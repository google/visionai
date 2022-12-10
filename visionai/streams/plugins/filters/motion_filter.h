/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_MOTION_FILTER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_MOTION_FILTER_H_

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/detection/motion_detection/opencv_motion_detector.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"
#include "visionai/types/raw_image.h"

namespace visionai {

// A filter that operates on video streams and passes through only the portion
// that contains motion.
//
// Init(): Called from the main thread to initialize the module.
// Run(): Called from a worker thread to actually filter data.
// Cancel(): Called from the main thread to stop the worker thread.
class MotionFilter : public Filter {
 public:
  MotionFilter() = default;
  ~MotionFilter() override = default;

  absl::Status Init(FilterInitContext* ctx) override;
  absl::Status Run(FilterRunContext* ctx) override;
  absl::Status Cancel() override;

 private:
  struct TimedRawImage {
    absl::Time timestamp;
    RawImage image;
  };
  // A buffer of images in the past (specified by lookback_window_in_seconds_).
  std::deque<TimedRawImage> image_buffer_;

  absl::Duration min_event_duration_;
  absl::Duration lookback_window_duration_;
  absl::Duration cool_down_period_duration_;
  absl::Duration poll_timeout_;

  absl::Notification is_cancelled_;
  std::unique_ptr<visionai::motion_detection::OpenCVMotionDetector>
      opencv_motion_detector_;
  // Time remaining in the cooldown period.
  absl::Duration cooldown_timer_;
  absl::Duration time_elapsed_since_last_packet_;
  absl::Time latest_motion_detection_time_;
  bool motion_event_started_;

  // Counter of consecutively frames with motion detected.
  int consecutive_motion_detections_;

  // Check if a new motion event is starting and update the event start time
  // accordingly. Returns true if a new motion event starts.
  bool CheckAndUpdateEventStartTime(bool motion_prediction);
  // Check if we have an active motion event going on.
  bool CurrentEventActive();
  // Decide if we are in a cool down period where we do not run motion detection
  // but simply filter all the incoming packets. After a motion event ends,
  // there will be a cool down period configured by
  // cool_down_period_in_seconds_.
  bool InCoolDown();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_MOTION_FILTER_H_
