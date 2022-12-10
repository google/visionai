// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/filtered_element.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {

namespace {
constexpr char kTestEventId[] = "event-1";
constexpr char kTestPacketString[] = "test-string";
}  // namespace

TEST(FilteredElement, PacketTypeTest) {
  auto p = MakePacket(std::string(kTestPacketString));
  EXPECT_TRUE(p.ok());
  auto f = MakePacketFilteredElement(kTestEventId, *std::move(p));
  EXPECT_TRUE(f.ok());
  EXPECT_EQ(f->type(), FilteredElementType::kPacket);
  EXPECT_EQ(f->event_id(), kTestEventId);
  EXPECT_TRUE(IsPacketType(*f));
  EXPECT_FALSE(IsOpenType(*f));

  auto packet = PacketFromFilteredElement(*std::move(f));
  EXPECT_TRUE(packet.ok());
  auto s = PacketAs<std::string>(*std::move(packet));
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(*s, kTestPacketString);
}

TEST(FilteredElement, ControlTypeTest) {
  {
    auto f = MakeOpenFilteredElement(kTestEventId);
    EXPECT_TRUE(f.ok());
    EXPECT_EQ(f->type(), FilteredElementType::kOpen);
    EXPECT_EQ(f->event_id(), kTestEventId);
    EXPECT_FALSE(IsPacketType(*f));
    EXPECT_TRUE(IsOpenType(*f));
    EXPECT_FALSE(IsCloseType(*f));
  }
  {
    auto f = MakeCloseFilteredElement(kTestEventId);
    EXPECT_TRUE(f.ok());
    EXPECT_EQ(f->type(), FilteredElementType::kClose);
    EXPECT_EQ(f->event_id(), kTestEventId);
    EXPECT_FALSE(IsPacketType(*f));
    EXPECT_FALSE(IsOpenType(*f));
    EXPECT_TRUE(IsCloseType(*f));
  }
}

}  // namespace visionai
