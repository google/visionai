/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_ENCODED_MOTION_FILTER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_ENCODED_MOTION_FILTER_H_

#include <deque>
#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector.h"
#include "visionai/algorithms/media/gstreamer_async_motion_decoder.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/framework/filter_def_registry.h"
#include "visionai/streams/util/h264_frame_buffer.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/producer_consumer_queue.h"

namespace visionai {

// A filter that operates on video streams and passes through only the portion
// that contains motion. It operates directly on encoded streams and does not do
// any transcoding inside the filter.
//
// Init(): Called from the main thread to initialize the module.
// Run(): Called from a worker thread to actually filter data.
// Cancel(): Called from the main thread to stop the worker thread.
class EncodedMotionFilter : public Filter {
 public:
  EncodedMotionFilter() = default;
  ~EncodedMotionFilter() override = default;

  absl::Status Init(FilterInitContext* ctx) override;
  absl::Status Run(FilterRunContext* ctx) override;
  absl::Status Cancel() override;

  // Wait until async motion decoder to finish. This is for unit test only. In
  // the real production environment, `ingester_app` will wait for a certain
  // period after `Cancel()` is called.
  bool WaitUntilCompleted(absl::Duration timeout);

 private:
  // A buffer of frames in the past (specified by lookback_window_duration_).
  H264FrameBuffer frame_buffer_;

  absl::Duration min_event_duration_;
  absl::Duration lookback_window_duration_;
  absl::Duration cool_down_period_duration_;
  absl::Duration poll_timeout_;

  std::string event_id_;
  absl::Notification is_cancelled_;

  // Motion detector and the configs.
  motion_detection::MotionVectorBasedMotionDetectorConfig
      mv_motion_detector_config_;
  std::unique_ptr<visionai::motion_detection::MotionVectorBasedMotionDetector>
      mv_motion_detector_;

  // The time point when the cooldown period ends.
  absl::Time cooldown_until_ = absl::UnixEpoch();
  absl::Time latest_motion_detection_time_ = absl::UnixEpoch();

  // Flag to indicate whether the current GOP is active.
  bool current_gop_active_ = false;

  // Counter of consecutively frames with motion detected.
  int consecutive_motion_detections_ = 0;

  // The mininum number of frames needed to trigger a motion event.
  int min_frames_trigger_motion_ = 0;

  // PCQueue for accessing motion vectors from the async motion decoder.
  std::unique_ptr<ProducerConsumerQueue<TimedFrame>> pcqueue_;

  // Async Gstreamer Motion Decoder
  std::unique_ptr<GstreamerAsyncMotionDecoder<>> motion_decoder_;

  // Duration of video that has been filtered.
  absl::Duration total_filtered_time_ = absl::ZeroDuration();

  // The timestamp offset for gstreamer buffer packets. The DTS (Decoding
  // TimeStamp) and PTS (Presentation TimeStamp) in gstreamer buffer is relevant
  // timestamps, and may or may not start from zero. Here we use this field to
  // convert DTS or PTS to an absolute timestamp.
  absl::Duration gbuf_ts_offset_ = absl::ZeroDuration();

  // Timestamp of first frame.
  absl::Time first_timestamp_;

  // Checks if a new motion event is starting.
  bool CheckEventStart();

  // Get duration of time that has passed in the video since the first frame.
  absl::Duration GetTimeInVideo(absl::Time timestamp);

  // Get total duraction of video that has been filtered.
  absl::Duration GetTotalTimeFiltered();

  // Updates `consecutive_motion_detections_` and updates
  // `latest_motion_detection_time_` after detections are observed for
  // `min_frames_trigger_motion_` times in a row.
  void UpdateMotionDetection(bool motion_prediction,
                             const absl::Time& timestamp);

  // Detects motions from the motion vectors of the next frame and pushes the
  // packet in the output buffer when needed.
  absl::Status RunInternal(const MotionVectors& motion_vectors,
                           FilterRunContext* ctx);

  // Initializes the motion filter after getting the first frame from the input
  // buffer.
  absl::Status InitInternal(const GstreamerBuffer& first_frame);

  // Gets frame resolution information based on the `caps_string` in the
  // `GstreamerBuffer`.
  absl::Status GetFrameResolution(const std::string& caps_string,
                                  int& frame_width, int& frame_height);

  // Friend class for accessing private functions.
  friend class EncodedMotionFilterTestPeer;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_FILTERS_ENCODED_MOTION_FILTER_H_
