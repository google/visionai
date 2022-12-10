// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#include "visionai/streams/plugins/filters/encoded_motion_filter.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector.h"
#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector_config.pb.h"
#include "visionai/algorithms/media/gstreamer_async_motion_decoder.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/motion_vector.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace {
// Default configuration for the motion filter.
constexpr int kCoolDownPeriodDurationInSeconds = 300;
constexpr int kCoolDownPeriodDurationInSecondsWarningLimit = 3600;
constexpr int kLookbackWindowInSeconds = 3;
constexpr int kLookbackWindowInSecondsWarningLimit = 300;
constexpr int kMinEventDurationInSeconds = 10;
constexpr int kMinEventDurationInSecondsWarningLimit = 3600;
constexpr int kSecondsBetweenLogOutput = 300;
constexpr int kSpatialGridNumber = 3;
constexpr int kSpatialGridNumberWarningLimit = 10;
constexpr int kTemporalBufferFrames = 8;
constexpr int kPollTimeOutInMs = 10000;
constexpr char kHighMotionDetectionSensitivity[] = "high";
constexpr char kMediumMotionDetectionSensitivity[] = "medium";
constexpr char kLowMotionDetectionSensitivity[] = "low";
constexpr float kHighMotionSensitityThreshold = 0.3;
constexpr float kMediumMotionSensitivityThreshold = 0.6;
constexpr float kLowMotionSensitivityThreshold = 0.9;

// The minimum number of consecutive motion frames to be considered as the start
// of a new motion event.
// TODO(yukunma): make it depends on fps.
constexpr int kMinMotionFramesForEvent = 3;
constexpr absl::Duration kFeedTimeout = absl::Seconds(10);
constexpr int kPcQueueSize = 30;
}  // namespace

