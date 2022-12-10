// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_UTILS_H264_FRAME_BUFFER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_UTILS_H264_FRAME_BUFFER_H_

#include <deque>

#include "absl/time/time.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/gstreamer_buffer.h"

namespace visionai {
struct TimedFrame {
  absl::Time timestamp = absl::UnixEpoch();
  Packet frame;
  bool is_key_frame = false;
};

// A queue to buffer frames inside the lookback window for the encoded motion
// filter.
//
// Assuming the timestamp of the current frame is `t`, then the looback window
// will be `[t - lookback_window_duration_, t]`.
//
// Hereby we define a GOP (group of pictures) as a series of non-key frames
// (B-frames or P-frames), bounded by two key frames (i.e., I-frames):
// I, B, B, P, B, ..., I
//
// Inside one GOP, although the non-key frames may not necessarily have
// monotonically increasing timestamps, the first key frame has the earliest
// timestamp and the second key frame has the latest timestamp.
// So if both key frames are out of the lookback window, then we can safely say
// this GOP is out of the lookback window.
// On the other hand, if either key frame is in the lookback window (only need
// to check the second one since it has the newest timestamp), this GOP is still
// in the lookback window.
class H264FrameBuffer {
 public:
  int Size() const;
  bool Empty() const;
  TimedFrame& Front();
  void Clear();

  // Appends a new `TimedFrame` to the back.
  void Push(TimedFrame&& elem);

  // Pops the `TimedFrame` from the front.
  void Pop();

  // Drops all GOPs before `look_back_window_boundary`.
  //
  // It keeps track of the second key frame timestamp for the
  // GOP in the front. And if the second key frame timestamp is already out of
  // the lookback window, it pops all the frames in the front GOP except for the
  // second key frame.
  void UpdateLookBackWindow(const absl::Time& look_back_window_boundary);

 private:
  // Called after elements are popped out.
  void UpdateSecondKeyFrameTimestamp();

  std::deque<TimedFrame> buffer_;
  absl::Time second_key_frame_timestamp_ = absl::UnixEpoch();
  int num_key_frames_ = 0;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_UTILS_H264_FRAME_BUFFER_H_
