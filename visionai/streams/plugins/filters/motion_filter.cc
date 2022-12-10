// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/filters/motion_filter.h"

#include <memory>
#include <string>
#include <vector>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/detection/motion_detection/opencv_motion_detector.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {
// Default configuration for the motion filter.
constexpr int kBackgroundHistoryFrameLength = 5;
constexpr float kVarianceThresholdNumPix = 16;
constexpr int kMotionForegroundPixelThreshold = 120;
constexpr float kImageScale = 0.1;
constexpr float kMotionAreaThreshold = 0.01;
constexpr absl::Duration kTimeOut = absl::Seconds(10);
constexpr absl::Duration kMinEventDuration = absl::Seconds(10);
constexpr absl::Duration kLookBackWindowDuration = absl::Seconds(3);
constexpr absl::Duration kCoolDownPeriodDuration = absl::Minutes(5);
// The minimum number of consecutive motion frames to be considered as the start
// of a new motion event.
// TODO: make it depends on fps.
constexpr int kMinMotionFramesForEvent = 3;
}  // namespace

absl::Status MotionFilter::Init(FilterInitContext* ctx) {
  // Get declared options.
  float variance_threshold_num_pix = kVarianceThresholdNumPix;
  int background_history_frame_length = kBackgroundHistoryFrameLength;
  int motion_foreground_pixel_threshold = kMotionForegroundPixelThreshold;
  float scale = kImageScale;
  bool shadow_detection = false;
  float motion_area_threshold = kMotionAreaThreshold;
  int time_out_in_ms = 0;

  // Get attributes to initialize the motion detector.
  // TODO: take a config instead.
  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("variance_threshold_num_pix", &variance_threshold_num_pix));
  if (variance_threshold_num_pix <= 0) {
    LOG(WARNING) << absl::StrCat(
        "The variance_threshold_num_pix must be positive. Got ",
        variance_threshold_num_pix,
        ". Reset to default: ", kVarianceThresholdNumPix);
    variance_threshold_num_pix = kVarianceThresholdNumPix;
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr("background_history_frame_length",
                               &background_history_frame_length));
  if (background_history_frame_length <= 0) {
    LOG(WARNING) << absl::StrCat(
        "The background_history_frame_length must be positive. Got ",
        background_history_frame_length,
        ". Reset to default: ", kBackgroundHistoryFrameLength);
    background_history_frame_length = kBackgroundHistoryFrameLength;
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr("shadow_detection", &shadow_detection));
  VAI_RETURN_IF_ERROR(ctx->GetAttr("scale", &scale));
  if (scale < 0 || scale > 1) {
    LOG(WARNING) << absl::StrCat("The scale must be [0, 1]. Got ", scale,
                                 ". Reset to default: ", kImageScale);
    scale = kImageScale;
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr("motion_foreground_pixel_threshold",
                               &motion_foreground_pixel_threshold));
  if (motion_foreground_pixel_threshold < 0 ||
      motion_foreground_pixel_threshold > 255) {
    LOG(WARNING) << absl::StrCat(
        "The motion_foreground_pixel_threshold must be [0, 255]. Got ",
        motion_foreground_pixel_threshold,
        ". Reset to default: ", kMotionForegroundPixelThreshold);
    motion_foreground_pixel_threshold = kMotionForegroundPixelThreshold;
  }
  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("motion_area_threshold", &motion_area_threshold));
  if (motion_area_threshold < 0 || motion_area_threshold > 1) {
    LOG(WARNING) << absl::StrCat(
        "The motion_area_threshold must be [0, 1]. Got ", motion_area_threshold,
        ". Reset to default: ", kMotionAreaThreshold);
    motion_area_threshold = kMotionAreaThreshold;
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr("time_out_in_ms", &time_out_in_ms));
  if (time_out_in_ms > 0) {
    poll_timeout_ = absl::Milliseconds(time_out_in_ms);
  } else {
    poll_timeout_ = kTimeOut;
  }

  ::visionai::motion_detection::OpenCVMotionDetectorConfig config;
  config.set_variance_threshold_num_pix(variance_threshold_num_pix);
  config.set_background_history_frame_length(background_history_frame_length);
  config.set_shadow_detection(shadow_detection);
  config.set_scale(scale);
  config.set_motion_foreground_pixel_threshold(
      motion_foreground_pixel_threshold);
  config.set_motion_area_threshold(motion_area_threshold);

  opencv_motion_detector_ =
      std::make_unique<::visionai::motion_detection::OpenCVMotionDetector>(
          config);

  // Get attributes to initialize the motion filter.
  int min_event_duration_in_seconds = 0;
  int lookback_window_duration_in_seconds = 0;
  int cool_down_period_duration_in_seconds = 0;
  VAI_RETURN_IF_ERROR(ctx->GetAttr("min_event_length_in_seconds",
                               &min_event_duration_in_seconds));
  if (min_event_duration_in_seconds <= 0) {
    LOG(WARNING) << absl::StrCat("The min_event_length must be positive. Got ",
                                 min_event_duration_in_seconds,
                                 ". Resetting "
                                 "to default value ")
                 << kMinEventDuration;
    min_event_duration_ = kMinEventDuration;
  } else {
    min_event_duration_ = absl::Seconds(min_event_duration_in_seconds);
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("lookback_window_in_seconds",
                               &lookback_window_duration_in_seconds));
  if (lookback_window_duration_in_seconds < 0) {
    LOG(WARNING) << absl::StrCat("The lookback_window can't be negative. Got ",
                                 lookback_window_duration_in_seconds,
                                 ". Resetting "
                                 "to default value ")
                 << kLookBackWindowDuration;
    lookback_window_duration_ = kLookBackWindowDuration;
  } else {
    lookback_window_duration_ =
        absl::Seconds(lookback_window_duration_in_seconds);
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("cool_down_period_in_seconds",
                               &cool_down_period_duration_in_seconds));
  if (cool_down_period_duration_in_seconds <= 0) {
    LOG(WARNING) << absl::StrCat("The cool_down_period must be positive. Got ",
                                 cool_down_period_duration_in_seconds,
                                 ". Resetting "
                                 "to default value ")
                 << kCoolDownPeriodDuration;
    cool_down_period_duration_ = kCoolDownPeriodDuration;
  } else {
    cool_down_period_duration_ =
        absl::Seconds(cool_down_period_duration_in_seconds);
  }

  motion_event_started_ = false;
  consecutive_motion_detections_ = 0;
  cooldown_timer_ = absl::ZeroDuration();

  return absl::OkStatus();
}

