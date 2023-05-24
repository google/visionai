// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/packet/packet.h"

#include <memory>
#include <string>
#include <type_traits>

#include "glog/logging.h"
#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/protobuf/util/message_differencer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/types/raw_image.h"
#include "visionai/types/signal.h"

namespace visionai {

namespace {

using google::cloud::visionai::v1::GstreamerBufferDescriptor;
using google::cloud::visionai::v1::Packet;
using google::protobuf::util::MessageDifferencer;

const absl::string_view kProtobufTypeClass = "protobuf";
const absl::string_view kGstreamerBufferTypeClass = "gst";
const absl::string_view kRawImageTypeClass = "raw-image";
const absl::string_view kSignalTypeClass = "signal";
const absl::string_view kStringTypeClass = "string";

}  // namespace

TEST(PacketTest, MakePacketStringTest) {
  {
    std::string s("hello!");
    auto packet = MakePacket(s);
    EXPECT_TRUE(packet.ok());
    EXPECT_EQ(packet->header().type().type_class(), kStringTypeClass);
    EXPECT_EQ(packet->payload(), s);
  }
  {
    std::string s("hello!");
    std::string tmp(s);
    auto packet = MakePacket(std::move(tmp));
    EXPECT_TRUE(packet.ok());
    EXPECT_EQ(packet->header().type().type_class(), kStringTypeClass);
    EXPECT_EQ(packet->payload(), s);
  }
}

TEST(PacketTest, PackPacket) {
  Packet p;

  // Create a basic string Packet
  std::string s("hello!");
  auto original_packet = MakePacket(s);
  absl::Time t = absl::FromUnixSeconds(123) + absl::Nanoseconds(456);
  EXPECT_TRUE(SetCaptureTime(t, &original_packet.value()).ok());

  std::string raw_iamge_caps(
      "video/x-raw, "
      "format=(string)RGB, "
      "width=(int)1280, height=(int)720, "
      "framerate=(fraction)0/1");
  std::string raw_iamge_payload_bytes(10, 2);
  GstreamerBuffer raw_image_gstreamer_buffer;
  raw_image_gstreamer_buffer.set_caps_string(raw_iamge_caps);
  raw_image_gstreamer_buffer.assign(raw_iamge_payload_bytes);
  raw_image_gstreamer_buffer.set_is_key_frame(true);
  auto raw_image_packet = MakePacket(raw_image_gstreamer_buffer);

  Packet packed_packet(original_packet.value());
  ASSERT_TRUE(Pack(raw_image_gstreamer_buffer, &packed_packet).ok());

  // The header type and the payload will be changed to the
  // corresponding value in the gstream_buffer.
  // All the other fields will remain unchanged.
  EXPECT_TRUE(MessageDifferencer::Equals(
      packed_packet.header().capture_time(),
      original_packet.value().header().capture_time()));
  EXPECT_EQ(t, GetCaptureTime(packed_packet));

  EXPECT_TRUE(
      MessageDifferencer::Equals(packed_packet.header().type(),
                                 raw_image_packet.value().header().type()));
  EXPECT_TRUE(MessageDifferencer::Equals(
      packed_packet.header().metadata(),
      original_packet.value().header().metadata()));
  EXPECT_TRUE(MessageDifferencer::Equals(
      packed_packet.header().server_metadata(),
      original_packet.value().header().server_metadata()));
  EXPECT_TRUE(MessageDifferencer::Equals(
      packed_packet.header().series_metadata(),
      original_packet.value().header().series_metadata()));
  EXPECT_TRUE(MessageDifferencer::Equals(
      packed_packet.header().server_metadata(),
      original_packet.value().header().server_metadata()));
  EXPECT_THAT(packed_packet.header().flags(),
              original_packet.value().header().flags());
  EXPECT_THAT(packed_packet.header().trace_context(),
              original_packet.value().header().trace_context());

  EXPECT_THAT(packed_packet.payload(),
              raw_image_packet.value().payload());
  EXPECT_EQ(packed_packet.payload(), raw_iamge_payload_bytes);
}

TEST(PacketTest, PacketAsStringTest) {
  {
    std::string src("hey!");
    auto packet = MakePacket(src);
    EXPECT_TRUE(packet.ok());

    auto dst = PacketAs<std::string>(*packet);
    EXPECT_TRUE(dst.ok());
    EXPECT_EQ(*dst, src);
  }
  {
    std::string src("hey!");
    auto packet = MakePacket(src);
    EXPECT_TRUE(packet.ok());

    auto dst = PacketAs<std::string>(*std::move(packet));
    EXPECT_TRUE(dst.ok());
    EXPECT_EQ(*dst, src);
  }
}

TEST(PacketTest, ProtobufPacketTest) {
  {
    // Create a protobuf Packet containing a Packet protobuf.
    std::string s("hello!");
    auto string_packet = MakePacket(s);
    EXPECT_TRUE(string_packet.ok());

    auto protobuf_packet = MakePacket(*string_packet);
    EXPECT_TRUE(protobuf_packet.ok());
    EXPECT_TRUE(IsProtobufPacket(*protobuf_packet));

    // Test that it is not possible to read it out as another protobuf message.
    auto bad_packet = PacketAs<GstreamerBufferDescriptor>(*protobuf_packet);
    EXPECT_FALSE(bad_packet.ok());

    // Read the true packet back.
    auto good_packet = PacketAs<Packet>(*protobuf_packet);
    EXPECT_TRUE(good_packet.ok());
    auto good_good_packet = PacketAs<std::string>(*good_packet);
    EXPECT_TRUE(good_good_packet.ok());
    EXPECT_EQ(*good_good_packet, s);
  }
}

TEST(PacketTest, GstreamerBufferPacketTest) {
  {
    // Create some make-believe gstreamer buffer.
    std::string caps(
        "video/x-h264, "
        "codec_data=(buffer)"
        "01f4000dffe1001d67f4000d919b28283f602d41804150000003001000000303c8f142"
        "996001000568ebec4480, stream-format=(string)avc, "
        "alignment=(string)au, level=(string)1.3, profile=(string)high-4:4:4, "
        "width=(int)320, height=(int)240, pixel-aspect-ratio=(fraction)1/1, "
        "framerate=(fraction)30/1, interlace-mode=(string)progressive, "
        "colorimetry=(string)bt601, chroma-site=(string)jpeg, "
        "multiview-mode=(string)mono, "
        "multiview-flags=(GstVideoMultiviewFlagsSet)0:ffffffff:/"
        "right-view-first/left-flipped/left-flopped/right-flipped/"
        "right-flopped/half-aspect/mixed-mono, chroma-format=(string)4:4:4, "
        "bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, "
        "parsed=(boolean)true");
    std::string bytes(10, 2);
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.set_caps_string(caps);
    gstreamer_buffer.assign(bytes);
    gstreamer_buffer.set_is_key_frame(true);

    auto gst_packet = MakePacket(gstreamer_buffer);
    EXPECT_TRUE(gst_packet.ok());
    EXPECT_EQ(gst_packet->header().type().type_class(),
              kGstreamerBufferTypeClass);
    for (size_t i = 0; i < bytes.size(); ++i) {
      EXPECT_EQ(gst_packet->payload()[i], bytes[i]);
    }
    EXPECT_EQ(gst_packet->payload().size(), bytes.size());

    auto gst = PacketAs<GstreamerBuffer>(*gst_packet);
    EXPECT_TRUE(gst.ok());
    EXPECT_EQ(gst->caps_string(), caps);
    EXPECT_EQ(gst->is_key_frame(), true);
    EXPECT_EQ(gst->media_type(), "video/x-h264");
    EXPECT_EQ(bytes, std::string(gst->data(), gst->size()));
  }
}

TEST(PacketTest, RawImagePacketTest) {
  {
    std::string src(45, 'a');
    RawImage rsrc(3, 5, RawImage::Format::kSRGB, std::string(src));

    auto packet = MakePacket(rsrc);
    EXPECT_TRUE(packet.ok());

    EXPECT_EQ(packet->header().type().type_class(), kRawImageTypeClass);
    auto type_name = GetTypeName(*packet);
    EXPECT_EQ(type_name, "raw-image/srgb");

    EXPECT_TRUE(
        packet->header().type().type_descriptor().has_raw_image_descriptor());
    int height = packet->header()
                     .type()
                     .type_descriptor()
                     .raw_image_descriptor()
                     .height();
    int width = packet->header()
                    .type()
                    .type_descriptor()
                    .raw_image_descriptor()
                    .width();
    std::string format = packet->header()
                             .type()
                             .type_descriptor()
                             .raw_image_descriptor()
                             .format();
    EXPECT_EQ(height, 3);
    EXPECT_EQ(width, 5);
    EXPECT_EQ(format, "srgb");

    EXPECT_EQ(packet->payload().size(), src.size());
    for (size_t i = 0; i < src.size(); ++i) {
      EXPECT_EQ(packet->payload()[i], src[i]);
    }

    auto rdst = PacketAs<RawImage>(*packet);
    EXPECT_TRUE(rdst.ok());
    EXPECT_EQ(rdst->height(), 3);
    EXPECT_EQ(rdst->width(), 5);
    EXPECT_EQ(rdst->channels(), 3);
    EXPECT_EQ(rdst->format(), RawImage::Format::kSRGB);
    EXPECT_EQ(rdst->size(), src.size());
    for (size_t i = 0; i < src.size(); ++i) {
      EXPECT_EQ((*rdst)(i), src[i]);
    }
  }
}

TEST(PacketTest, CaptureTimeTest) {
  absl::Time t1 = absl::FromUnixSeconds(123) + absl::Nanoseconds(456);
  Packet p;
  auto status = SetCaptureTime(t1, &p);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(123, p.header().capture_time().seconds());
  EXPECT_EQ(456, p.header().capture_time().nanos());

  auto t2 = GetCaptureTime(p);
  EXPECT_EQ(t1, t2);
}

TEST(PacketTest, IngestTimeTest) {
  absl::Time t1 = absl::FromUnixSeconds(123) + absl::Nanoseconds(456);
  Packet p;
  auto status = SetIngestTime(t1, &p);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(123, p.header().server_metadata().ingest_time().seconds());
  EXPECT_EQ(456, p.header().server_metadata().ingest_time().nanos());

  auto t2 = GetIngestTime(p);
  EXPECT_EQ(t1, t2);
}

TEST(PacketTest, OffsetTest) {
  Packet p;
  int64_t value = 42;
  auto status = SetOffset(42, &p);
  EXPECT_TRUE(status.ok());

  auto result = GetOffset(p);
  EXPECT_EQ(value, result);
}

TEST(PacketTest, GetTypeClassTest) {
  {
    std::string s("hello!");
    auto packet = MakePacket(s);
    EXPECT_TRUE(packet.ok());
    auto type_class = GetTypeClass(*packet);
    EXPECT_EQ(type_class, std::string(kStringTypeClass));
  }
  {
    // Create a protobuf Packet containing a Packet protobuf.
    std::string s("hello!");
    auto string_packet = MakePacket(s);
    EXPECT_TRUE(string_packet.ok());

    auto protobuf_packet = MakePacket(*string_packet);
    EXPECT_TRUE(protobuf_packet.ok());

    auto type_class = GetTypeClass(*protobuf_packet);
    EXPECT_EQ(type_class, kProtobufTypeClass);
  }
}

TEST(PacketTest, GetTypeTest) {
  {
    std::string s("hello!");
    auto packet = MakePacket(s);
    EXPECT_TRUE(packet.ok());
    auto type = GetType(*packet);
    std::string expected = std::string();
    EXPECT_EQ(type, expected);
  }
  {
    // Create a protobuf Packet containing a Packet protobuf.
    std::string s("hello!");
    auto string_packet = MakePacket(s);
    EXPECT_TRUE(string_packet.ok());

    auto protobuf_packet = MakePacket(*string_packet);
    EXPECT_TRUE(protobuf_packet.ok());

    auto type = GetType(*protobuf_packet);
    std::string expected = Packet().GetTypeName();
    EXPECT_EQ(type, expected);
  }
  {
    // Create some make-believe gstreamer buffer.
    std::string caps(
        "video/x-h264, "
        "codec_data=(buffer)"
        "01f4000dffe1001d67f4000d919b28283f602d41804150000003001000000303c8f142"
        "996001000568ebec4480, stream-format=(string)avc, "
        "alignment=(string)au, level=(string)1.3, profile=(string)high-4:4:4, "
        "width=(int)320, height=(int)240, pixel-aspect-ratio=(fraction)1/1, "
        "framerate=(fraction)30/1, interlace-mode=(string)progressive, "
        "colorimetry=(string)bt601, chroma-site=(string)jpeg, "
        "multiview-mode=(string)mono, "
        "multiview-flags=(GstVideoMultiviewFlagsSet)0:ffffffff:/"
        "right-view-first/left-flipped/left-flopped/right-flipped/"
        "right-flopped/half-aspect/mixed-mono, chroma-format=(string)4:4:4, "
        "bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, "
        "parsed=(boolean)true");
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.set_caps_string(caps);

    auto gst_packet = MakePacket(gstreamer_buffer);
    EXPECT_TRUE(gst_packet.ok());

    auto type = GetType(*gst_packet);
    std::string expected = "video/x-h264";
    EXPECT_EQ(type, expected);
  }
}

TEST(PacketTest, GetTypeNameTest) {
  {
    std::string s("hello!");
    auto packet = MakePacket(s);
    EXPECT_TRUE(packet.ok());
    auto type_name = GetTypeName(*packet);
    EXPECT_EQ(type_name, std::string(kStringTypeClass));
  }
  {
    // Create a protobuf Packet containing a Packet protobuf.
    std::string s("hello!");
    auto string_packet = MakePacket(s);
    EXPECT_TRUE(string_packet.ok());

    auto protobuf_packet = MakePacket(*string_packet);
    EXPECT_TRUE(protobuf_packet.ok());

    auto type_name = GetTypeName(*protobuf_packet);
    std::string expected =
        absl::StrFormat("%s/%s", kProtobufTypeClass, Packet().GetTypeName());
    EXPECT_EQ(type_name, expected);
  }
  {
    // Create some make-believe gstreamer buffer.
    std::string caps(
        "video/x-h264, "
        "codec_data=(buffer)"
        "01f4000dffe1001d67f4000d919b28283f602d41804150000003001000000303c8f142"
        "996001000568ebec4480, stream-format=(string)avc, "
        "alignment=(string)au, level=(string)1.3, profile=(string)high-4:4:4, "
        "width=(int)320, height=(int)240, pixel-aspect-ratio=(fraction)1/1, "
        "framerate=(fraction)30/1, interlace-mode=(string)progressive, "
        "colorimetry=(string)bt601, chroma-site=(string)jpeg, "
        "multiview-mode=(string)mono, "
        "multiview-flags=(GstVideoMultiviewFlagsSet)0:ffffffff:/"
        "right-view-first/left-flipped/left-flopped/right-flipped/"
        "right-flopped/half-aspect/mixed-mono, chroma-format=(string)4:4:4, "
        "bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, "
        "parsed=(boolean)true");
    GstreamerBuffer gstreamer_buffer;
    gstreamer_buffer.set_caps_string(caps);

    auto gst_packet = MakePacket(gstreamer_buffer);
    EXPECT_TRUE(gst_packet.ok());

    auto type_name = GetTypeName(*gst_packet);
    std::string expected =
        absl::StrFormat("%s/%s", kGstreamerBufferTypeClass, "video/x-h264");
    EXPECT_EQ(type_name, expected);
  }
}

TEST(PacketTest, SeriesNameTest) {
  Packet p;

  // By default, series name is empty.
  EXPECT_TRUE(GetSeriesName(p).empty());
  const std::string series_name = "test-series";
  auto status = SetSeriesName(series_name, &p);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(series_name, p.header().series_metadata().series());

  EXPECT_EQ(GetSeriesName(p), series_name);

  EXPECT_TRUE(absl::IsInvalidArgument(SetSeriesName(series_name, nullptr)));
}

TEST(PacketTest, MetadataTest) {
  Packet p;

  std::string key = "field_name";
  auto value = google::protobuf::Value();
  value.set_string_value("field_value");

  // Getting an unset field should return an error.
  auto get_status = GetMetadataField(key, p);
  ASSERT_TRUE(!get_status.ok());
  EXPECT_TRUE(absl::IsNotFound(get_status.status()));

  auto set_status = SetMetadataField(key, value, &p);
  EXPECT_TRUE(set_status.ok());
  EXPECT_TRUE(MessageDifferencer::Equals(
      value, p.header().metadata().fields().at(key)));

  auto get_val = GetMetadataField(key, p);
  ASSERT_TRUE(get_val.ok());
  EXPECT_TRUE(MessageDifferencer::Equals(value, *get_val));

  EXPECT_TRUE(absl::IsInvalidArgument(SetMetadataField(key, value, nullptr)));

  auto remove_status = RemoveMetadataField(key, &p);
  ASSERT_TRUE(remove_status.ok());
  get_status = GetMetadataField(key, p);
  ASSERT_FALSE(get_status.ok());
  EXPECT_TRUE(absl::IsInvalidArgument(RemoveMetadataField(key, nullptr)));
}

TEST(PacketTest, SignalPacketTest) {
  {
    Signal signal;
    auto packet = MakePacket(signal);
    EXPECT_TRUE(packet.ok());

    EXPECT_EQ(packet->header().type().type_class(), kSignalTypeClass);
    auto type_name = GetTypeName(*packet);
    EXPECT_EQ(type_name, "signal/unknown");

    auto sdst = PacketAs<Signal>(*packet);
    EXPECT_TRUE(sdst.ok());
    EXPECT_EQ(sdst->code(), Signal::SignalCode::kUnknown);
  }
}

TEST(PacketTest, SignalPacketMethodTest) {
  {
    Signal signal;
    auto packet = MakePacket(signal);
    EXPECT_TRUE(packet.ok());
    EXPECT_TRUE(IsSignalPacket(*packet));
    EXPECT_FALSE(IsPhantomPacket(*packet));
  }
  {
    Signal signal(Signal::SignalCode::kPhantom);
    auto packet = MakePacket(signal);
    EXPECT_TRUE(packet.ok());
    EXPECT_TRUE(IsSignalPacket(*packet));
    EXPECT_TRUE(IsPhantomPacket(*packet));
  }
  {
    std::string s("hello!");
    auto packet = MakePacket(s);
    EXPECT_TRUE(packet.ok());
    EXPECT_FALSE(IsSignalPacket(*packet));
    EXPECT_FALSE(IsPhantomPacket(*packet));
  }
  {
    auto packet = MakePhantomPacket();
    EXPECT_TRUE(packet.ok());
    EXPECT_TRUE(IsSignalPacket(*packet));
    EXPECT_TRUE(IsPhantomPacket(*packet));
  }
  {
    auto packet = MakeEOSPacket();
    EXPECT_TRUE(packet.ok());
    EXPECT_TRUE(IsSignalPacket(*packet));
    EXPECT_TRUE(IsEOSPacket(*packet));
    EXPECT_FALSE(IsPhantomPacket(*packet));
  }
}

}  // namespace visionai
