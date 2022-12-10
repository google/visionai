// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/read_write_channel.h"

#include <memory>
#include <thread>

#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/client/picked_notification.h"

namespace visionai {
namespace streams_internal {

TEST(ReadWriteChannelTest, ReaderUnblock) {
  ReadWriteChannel<int> read_write_channel(1);
  std::thread reader([&read_write_channel]() {
    int result;
    bool ok;
    EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                  &result, &ok));
    EXPECT_FALSE(ok);
  });
  read_write_channel.writer()->Close();
  reader.join();
}

TEST(ReadWriteChannelTest, ReaderTimeout) {
  ReadWriteChannel<int> read_write_channel(1);
  int result = 42;
  bool ok = true;
  EXPECT_FALSE(
      read_write_channel.reader()->Read(absl::ZeroDuration(), &result, &ok));
  EXPECT_EQ(result, 42);
  EXPECT_EQ(ok, true);
}

TEST(ReadWriteChannelTest, WriterCancel) {
  ReadWriteChannel<int> read_write_channel(1);
  std::thread writer([&read_write_channel]() {
    bool ok;
    EXPECT_TRUE(
        read_write_channel.writer()->Write(absl::InfiniteDuration(), 1, &ok));
    EXPECT_FALSE(ok);
  });
  read_write_channel.CancelWriters();
  writer.join();
}

TEST(ReadWriteChannelTest, WriterTimeout) {
  ReadWriteChannel<int> read_write_channel(1);
  bool ok;
  EXPECT_TRUE(
      read_write_channel.writer()->Write(absl::ZeroDuration(), 42, &ok));
  EXPECT_EQ(ok, true);
  EXPECT_FALSE(
      read_write_channel.writer()->Write(absl::ZeroDuration(), 42, &ok));
  EXPECT_EQ(ok, true);
}

TEST(ReadWriteChannelTest, WriterAlreadyCancelled) {
  ReadWriteChannel<int> read_write_channel(1);
  read_write_channel.CancelWriters();
  bool ok = true;
  EXPECT_TRUE(
      read_write_channel.writer()->Write(absl::InfiniteDuration(), 1, &ok));
  EXPECT_FALSE(ok);
}

TEST(ReadWriteChannelTest, WriterAlreadyClosed) {
  ReadWriteChannel<int> read_write_channel(1);
  read_write_channel.writer()->Close();
  int result = 42;
  bool ok = true;
  EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                &result, &ok));
  EXPECT_EQ(result, 42);
  EXPECT_EQ(ok, false);
}

TEST(ReadWriteChannelTest, OneItemBufferTest) {
  ReadWriteChannel<int> read_write_channel(1);
  std::thread writer([&read_write_channel]() {
    for (int i = 0; i < 6; ++i) {
      bool ok;
      EXPECT_TRUE(
          read_write_channel.writer()->Write(absl::InfiniteDuration(), i, &ok));
      EXPECT_TRUE(ok);
    }
    read_write_channel.writer()->Close();
  });

  int result;
  bool ok;
  for (int i = 0; i < 6; ++i) {
    EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                  &result, &ok));
    EXPECT_EQ(result, i);
    EXPECT_TRUE(ok);
  }
  EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                &result, &ok));
  EXPECT_EQ(result, 5);
  EXPECT_FALSE(ok);
  writer.join();
}

TEST(ReadWriteChannelTest, MultiItemBufferTest) {
  ReadWriteChannel<int> read_write_channel(3);
  std::thread writer([&read_write_channel]() {
    for (int i = 0;; ++i) {
      bool ok;
      EXPECT_TRUE(
          read_write_channel.writer()->Write(absl::InfiniteDuration(), i, &ok));
      if (!ok) {
        break;
      }
    }
  });
  for (int i = 0; i < 3; ++i) {
    int result;
    bool ok;
    EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                  &result, &ok));
    EXPECT_EQ(result, i);
    EXPECT_TRUE(ok);
  }
  read_write_channel.CancelWriters();
  writer.join();
}

TEST(ReadWriteChannelTest, MultiItemSerialTest) {
  ReadWriteChannel<int> read_write_channel(6);
  {
    bool ok;
    for (int i = 0; i < 6; ++i) {
      EXPECT_TRUE(
          read_write_channel.writer()->Write(absl::InfiniteDuration(), i, &ok));
      EXPECT_TRUE(ok);
    }
    EXPECT_FALSE(
        read_write_channel.writer()->Write(absl::ZeroDuration(), 42, &ok));
    EXPECT_TRUE(ok);
    read_write_channel.writer()->Close();
  }

  {
    int result;
    bool ok;
    for (int i = 0; i < 6; ++i) {
      EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                    &result, &ok));
      EXPECT_EQ(result, i);
      EXPECT_TRUE(ok);
    }
    EXPECT_TRUE(read_write_channel.reader()->Read(absl::InfiniteDuration(),
                                                  &result, &ok));
    EXPECT_EQ(result, 5);
    EXPECT_FALSE(ok);
  }
}

TEST(ReadWriteChannelTest, SelectForWriteEventTest) {
  {
    auto n = std::make_shared<PickedNotification>();
    ReadWriteChannel<int> ch0(1);
    ReadWriteChannel<int> ch1(1);
    ReadWriteChannel<int> ch2(1);
    ReadWriteChannel<int> ch3(1);
    std::thread t([&ch3]() { ch3.writer()->Close(); });
    auto pick = SelectForWriteEvent(
        {ch0.reader(), ch1.reader(), ch2.reader(), ch3.reader()});
    EXPECT_EQ(pick, 3);
    t.join();
  }
}

}  // namespace streams_internal
}  // namespace visionai