absl::Status MotionFilter::Run(FilterRunContext* ctx) {
  std::string event_id;
  Packet current_packet;
  while (!is_cancelled_.HasBeenNotified()) {
    VAI_RETURN_IF_ERROR(ctx->Poll(&current_packet, poll_timeout_));
    PacketAs<RawImage> p_as_image(current_packet);
    if (!p_as_image.status().ok()) {
      return p_as_image.status();
    }
    TimedRawImage timed_raw_image;
    // TODO: what we really want is PTS.
    timed_raw_image.timestamp = GetCaptureTime(current_packet);
    VLOG(2) << "motion timestamp: " << timed_raw_image.timestamp;
    timed_raw_image.image = std::move(*p_as_image);
    image_buffer_.emplace_back(std::move(timed_raw_image));
    VLOG(2) << "motion buffer size : " << image_buffer_.size();

    time_elapsed_since_last_packet_ =
        image_buffer_.back().timestamp - image_buffer_.front().timestamp;
    // Make sure images in the buffer do not exceed the duration of
    // lookback_window_in_seconds_.
    if (time_elapsed_since_last_packet_ > lookback_window_duration_) {
      VLOG(2) << "Pop from the lookback buffer.";
      image_buffer_.pop_front();
    }
    // Filter all the frames during the cool down period.
    if (InCoolDown()) {
      continue;
    }

    // Note that the background model needs a buffer of frames to update so
    // everytime it requires sometime after the cool down for the background
    // model to be updated.
    VAI_ASSIGN_OR_RETURN(
        auto motion_prediction,
        opencv_motion_detector_->DetectMotion(image_buffer_.back().image));
    // Check if a new motion event is starting and update the event start time
    // according.
    bool start_new_event = CheckAndUpdateEventStartTime(motion_prediction);
    bool event_active = CurrentEventActive();
    if (!event_active) {
      if (motion_event_started_) {
        VAI_RETURN_IF_ERROR(ctx->EndEvent(event_id));
        motion_event_started_ = false;
        VLOG(2) << "motion filter event ended";
        // Start cool down period.
        cooldown_timer_ = cool_down_period_duration_;
      } else if (start_new_event) {
        VAI_ASSIGN_OR_RETURN(event_id, ctx->StartEvent());
        motion_event_started_ = true;
        VLOG(2) << "motion filter event started";
        // Push all the packets in the lookback window to event writer.
        while (!image_buffer_.empty()) {
          VAI_ASSIGN_OR_RETURN(
              auto raw_image_gstreamer_buffer,
              ToGstreamerBuffer(std::move(image_buffer_.front().image)));
          VAI_ASSIGN_OR_RETURN(auto pkt,
                           MakePacket(std::move(raw_image_gstreamer_buffer)));
          VAI_RETURN_IF_ERROR(ctx->Push(event_id, pkt));
          image_buffer_.pop_front();
        }
      }
    } else {
      // In this state, the current motion event is active so we simply push the
      // latest input packet to the event writer. There should be no pile up in
      // the image_buffer.
      VAI_ASSIGN_OR_RETURN(
          auto raw_image_gstreamer_buffer,
          ToGstreamerBuffer(std::move(image_buffer_.front().image)));
      VAI_ASSIGN_OR_RETURN(auto pkt,
                       MakePacket(std::move(raw_image_gstreamer_buffer)));
      VAI_RETURN_IF_ERROR(ctx->Push(event_id, pkt));
      image_buffer_.pop_front();
    }
  }
  return absl::OkStatus();
}

