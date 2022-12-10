// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/util/packet_loop_runner.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "visionai/streams/client/constants.h"
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

TEST(PacketLoopRunnerTest, NoPackets) {
  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options) {
        return CreateMockPacketReceiverExpectNoPackets();
      };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    return CreateMockEventWriterExpectNoPackets();
  };
  PacketLoopRunner::Options options;
  options.current_event = CreateEventUpdate(0, "ev-0");
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "controlled"};
  options.event_commit_callback = [](int64_t offset) -> void { return; };
  PacketLoopRunner runner(options);
  ASSERT_TRUE(runner.Run().ok());
}

TEST(PacketLoopRunnerTest, SinglePacketReceiverControlledMode) {
  std::shared_ptr<MockPacketReceiver> packet_receiver =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());
  std::shared_ptr<MockEventWriter> event_writer =
      std::make_shared<MockEventWriter>();

  std::vector<int64_t> packet_offsets{101, 102, 103};
  int index = 0;
  {
    InSequence s;
    // Timeouts.
    EXPECT_CALL(*packet_receiver, Receive(_, _, _))
        .Times(2)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p, bool *ok) {
          return false;
        });
    // Receive 3 packets.
    EXPECT_CALL(*packet_receiver, Receive(_, _, _))
        .Times(3)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = true;
          Packet packet;
          SetOffset(packet_offsets[index], &packet).IgnoreError();
          index++;
          *p = packet;
          return true;
        });
    // Timeouts.
    EXPECT_CALL(*packet_receiver, Receive(_, _, _))
        .Times(2)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p, bool *ok) {
          return false;
        });
    // Finish with OUT_OF_RANGE.
    EXPECT_CALL(*packet_receiver, Receive(_, _, _))
        .WillOnce([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = false;
          return true;
        });
    EXPECT_CALL(*packet_receiver, CommitsDone()).WillOnce(Return());
    EXPECT_CALL(*packet_receiver, Finish())
        .WillOnce(Return(absl::OutOfRangeError("event ended")));
  }
  EXPECT_CALL(*packet_receiver, Commit(_, _, _))
      .Times(3)
      .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
        *ok = true;
        return true;
      });

  // Set up event writer to process 3 packets.
  {
    InSequence s;
    // Unavailable error to be ignored.
    EXPECT_CALL(*event_writer, Write(_)).WillOnce([&](Packet packet) {
      return absl::UnavailableError("unavailable");
    });
    // Process 3 packets.
    EXPECT_CALL(*event_writer, Write(_))
        .Times(3)
        .WillRepeatedly([&](Packet packet) {
          event_writer->callback_(GetOffset(packet));
          return absl::OkStatus();
        });
    EXPECT_CALL(*event_writer, Close()).WillOnce(Return(absl::OkStatus()));
  }

  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options) {
        return packet_receiver;
      };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    event_writer->SetCallback(callback);
    return event_writer;
  };

  PacketLoopRunner::Options options;
  options.current_event = CreateEventUpdate(0, "ev-0");
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "controlled"};
  options.event_commit_callback = [](int64_t offset) -> void { return; };
  PacketLoopRunner runner(options);
  ASSERT_TRUE(runner.Run().ok());
}

