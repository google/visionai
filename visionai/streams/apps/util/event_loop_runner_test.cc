// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/util/event_loop_runner.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/client/mock_event_update_receiver.h"
#include "visionai/streams/client/mock_packet_receiver.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

class MockEventWriter : public EventWriter {
 public:
  explicit MockEventWriter() : EventWriter() {}
  MOCK_METHOD(absl::Status, Init, (EventWriterInitContext * ctx), (override));
  MOCK_METHOD(absl::Status, Open, (absl::string_view event_id), (override));
  MOCK_METHOD(absl::Status, Write, (Packet packet), (override));
  MOCK_METHOD(absl::Status, Close, (), (override));

  void SetCallback(OffsetCommitCallback callback) { callback_ = callback; }
  OffsetCommitCallback callback_;
};

std::shared_ptr<MockPacketReceiver>
CreateMockPacketReceiverExpectNoPackets() {
  std::shared_ptr<MockPacketReceiver> packet_receiver =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());
  {
    InSequence s;
    EXPECT_CALL(*packet_receiver, Receive(_, _, _))
        .WillOnce([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = false;
          return true;
        });
    EXPECT_CALL(*packet_receiver, CommitsDone()).WillOnce(Return());
    EXPECT_CALL(*packet_receiver, Finish())
        .WillOnce(Return(absl::OutOfRangeError("event ended")));
  }
  return packet_receiver;
}

std::shared_ptr<MockEventWriter> CreateMockEventWriterExpectNoPackets() {
  std::shared_ptr<MockEventWriter> event_writer =
      std::make_shared<MockEventWriter>();
  EXPECT_CALL(*event_writer, Close()).WillOnce(Return(absl::OkStatus()));
  return event_writer;
}

EventUpdate CreateEventUpdate(int64_t offset, std::string event) {
  EventUpdate event_update;
  event_update.set_offset(offset);
  event_update.set_event(event);
  return event_update;
}

TEST(EventLoopRunnerTest, NoEvents) {
  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options) {
        return CreateMockPacketReceiverExpectNoPackets();
      };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    return CreateMockEventWriterExpectNoPackets();
  };
  std::shared_ptr<MockEventUpdateReceiver> event_update_receiver =
      std::make_shared<MockEventUpdateReceiver>(EventUpdateReceiver::Options());
  EventReceiverFactory event_receiver_factory =
      [&](const EventUpdateReceiver::Options &options) {
        return event_update_receiver;
      };
  EXPECT_CALL(*event_update_receiver, Receive(_, _, _))
      .WillRepeatedly(
          [&](absl::Duration timeout, EventUpdate *event_update, bool *ok) {
            absl::SleepFor(absl::Seconds(1));
            return false;
          });

  EventLoopRunner::Options options;
  options.event_receiver_factory = event_receiver_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "controlled"};
  options.event_receiver_options = EventUpdateReceiver::Options();
  EventLoopRunner runner(options);

  std::thread t([&]() -> void {
    absl::SleepFor(absl::Seconds(1));
    runner.Cancel();
  });

  ASSERT_TRUE(runner.Run().ok());
  t.join();
}

TEST(EventLoopRunnerTest, SingleEventUpdateReceiver) {
  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options) {
        return CreateMockPacketReceiverExpectNoPackets();
      };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    return CreateMockEventWriterExpectNoPackets();
  };
  std::shared_ptr<MockEventUpdateReceiver> event_update_receiver =
      std::make_shared<MockEventUpdateReceiver>(EventUpdateReceiver::Options());
  EventReceiverFactory event_receiver_factory =
      [&](const EventUpdateReceiver::Options &options) {
        return event_update_receiver;
      };

  std::vector<EventUpdate> event_updates = {
      CreateEventUpdate(1001, "ev-1001"), CreateEventUpdate(1002, "ev-1002"),
      CreateEventUpdate(1003, "ev-1003")};

  int index = 0;
  EXPECT_CALL(*event_update_receiver, Receive(_, _, _))
      .WillRepeatedly(
          [&](absl::Duration timeout, EventUpdate *event_update, bool *ok) {
            absl::SleepFor(absl::Milliseconds(20));
            if (index < event_updates.size()) {
              *event_update = event_updates[index++];
              *ok = true;
              return true;
            }
            return false;
          });
  EXPECT_CALL(*event_update_receiver, Commit(_, _, _))
      .Times(3)
      .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
        *ok = true;
        return true;
      });

  EventLoopRunner::Options options;
  options.event_receiver_factory = event_receiver_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "controlled"};
  options.event_receiver_options = EventUpdateReceiver::Options();
  EventLoopRunner runner(options);

  std::thread t([&]() -> void {
    absl::SleepFor(absl::Seconds(2));
    runner.Cancel();
  });

  ASSERT_TRUE(runner.Run().ok());
  t.join();
}

