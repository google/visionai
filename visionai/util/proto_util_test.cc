// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/proto_util.h"

#include "google/protobuf/timestamp.pb.h"
#include "gtest/gtest.h"

namespace visionai {
namespace {

constexpr char kExpectedLineAnnotationString[] = "0.1:0.1;0.5:0.1;0.9:0.1";
constexpr char kExpectedZoneAnnotationString[] = "0.1:0.1;0.5:0.9;0.9:0.1";
constexpr char kExpectedMultipleLinesAnnotationString[] =
    "0.1:0.1;0.5:0.1;0.9:0.1-0.1:0.1;0.5:0.1;0.9:0.1-0.1:0.1;0.5:0.1;0.9:0.1";
constexpr char kExpectedMultipleZonesAnnotationString[] =
    "0.1:0.1;0.5:0.9;0.9:0.1-0.1:0.1;0.5:0.1;0.9:0.1-0.1:0.1;0.5:0.1;0.9:0.1";
constexpr char test_proto[] =
    "visionai/util/testdata/timestamp.pbtxt";

TEST(ProtoUtil, TestReadAndParseProtoFromFile) {
  absl::StatusOr<google::protobuf::Timestamp> status_or_timestamp =
      ReadAndParseProtoFromFile<google::protobuf::Timestamp>(test_proto);
  ASSERT_TRUE(status_or_timestamp.ok());
  ASSERT_EQ((*status_or_timestamp).seconds(), 10);
  ASSERT_EQ((*status_or_timestamp).nanos(), 20);
}

TEST(ProtoUtil, TestReadAndParseProtoFromFileFailed) {
  absl::StatusOr<google::protobuf::Timestamp> status_or_timestamp =
      ReadAndParseProtoFromFile<google::protobuf::Timestamp>("");
  ASSERT_FALSE(status_or_timestamp.ok());
}

TEST(ProtoUtil, TestBuildSerializedStreamAnnotationsLines) {
  std::string serialized_proto_string =
      BuildSerializedStreamAnnotations(
          kExpectedLineAnnotationString,
          StreamAnnotationType::STREAM_ANNOTATION_TYPE_CROSSING_LINE)
          .value();
  std::string deserialized_proto_string =
      SetAnnotations(serialized_proto_string).value();
  EXPECT_EQ(deserialized_proto_string, kExpectedLineAnnotationString);
}

TEST(ProtoUtil, TestBuildSerializedStreamAnnotationsLinesWithoutWebEncoding) {
  std::string serialized_proto_string =
      BuildSerializedStreamAnnotations(
          kExpectedLineAnnotationString,
          StreamAnnotationType::STREAM_ANNOTATION_TYPE_CROSSING_LINE,
          /*use_web_base64_string=*/false)
          .value();
  std::string deserialized_proto_string =
      SetAnnotations(serialized_proto_string,
                     /*is_web_base64_string=*/false)
          .value();
  EXPECT_EQ(deserialized_proto_string, kExpectedLineAnnotationString);
}

TEST(ProtoUtil, TestBuildSerializedStreamAnnotationsZones) {
  std::string serialized_proto_string =
      BuildSerializedStreamAnnotations(
          kExpectedZoneAnnotationString,
          StreamAnnotationType::STREAM_ANNOTATION_TYPE_ACTIVE_ZONE)
          .value();
  std::string deserialized_proto_string =
      SetAnnotations(serialized_proto_string).value();
  EXPECT_EQ(deserialized_proto_string, kExpectedZoneAnnotationString);
}

TEST(ProtoUtil, TestBuildSerializedStreamAnnotationsMultiLines) {
  std::string serialized_proto_string =
      BuildSerializedStreamAnnotations(
          kExpectedMultipleLinesAnnotationString,
          StreamAnnotationType::STREAM_ANNOTATION_TYPE_CROSSING_LINE)
          .value();
  std::string deserialized_proto_string =
      SetAnnotations(serialized_proto_string).value();
  EXPECT_EQ(deserialized_proto_string, kExpectedMultipleLinesAnnotationString);
}

TEST(ProtoUtil, TestBuildSerializedStreamAnnotationsMultiZones) {
  std::string serialized_proto_string =
      BuildSerializedStreamAnnotations(
          kExpectedMultipleZonesAnnotationString,
          StreamAnnotationType::STREAM_ANNOTATION_TYPE_ACTIVE_ZONE)
          .value();
  std::string deserialized_proto_string =
      SetAnnotations(serialized_proto_string).value();
  EXPECT_EQ(deserialized_proto_string, kExpectedMultipleZonesAnnotationString);
}

}  // namespace
}  // namespace visionai