absl::Status EncodedMotionFilter::Init(FilterInitContext* ctx) {
  // Get declared options.
  int spatial_grid_number = kSpatialGridNumber;
  int temporal_buffer_frames = kTemporalBufferFrames;
  std::string motion_detection_sensitivity = kMediumMotionDetectionSensitivity;

  // Get attributes to initialize the motion detector.
  VAI_RETURN_IF_ERROR(ctx->GetAttr("spatial_grid_number", &spatial_grid_number));
  if (spatial_grid_number <= 0) {
    LOG(WARNING) << absl::StrCat(
        "The spatial_grid_number must be positive. Got ", spatial_grid_number,
        ". Reset to default: ", kSpatialGridNumber);
    spatial_grid_number = kSpatialGridNumber;
  } else if (spatial_grid_number > kSpatialGridNumberWarningLimit) {
    LOG(WARNING) << absl::StrFormat(
        "Extreme high `spatial_grid_number` may bring undesired impact to the "
        "motion filter accuracy (currently it's %d). Please only alternate "
        "this parameter if you understand its usage.",
        spatial_grid_number);
  }

  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("temporal_buffer_frames", &temporal_buffer_frames));
  if (temporal_buffer_frames <= 0) {
    LOG(WARNING) << absl::StrCat(
        "The temporal_buffer_frames must be positive. Got ",
        temporal_buffer_frames, ". Reset to default: ", kTemporalBufferFrames);
    temporal_buffer_frames = kTemporalBufferFrames;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("motion_detection_sensitivity",
                               &motion_detection_sensitivity));
  if (motion_detection_sensitivity != kHighMotionDetectionSensitivity &&
      motion_detection_sensitivity != kMediumMotionDetectionSensitivity &&
      motion_detection_sensitivity != kLowMotionDetectionSensitivity) {
    LOG(WARNING) << absl::StrCat(
        "The motion_detection_sensitivity must be one of \"%s\", "
        "\"%s\", \"%s\". Got ",
        motion_detection_sensitivity,
        ". Reset to default: ", kMediumMotionDetectionSensitivity);
    motion_detection_sensitivity = kMediumMotionDetectionSensitivity;
  }

  float motion_sensitivity = 0.0;
  if (motion_detection_sensitivity == kHighMotionDetectionSensitivity) {
    motion_sensitivity = kHighMotionSensitityThreshold;
  } else if (motion_detection_sensitivity ==
             kMediumMotionDetectionSensitivity) {
    motion_sensitivity = kMediumMotionSensitivityThreshold;
  } else {  // motion_detection_sensitivity == kLowMotionDetectionSensitivity
    motion_sensitivity = kLowMotionSensitivityThreshold;
  }

  mv_motion_detector_config_.Clear();
  mv_motion_detector_config_.set_spatial_grid_number(spatial_grid_number);
  mv_motion_detector_config_.set_temporal_buffer_frames(temporal_buffer_frames);
  mv_motion_detector_config_.set_motion_sensitivity(motion_sensitivity);
  mv_motion_detector_.reset();

  // Get attributes to initialize the motion filter.
  int poll_time_out_in_ms = kPollTimeOutInMs;
  int min_event_duration_in_seconds = kMinEventDurationInSeconds;
  int cool_down_period_duration_in_seconds = kCoolDownPeriodDurationInSeconds;
  int lookback_window_in_seconds = kLookbackWindowInSeconds;
  min_frames_trigger_motion_ = kMinMotionFramesForEvent;

  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("min_frames_trigger_motion", &min_frames_trigger_motion_));
  if (min_frames_trigger_motion_ <= 0) {
    LOG(WARNING) << absl::StrCat(
        "The min_frames_trigger_motion must be positive. Got ",
        min_frames_trigger_motion_,
        ". Reset to default: ", kMinMotionFramesForEvent);
    min_frames_trigger_motion_ = kMinMotionFramesForEvent;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("time_out_in_ms", &poll_time_out_in_ms));
  if (poll_time_out_in_ms > 0) {
    poll_timeout_ = absl::Milliseconds(poll_time_out_in_ms);
  } else {
    LOG(WARNING) << absl::StrCat(
        "The poll_time_out_in_ms must be positive. Got ", poll_time_out_in_ms,
        ". Reset to default: ", kPollTimeOutInMs);
    poll_timeout_ = absl::Milliseconds(kPollTimeOutInMs);
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("min_event_length_in_seconds",
                               &min_event_duration_in_seconds));
  if (min_event_duration_in_seconds <= 0) {
    LOG(WARNING)
        << absl::StrCat(
               "The min_event_length_in_seconds must be positive. Got ",
               min_event_duration_in_seconds, ". Resetting to default value ")
        << kMinEventDurationInSeconds;
    min_event_duration_ = absl::Seconds(kMinEventDurationInSeconds);
  } else {
    if (min_event_duration_in_seconds >
        kMinEventDurationInSecondsWarningLimit) {
      LOG(WARNING) << absl::StrFormat(
          "More non-motion frames may be included in the filter output if "
          "`min_event_length_in_seconds` is set too high (currently it's %d "
          "seconds). Please only alternate this parameter if you understand "
          "its usage.",
          min_event_duration_in_seconds);
    }
    min_event_duration_ = absl::Seconds(min_event_duration_in_seconds);
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("cool_down_period_in_seconds",
                               &cool_down_period_duration_in_seconds));
  if (cool_down_period_duration_in_seconds < 0) {
    LOG(WARNING)
        << absl::StrCat(
               "The cool_down_period_in_seconds must not be negative. Got ",
               cool_down_period_duration_in_seconds,
               ". Resetting to default value ")
        << kCoolDownPeriodDurationInSeconds;
    cool_down_period_duration_ =
        absl::Seconds(kCoolDownPeriodDurationInSeconds);
  } else {
    if (cool_down_period_duration_in_seconds >
        kCoolDownPeriodDurationInSecondsWarningLimit) {
      LOG(WARNING) << absl::StrFormat(
          "More frames will be skipped if "
          "`cool_down_period_duration_in_seconds` is set too high (currently "
          "it's %d seconds). Please only alternate this parameter if you "
          "understand its usage.",
          cool_down_period_duration_in_seconds);
    }
    cool_down_period_duration_ =
        absl::Seconds(cool_down_period_duration_in_seconds);
  }

  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("lookback_window_in_seconds", &lookback_window_in_seconds));
  if (lookback_window_in_seconds < 0) {
    LOG(WARNING) << absl::StrCat("The lookback_window can't be negative. Got ",
                                 lookback_window_in_seconds,
                                 ". Resetting "
                                 "to default value ")
                 << kLookbackWindowInSeconds;
    lookback_window_duration_ = absl::Seconds(kLookbackWindowInSeconds);
  } else {
    if (lookback_window_in_seconds > kLookbackWindowInSecondsWarningLimit) {
      LOG(WARNING) << absl::StrFormat(
          "More non-motion frames may be included in the filter output if "
          "`lookback_window_in_seconds` is set too high (currently it's %d "
          "seconds). Please only alternate this parameter if you understand "
          "its usage.",
          lookback_window_in_seconds);
    }
    lookback_window_duration_ = absl::Seconds(lookback_window_in_seconds);
  }

  consecutive_motion_detections_ = 0;
  cooldown_until_ = absl::UnixEpoch();
  current_gop_active_ = false;
  event_id_.clear();
  pcqueue_ = std::make_unique<ProducerConsumerQueue<TimedFrame>>(kPcQueueSize);
  gbuf_ts_offset_ = absl::ZeroDuration();

  return absl::OkStatus();
}

