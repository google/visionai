// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/util/h264_frame_buffer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/time/time.h"
#include "visionai/types/gstreamer_buffer.h"

namespace visionai {
namespace {

TimedFrame CreateTimedFrame(int ts_secs, bool is_key_frame) {
  TimedFrame timed_frame{/*.timestamp = */ absl::FromUnixSeconds(ts_secs),
                         /*.frame = */ Packet(),
                         /* is_key_frame = */ is_key_frame};
  return timed_frame;
}

TEST(H264FrameBufferTest, BasicTest) {
  H264FrameBuffer buffer;
  TimedFrame frame0 = CreateTimedFrame(0, true);
  TimedFrame frame1 = CreateTimedFrame(1, true);
  TimedFrame frame2 = CreateTimedFrame(2, false);
  TimedFrame frame3 = CreateTimedFrame(3, true);

  buffer.Push(std::move(frame0));
  buffer.Push(std::move(frame1));
  // Current state: {0, 1}.
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 0);

  buffer.Pop();
  // Current state: {1}.
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 1);

  buffer.Push(std::move(frame2));
  buffer.Push(std::move(frame3));
  // Current State: {1, 2, 3}.

  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(3));
  // Current state: {3}.
  EXPECT_EQ(buffer.Size(), 1);
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 3);
}

TEST(H264FrameBufferTest, FirstFrameIsNotKeyFrame) {
  H264FrameBuffer buffer;
  TimedFrame frame0 = CreateTimedFrame(0, false);
  TimedFrame frame1 = CreateTimedFrame(1, true);
  TimedFrame frame2 = CreateTimedFrame(2, false);
  TimedFrame frame3 = CreateTimedFrame(3, true);

  buffer.Push(std::move(frame0));
  buffer.Push(std::move(frame1));
  // Current state: {0, 1}.
  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(1));
  // Current state: {0, 1}.
  EXPECT_EQ(buffer.Size(), 2);

  buffer.Push(std::move(frame2));
  buffer.Push(std::move(frame3));
  // Current State: {0, 1, 2, 3}.

  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(3));
  // Current state: {3}.
  EXPECT_EQ(buffer.Size(), 1);
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 3);
}

TEST(H264FrameBufferTest, AllKeyFrames) {
  H264FrameBuffer buffer;
  TimedFrame frame0 = CreateTimedFrame(0, true);
  TimedFrame frame1 = CreateTimedFrame(1, true);
  TimedFrame frame2 = CreateTimedFrame(2, true);
  TimedFrame frame3 = CreateTimedFrame(3, true);

  buffer.Push(std::move(frame0));
  buffer.Push(std::move(frame1));
  buffer.Push(std::move(frame2));
  buffer.Push(std::move(frame3));
  // Current state: {0, 1, 2, 3}.

  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(0));
  // Current state: {0, 1, 2, 3}.
  EXPECT_EQ(buffer.Size(), 4);

  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(1));
  // Current state: {1, 2, 3}.
  EXPECT_EQ(buffer.Size(), 3);
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 1);

  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(2));
  // Current state: {2, 3}.
  EXPECT_EQ(buffer.Size(), 2);
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 2);

  buffer.UpdateLookBackWindow(absl::FromUnixSeconds(3));
  // Current state: {3}.
  EXPECT_EQ(buffer.Size(), 1);
  EXPECT_EQ(absl::ToUnixSeconds(buffer.Front().timestamp), 3);
}

}  // namespace
}  // namespace visionai
