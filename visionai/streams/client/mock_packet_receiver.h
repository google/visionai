/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_PACKET_RECEIVER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_PACKET_RECEIVER_H_

#include "gmock/gmock.h"
#include "visionai/streams/client/packet_receiver.h"

namespace visionai {

class MockPacketReceiver : public PacketReceiver {
 public:
  explicit MockPacketReceiver(const PacketReceiver::Options &options)
      : PacketReceiver(options) {}
  ~MockPacketReceiver() override = default;

  MOCK_METHOD(bool, Receive, (absl::Duration timeout, Packet *packet, bool *ok),
              (override));
  MOCK_METHOD(absl::Status, Receive, (absl::Duration timeout, Packet *packet),
              (override));
  MOCK_METHOD(bool, Commit, (absl::Duration timeout, int64_t offset, bool *ok),
              (override));
  MOCK_METHOD(void, CommitsDone, (), (override));
  MOCK_METHOD(absl::Status, Finish, (), (override));
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_PACKET_RECEIVER_H_