TEST(PacketLoopRunnerTest, PacketReceiverRestartControlledMode) {
  std::shared_ptr<MockPacketReceiver> packet_receiver_1 =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());
  std::shared_ptr<MockPacketReceiver> packet_receiver_2 =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());

  std::shared_ptr<MockEventWriter> event_writer =
      std::make_shared<MockEventWriter>();

  std::vector<int64_t> packet_offsets{101, 102, 103, 104, 105};
  int index = 0;
  {
    InSequence s;
    // Receive 3 packets: 101, 102, 103.
    EXPECT_CALL(*packet_receiver_1, Receive(_, _, _))
        .Times(3)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = true;
          Packet packet;
          SetOffset(packet_offsets[index], &packet).IgnoreError();
          index++;
          *p = packet;
          return true;
        });
    // Finish with INTERNAL.
    EXPECT_CALL(*packet_receiver_1, Receive(_, _, _))
        .WillOnce([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = false;
          return true;
        });
    EXPECT_CALL(*packet_receiver_1, CommitsDone()).WillOnce(Return());
    EXPECT_CALL(*packet_receiver_1, Finish())
        .WillOnce(Return(absl::InternalError("internal")));
  }

  {
    InSequence s;
    // 2 succesful commits.
    EXPECT_CALL(*packet_receiver_1, Commit(_, _, _))
        .Times(2)
        .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
          *ok = true;
          return true;
        });
    // Last commit failed.
    EXPECT_CALL(*packet_receiver_1, Commit(_, _, _))
        .Times(1)
        .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
          index--;
          *ok = false;
          return true;
        });
  }

  {
    InSequence s;
    // Receive 3 packets: 103 (duplicated), 104, 105.
    EXPECT_CALL(*packet_receiver_2, Receive(_, _, _))
        .Times(3)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = true;
          Packet packet;
          SetOffset(packet_offsets[index], &packet).IgnoreError();
          index++;
          *p = packet;
          return true;
        });
    // Finish with OUT_OF_RANGE.
    EXPECT_CALL(*packet_receiver_2, Receive(_, _, _))
        .WillOnce([&](absl::Duration timeout, Packet *p, bool *ok) {
          *ok = false;
          return true;
        });
    EXPECT_CALL(*packet_receiver_2, CommitsDone()).WillOnce(Return());
    EXPECT_CALL(*packet_receiver_2, Finish())
        .WillOnce(Return(absl::OutOfRangeError("event ended")));
  }
  // Only the last 2 packets are processed.
  EXPECT_CALL(*packet_receiver_2, Commit(_, _, _))
      .Times(2)
      .WillRepeatedly([&](absl::Duration timeout, int64_t offset, bool *ok) {
        *ok = true;
        return true;
      });

  EXPECT_CALL(*event_writer, Write(_))
      .Times(5)
      .WillRepeatedly([&](Packet packet) {
        event_writer->callback_(GetOffset(packet));
        return absl::OkStatus();
      });
  EXPECT_CALL(*event_writer, Close()).WillOnce(Return(absl::OkStatus()));

  int packet_receiver_count = 0;
  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options)
      -> absl::StatusOr<std::shared_ptr<PacketReceiver>> {
    packet_receiver_count++;
    if (packet_receiver_count == 1) {
      return packet_receiver_1;
    }
    if (packet_receiver_count == 2) {
      return packet_receiver_2;
    }
    return absl::InternalError("no more receivers");
  };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    event_writer->SetCallback(callback);
    return event_writer;
  };

  PacketLoopRunner::Options options;
  options.current_event = CreateEventUpdate(0, "ev-0");
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "controlled"};
  options.event_commit_callback = [](int64_t offset) -> void { return; };
  PacketLoopRunner runner(options);
  ASSERT_TRUE(runner.Run().ok());
}

TEST(PacketLoopRunnerTest, SinglePacketReceiverEagerMode) {
  std::shared_ptr<MockPacketReceiver> packet_receiver =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());
  std::shared_ptr<MockEventWriter> event_writer =
      std::make_shared<MockEventWriter>();

  std::vector<int64_t> packet_offsets{101, 102, 103};
  int index = 0;
  {
    InSequence s;
    // Timeouts.
    EXPECT_CALL(*packet_receiver, Receive(_, _))
        .Times(2)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p) {
          return absl::NotFoundError(absl::StrFormat(
              "%s: no available packets", kPacketReceiverErrMsgPrefix));
        });
    // Receive 3 packets.
    EXPECT_CALL(*packet_receiver, Receive(_, _))
        .Times(3)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p) {
          Packet packet;
          SetOffset(packet_offsets[index], &packet).IgnoreError();
          index++;
          *p = packet;
          return absl::OkStatus();
        });
    // Timeouts.
    EXPECT_CALL(*packet_receiver, Receive(_, _))
        .Times(2)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p) {
          return absl::DeadlineExceededError("deadline exceeded");
        });
    // Finish with OUT_OF_RANGE.
    EXPECT_CALL(*packet_receiver, Receive(_, _))
        .WillOnce([&](absl::Duration timeout, Packet *p) {
          return absl::OutOfRangeError("event ended");
        });
  }

  // Set up event writer to process 3 packets.
  {
    InSequence s;
    // Process 3 packets.
    EXPECT_CALL(*event_writer, Write(_))
        .Times(3)
        .WillRepeatedly([&](Packet packet) { return absl::OkStatus(); });
    EXPECT_CALL(*event_writer, Close()).WillOnce(Return(absl::OkStatus()));
  }

  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options) {
        return packet_receiver;
      };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    return event_writer;
  };

  PacketLoopRunner::Options options;
  options.current_event = CreateEventUpdate(0, "ev-0");
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "eager"};
  int acked_event_count = 0;
  options.event_commit_callback = [&acked_event_count](int64_t offset) {
    acked_event_count++;
  };
  PacketLoopRunner runner(options);
  ASSERT_TRUE(runner.Run().ok());

  EXPECT_EQ(acked_event_count, 1);
}