absl::Status MotionFilter::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

bool MotionFilter::InCoolDown() {
  if (cooldown_timer_ > time_elapsed_since_last_packet_) {
    // TODO: we really want cooldown_timer_ = cooldown_timer -
    // absl::Nanoseconds(GST_BUFFER_DURATION(gstbuffer_from_packet));
    cooldown_timer_ = cooldown_timer_ - time_elapsed_since_last_packet_;
    VLOG(2) << "motion filter in cooldown";
    return true;
  } else {
    cooldown_timer_ = absl::ZeroDuration();
    return false;
  }
}

bool MotionFilter::CheckAndUpdateEventStartTime(bool motion_prediction) {
  if (motion_prediction) {
    ++consecutive_motion_detections_;
    if (consecutive_motion_detections_ >= kMinMotionFramesForEvent)
      // Update the event start time to the latest detection motion timestamp so
      // that we can decide if the current motion event is still active
      // correctly.
      latest_motion_detection_time_ = image_buffer_.back().timestamp;
  } else {
    consecutive_motion_detections_ = 0;
  }

  return consecutive_motion_detections_ == kMinMotionFramesForEvent;
}

bool MotionFilter::CurrentEventActive() {
  // A motion event is active if the motion event already started and the time
  // elapsed since last consecutive motion detection is less than
  // min_event_length_in_seconds_.
  if (motion_event_started_ &&
      image_buffer_.back().timestamp - latest_motion_detection_time_ <
          min_event_duration_) {
    VLOG(2) << "motion filter event active";
    return true;
  } else {
    return false;
  }
}

REGISTER_FILTER_INTERFACE("MotionFilter")
    .Attr("variance_threshold_num_pix", "float")
    .Attr("background_history_frame_length", "int")
    .Attr("shadow_detection", "bool")
    .Attr("scale", "float")
    .Attr("motion_foreground_pixel_threshold", "int")
    .Attr("motion_area_threshold", "float")
    .Attr("time_out_in_ms", "int")
    .Attr("min_event_length_in_seconds", "int")
    .Attr("lookback_window_in_seconds", "int")
    .Attr("cool_down_period_in_seconds", "int")
    .Doc(
        "MotionFilter is to filter out video segments that do not contain "
        "motion.");
REGISTER_FILTER_IMPLEMENTATION("MotionFilter", MotionFilter);

}  // namespace visionai
