/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_TESTING_VAICTL_MOCK_PACKAGE_SENDER_H_
#define THIRD_PARTY_VISIONAI_TESTING_VAICTL_MOCK_PACKAGE_SENDER_H_

#include "gmock/gmock.h"
#include "visionai/streams/client/packet_sender.h"

namespace visionai {

class MockPacketSender : public PacketSender {
 public:
  explicit MockPacketSender(const PacketSender::Options& options)
      : PacketSender(options) {}

  ~MockPacketSender() override = default;

  MOCK_METHOD(absl::Status, Send, (const Packet p), (override));
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TESTING_VAICTL_MOCK_PACKAGE_SENDER_H_