TEST(ConsumeSequentialEventTest, EventUpdateReceiverRestart) {
  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options) {
        return CreateMockPacketReceiverExpectNoPackets();
      };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    return CreateMockEventWriterExpectNoPackets();
  };
  std::shared_ptr<MockEventUpdateReceiver> event_update_receiver =
      std::make_shared<MockEventUpdateReceiver>(EventUpdateReceiver::Options());

  std::shared_ptr<MockEventUpdateReceiver> event_update_receiver_1 =
      std::make_shared<MockEventUpdateReceiver>(EventUpdateReceiver::Options());

  std::vector<EventUpdate> event_updates_1 = {
      CreateEventUpdate(1001, "ev-1001"), CreateEventUpdate(1002, "ev-1002"),
      CreateEventUpdate(1003, "ev-1003")};
  int index_1 = 0;
  int commit_count = 0;
  {
    InSequence seq;
    EXPECT_CALL(*event_update_receiver_1, Receive(_, _, _))
        .WillRepeatedly(
            [&](absl::Duration timeout, EventUpdate *event_update, bool *ok) {
              absl::SleepFor(absl::Milliseconds(20));
              if (index_1 < event_updates_1.size()) {
                *event_update = event_updates_1[index_1++];
                *ok = true;
                return true;
              }
              *ok = false;
              return true;
            });
    EXPECT_CALL(*event_update_receiver_1, CommitsDone()).WillOnce(Return());
    EXPECT_CALL(*event_update_receiver_1, Finish())
        .WillOnce(Return(absl::OkStatus()));
  }
  EXPECT_CALL(*event_update_receiver_1, Commit(_, _, _))
      .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
        commit_count++;
        *ok = true;
        return true;
      });

  std::shared_ptr<MockEventUpdateReceiver> event_update_receiver_2 =
      std::make_shared<MockEventUpdateReceiver>(EventUpdateReceiver::Options());

  std::vector<EventUpdate> event_updates_2 = {
      CreateEventUpdate(1004, "ev-1004")};
  int index_2 = 0;
  EXPECT_CALL(*event_update_receiver_2, Receive(_, _, _))
      .WillRepeatedly(
          [&](absl::Duration timeout, EventUpdate *event_update, bool *ok) {
            // sleep so that the event can be comitted.
            absl::SleepFor(absl::Milliseconds(20));
            if (index_2 < event_updates_2.size()) {
              *event_update = event_updates_2[index_2++];
              *ok = true;
              return true;
            }
            absl::SleepFor(absl::Milliseconds(100));
            return false;
          });
  EXPECT_CALL(*event_update_receiver_2, Commit(_, _, _))
      .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
        commit_count++;
        *ok = true;
        return true;
      });

  int event_receiver_count = 0;
  EventReceiverFactory event_receiver_factory =
      [&](const EventUpdateReceiver::Options &options)
      -> absl::StatusOr<std::shared_ptr<EventUpdateReceiver>> {
    event_receiver_count++;
    if (event_receiver_count == 1) {
      return event_update_receiver_1;
    }
    if (event_receiver_count == 2) {
      return event_update_receiver_2;
    }
    return absl::InternalError("no available receiver");
  };

  EventLoopRunner::Options options;
  options.event_receiver_factory = event_receiver_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "controlled"};
  options.event_receiver_options = EventUpdateReceiver::Options();
  EventLoopRunner runner(options);

  std::thread t([&]() -> void {
    absl::SleepFor(absl::Seconds(2));
    runner.Cancel();
  });

  ASSERT_TRUE(runner.Run().ok());
  t.join();
  EXPECT_GE(commit_count, 4);
}

}  // namespace visionai
