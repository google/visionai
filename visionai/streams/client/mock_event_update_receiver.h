/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_EVENT_UPDATE_RECEIVER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_EVENT_UPDATE_RECEIVER_H_

#include "gmock/gmock.h"
#include "visionai/streams/client/event_update_receiver.h"

namespace visionai {
class MockEventUpdateReceiver : public EventUpdateReceiver {
 public:
  explicit MockEventUpdateReceiver(const EventUpdateReceiver::Options &options)
      : EventUpdateReceiver(options) {}
  ~MockEventUpdateReceiver() override = default;

  MOCK_METHOD(bool, Receive,
              (absl::Duration timeout, EventUpdate *event_update, bool *ok),
              (override));
  MOCK_METHOD(bool, Commit, (absl::Duration timeout, int64_t offset, bool *ok),
              (override));
  MOCK_METHOD(void, CommitsDone, (), (override));
  MOCK_METHOD(absl::Status, Finish, (), (override));
  MOCK_METHOD(void, Cancel, (), (override));
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_EVENT_UPDATE_RECEIVER_H_