TEST(PacketLoopRunnerTest, PacketReceiverRestartEagerMode) {
  std::shared_ptr<MockPacketReceiver> packet_receiver_1 =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());
  std::shared_ptr<MockPacketReceiver> packet_receiver_2 =
      std::make_shared<MockPacketReceiver>(PacketReceiver::Options());

  std::shared_ptr<MockEventWriter> event_writer =
      std::make_shared<MockEventWriter>();

  std::vector<int64_t> packet_offsets{101, 102, 103, 104, 105};
  int index = 0;
  {
    InSequence s;
    // Receive 3 packets: 101, 102, 103.
    EXPECT_CALL(*packet_receiver_1, Receive(_, _))
        .Times(3)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p) {
          Packet packet;
          SetOffset(packet_offsets[index], &packet).IgnoreError();
          index++;
          *p = packet;
          return absl::OkStatus();
        });
    // Finish with INTERNAL.
    EXPECT_CALL(*packet_receiver_1, Receive(_, _))
        .WillOnce([&](absl::Duration timeout, Packet *p) {
          return absl::InternalError("internal");
        });
  }
  {
    InSequence s;
    // Receive 4 packets: 104, 105.
    EXPECT_CALL(*packet_receiver_2, Receive(_, _))
        .Times(2)
        .WillRepeatedly([&](absl::Duration timeout, Packet *p) {
          Packet packet;
          SetOffset(packet_offsets[index], &packet).IgnoreError();
          index++;
          *p = packet;
          return absl::OkStatus();
        });
    // Finish with OUT_OF_RANGE.
    EXPECT_CALL(*packet_receiver_2, Receive(_, _))
        .WillOnce([&](absl::Duration timeout, Packet *p) {
          return absl::OutOfRangeError("event ended");
        });
  }

  EXPECT_CALL(*event_writer, Write(_))
      .Times(5)
      .WillRepeatedly([&](Packet packet) { return absl::OkStatus(); });
  EXPECT_CALL(*event_writer, Close()).WillOnce(Return(absl::OkStatus()));

  int packet_receiver_count = 0;
  PacketReceiverFactory packet_receiver_factory =
      [&](const PacketReceiver::Options &options)
      -> absl::StatusOr<std::shared_ptr<PacketReceiver>> {
    packet_receiver_count++;
    if (packet_receiver_count == 1) {
      return packet_receiver_1;
    }
    if (packet_receiver_count == 2) {
      return packet_receiver_2;
    }
    return absl::InternalError("no more receivers");
  };
  EventWriterFactory event_writer_factory = [&](const std::string &event_id,
                                                OffsetCommitCallback callback) {
    return event_writer;
  };

  PacketLoopRunner::Options options;
  options.current_event = CreateEventUpdate(0, "ev-0");
  options.event_writer_factory = event_writer_factory;
  options.packet_receiver_factory = packet_receiver_factory;
  options.packet_receiver_options =
      PacketReceiver::Options{.receive_mode = "eager"};
  options.event_commit_callback = nullptr;
  PacketLoopRunner runner(options);
  ASSERT_TRUE(runner.Run().ok());
}

}  // namespace visionai