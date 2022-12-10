// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/util/h264_frame_buffer.h"

#include "glog/logging.h"

namespace visionai {

int H264FrameBuffer::Size() const { return buffer_.size(); }

bool H264FrameBuffer::Empty() const { return buffer_.empty(); }

TimedFrame& H264FrameBuffer::Front() { return buffer_.front(); }

void H264FrameBuffer::Push(TimedFrame&& elem) {
  if (!buffer_.empty() && buffer_.front().timestamp >= elem.timestamp) {
    LOG(WARNING) << "The timestamps are expected to be monotonically "
                    "increasing for the frames. Got backward DTS now.";
  }
  buffer_.push_back(std::move(elem));
  if (buffer_.back().is_key_frame) num_key_frames_++;
  UpdateSecondKeyFrameTimestamp();
}

void H264FrameBuffer::Pop() {
  if (buffer_.front().is_key_frame) {
    num_key_frames_--;
    second_key_frame_timestamp_ = absl::UnixEpoch();
    UpdateSecondKeyFrameTimestamp();
  }
  buffer_.pop_front();
}

void H264FrameBuffer::Clear() {
  num_key_frames_ = 0;
  second_key_frame_timestamp_ = absl::UnixEpoch();
  buffer_.clear();
}

void H264FrameBuffer::UpdateLookBackWindow(
    const absl::Time& look_back_window_boundary) {
  while (num_key_frames_ >= 2 &&
         second_key_frame_timestamp_ <= look_back_window_boundary) {
    // Remove the GOP in the front except for the second key frame.
    bool found_first_key_frame = false;
    int second_key_frame_idx = 0;
    for (int i = 0; i < buffer_.size(); ++i) {
      if (buffer_[i].is_key_frame) {
        if (found_first_key_frame) {
          // Found the second key frame in the buffer.
          second_key_frame_idx = i;
          break;
        } else {
          // Found the first key frame in the buffer.
          found_first_key_frame = true;
        }
      }
    }
    buffer_.erase(buffer_.begin(), buffer_.begin() + second_key_frame_idx);
    num_key_frames_--;
    second_key_frame_timestamp_ = absl::UnixEpoch();

    UpdateSecondKeyFrameTimestamp();
  }
}

void H264FrameBuffer::UpdateSecondKeyFrameTimestamp() {
  // After key frames are popped, we iterate over the buffer to find the
  // second key frame to update `second_key_frame_timestamp_`.
  // Although the iteration seems time-consuming, the amortized time for each
  // pop operation is O(1).
  if (num_key_frames_ >= 2) {
    bool found_first_key_frame = false;
    for (int i = 0; i < buffer_.size(); ++i) {
      if (buffer_[i].is_key_frame) {
        if (found_first_key_frame) {
          // Found the second key frame in the buffer.
          second_key_frame_timestamp_ = buffer_[i].timestamp;
          break;
        } else {
          // Found the first key frame in the buffer.
          found_first_key_frame = true;
        }
      }
    }
  }
}

}  // namespace visionai