absl::Status EncodedMotionFilter::GetFrameResolution(
    const std::string& caps_string, int& frame_width, int& frame_height) {
  GstCaps* gst_caps = gst_caps_from_string(caps_string.c_str());
  int num_structures = gst_caps_get_size(gst_caps);

  bool got_shape_info = false;
  for (int i = 0; i < num_structures && !got_shape_info; i++) {
    GstStructure* gst_structure = gst_caps_get_structure(gst_caps, i);
    got_shape_info =
        gst_structure_get_int(gst_structure, "width", &frame_width) &
        gst_structure_get_int(gst_structure, "height", &frame_height);
  }
  if (!got_shape_info) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "`caps_string` does not have frame shape information. The contents of "
        "`caps_string` are: %s",
        caps_string));
  }
  gst_caps_unref(gst_caps);
  return absl::OkStatus();
}

absl::Status EncodedMotionFilter::InitInternal(
    const GstreamerBuffer& first_frame) {
  int frame_width = 0;
  int frame_height = 0;
  VAI_RETURN_IF_ERROR(
      GetFrameResolution(first_frame.caps_string(), frame_width, frame_height));
  first_timestamp_ = absl::Now();
  gbuf_ts_offset_ =
      first_timestamp_ - absl::FromUnixNanos(first_frame.get_dts());
  mv_motion_detector_ =
      std::make_unique<motion_detection::MotionVectorBasedMotionDetector>(
          mv_motion_detector_config_, frame_width, frame_height);
  return absl::OkStatus();
}

absl::Status EncodedMotionFilter::RunInternal(
    const MotionVectors& motion_vectors, FilterRunContext* ctx) {
  TimedFrame timed_frame;
  pcqueue_->Pop(timed_frame);
  absl::Time timestamp = timed_frame.timestamp;

  bool motion_prediction = mv_motion_detector_->DetectMotion(motion_vectors);
  UpdateMotionDetection(motion_prediction, timestamp);

  if (timed_frame.is_key_frame && current_gop_active_ &&
      latest_motion_detection_time_ + min_event_duration_ <
          timed_frame.timestamp) {
    // The motion event ends because it's been long enough since the last
    // motion detected.
    current_gop_active_ = false;
    cooldown_until_ = timed_frame.timestamp + cool_down_period_duration_;

    // Push this key frame since in some cases it is essential for decoding
    // previous frames.
    VAI_RETURN_IF_ERROR(ctx->Push(event_id_, timed_frame.frame));
    VAI_RETURN_IF_ERROR(ctx->EndEvent(event_id_));
  }

  // Pushes the current frame and discards GOPs out of lookback window.
  // TODO(yukunma): Avoid pushing key frames twice if two events are back to
  // back.
  frame_buffer_.Push(std::move(timed_frame));

  absl::Time previous_front = frame_buffer_.Front().timestamp;
  frame_buffer_.UpdateLookBackWindow(timestamp - lookback_window_duration_);
  total_filtered_time_ += frame_buffer_.Front().timestamp - previous_front;
  LOG_EVERY_T(INFO, kSecondsBetweenLogOutput)
      << "Total filtered time: " << GetTotalTimeFiltered()
      << "; Currently processing video time: "
      << GetTimeInVideo(timestamp);

  VLOG(2) << "motion buffer size : " << frame_buffer_.Size();

  // Filter all the frames during the cool down period.
  if (cooldown_until_ > timestamp) {
    return absl::OkStatus();
  }

  // Check if a new motion event is starting.
  bool start_new_event = CheckEventStart();

  if (current_gop_active_) {
    // In this state, the current motion event is active so we simply push the
    // latest input packet to the event writer. There should be no pile up in
    // `frame_buffer_`. The latest input packet happens to be the earliest in
    // the frame buffer as well since the buffer has only one frame now.
    if (frame_buffer_.Size() != 1) {
      return absl::InternalError(
          absl::StrFormat("There is supposed to be exactly one frame in the "
                          "frame buffer now, got %d instead.",
                          frame_buffer_.Size()));
    }
    VAI_RETURN_IF_ERROR(
        ctx->Push(event_id_, std::move(frame_buffer_.Front().frame)));
    frame_buffer_.Pop();
  } else if (start_new_event) {
    // A new event starts.
    VAI_ASSIGN_OR_RETURN(event_id_, ctx->StartEvent());
    current_gop_active_ = true;
    VLOG(2) << "motion filter event started";

    // Push all the frames in the buffer to event writer.
    while (!frame_buffer_.Empty()) {
      VAI_RETURN_IF_ERROR(
          ctx->Push(event_id_, std::move(frame_buffer_.Front().frame)));
      frame_buffer_.Pop();
    }
  }

  return absl::OkStatus();
}

absl::Status EncodedMotionFilter::Run(FilterRunContext* ctx) {
  Packet current_packet;
  motion_decoder_ = std::make_unique<GstreamerAsyncMotionDecoder<>>(
      [this, ctx](absl::StatusOr<MotionVectors> statusor_motion_vectors)
          -> absl::Status {
        VAI_RETURN_IF_ERROR(statusor_motion_vectors.status());
        VAI_RETURN_IF_ERROR(this->RunInternal(*statusor_motion_vectors, ctx));
        return absl::OkStatus();
      });
  bool is_first_frame = true;
  while (!is_cancelled_.HasBeenNotified()) {
    VAI_RETURN_IF_ERROR(ctx->Poll(&current_packet, poll_timeout_));
    Packet packet_for_gbuf = current_packet;
    PacketAs<GstreamerBuffer> packet_as_gbuf(packet_for_gbuf);
    VAI_RETURN_IF_ERROR(packet_as_gbuf.status());
    if (is_first_frame) {
      VAI_RETURN_IF_ERROR(InitInternal(*packet_as_gbuf));
      is_first_frame = false;
    }

    // TODO(b/246077382): Use presentation timestamp (PTS) here.
    absl::Time timestamp =
        absl::FromUnixNanos(packet_as_gbuf->get_dts()) + gbuf_ts_offset_;
    bool is_key_frame = packet_as_gbuf->is_key_frame();
    auto timed_frame = std::make_unique<TimedFrame>(TimedFrame{
        /*.timestamp = */ timestamp, /*.frame = */ std::move(current_packet),
        /*.is_key_frame = */ is_key_frame});

    // Feeds the frame into the decoder. After the motion vectors are extracted,
    // `RunInternal` will be called in the callback of the decoder.
    VAI_RETURN_IF_ERROR(motion_decoder_->Feed(*packet_as_gbuf));
    if (!pcqueue_->TryPush(timed_frame, kFeedTimeout)) {
      // Timeout exceeded.
      return absl::DeadlineExceededError(
          "Frame processing timeout deadline reached.");
    }
  }
  return absl::OkStatus();
}

absl::Status EncodedMotionFilter::Cancel() {
  is_cancelled_.Notify();
  motion_decoder_->SignalEOS();
  return absl::OkStatus();
}

bool EncodedMotionFilter::WaitUntilCompleted(absl::Duration timeout) {
  return motion_decoder_->WaitUntilCompleted(timeout);
}

bool EncodedMotionFilter::CheckEventStart() {
  return consecutive_motion_detections_ == min_frames_trigger_motion_;
}

absl::Duration EncodedMotionFilter::GetTotalTimeFiltered() {
  return total_filtered_time_;
}

absl::Duration EncodedMotionFilter::GetTimeInVideo(absl::Time timestamp) {
  return timestamp - first_timestamp_;
}

void EncodedMotionFilter::UpdateMotionDetection(bool motion_prediction,
                                                const absl::Time& timestamp) {
  if (motion_prediction) {
    ++consecutive_motion_detections_;
    if (consecutive_motion_detections_ >= min_frames_trigger_motion_)
      // Update the event start time to the latest detection motion timestamp
      // so that we can decide if the current motion event is still active
      // correctly.
      latest_motion_detection_time_ = timestamp;
  } else {
    consecutive_motion_detections_ = 0;
  }
}

REGISTER_FILTER_INTERFACE("EncodedMotionFilter")
    .Attr("spatial_grid_number", "int")
    .Attr("temporal_buffer_frames", "int")
    .Attr("motion_detection_sensitivity", "string")
    .Attr("min_frames_trigger_motion", "int")
    .Attr("min_event_length_in_seconds", "int")
    .Attr("lookback_window_in_seconds", "int")
    .Attr("cool_down_period_in_seconds", "int")
    .Attr("time_out_in_ms", "int")
    .Doc(
        "EncodedMotionFilter is to filter out video segments that do not "
        "contain motion.");
REGISTER_FILTER_IMPLEMENTATION("EncodedMotionFilter", EncodedMotionFilter);

}  // namespace visionai
