// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/proto_util.h"

#include "absl/strings/escaping.h"
#include "absl/strings/str_split.h"
#include "visionai/util/line_crossing_util.h"

#define RET_CHECK(expr) \
  do { \
    if (! (expr) ) { \
      return absl::InternalError("internal error occured"); \
    } \
  } while (false)
namespace visionai {
namespace {

using ::google::cloud::visionai::v1::NormalizedPolygon;
using ::google::cloud::visionai::v1::NormalizedPolyline;
using ::google::cloud::visionai::v1::NormalizedVertex;
using ::google::cloud::visionai::v1::StreamAnnotation;
using ::google::cloud::visionai::v1::StreamAnnotations;

}  // namespace

absl::StatusOr<std::string> SetAnnotations(std::string serialized_annotation,
                                           bool is_web_base64_string) {
  StreamAnnotations stream_annotations;
  if (is_web_base64_string) {
    std::string basic_serialized_annotation;
    absl::WebSafeBase64Unescape(serialized_annotation,
                                &basic_serialized_annotation);
    stream_annotations.ParseFromString(
        std::string(basic_serialized_annotation));
  } else {
    stream_annotations.ParseFromString(std::string(serialized_annotation));
  }
  std::string annotation_segment_end_points;
  for (const StreamAnnotation& stream_annotation :
       stream_annotations.stream_annotations()) {
    switch (stream_annotation.type()) {
      case StreamAnnotationType::STREAM_ANNOTATION_TYPE_ACTIVE_ZONE: {
        std::string polygon_string;
        for (const NormalizedVertex& vertex :
             stream_annotation.active_zone().normalized_vertices()) {
          if (polygon_string.empty()) {
            absl::StrAppend(&polygon_string, vertex.x(), ":", vertex.y());
          } else {
            absl::StrAppend(&polygon_string, ";", vertex.x(), ":", vertex.y());
          }
        }
        if (annotation_segment_end_points.empty()) {
          annotation_segment_end_points = polygon_string;
        } else {
          absl::StrAppend(&annotation_segment_end_points, "-", polygon_string);
        }
      } break;
      case StreamAnnotationType::STREAM_ANNOTATION_TYPE_CROSSING_LINE: {
        std::string polyline_string;
        for (const NormalizedVertex& vertex :
             stream_annotation.crossing_line().normalized_vertices()) {
          if (polyline_string.empty()) {
            absl::StrAppend(&polyline_string, vertex.x(), ":", vertex.y());
          } else {
            absl::StrAppend(&polyline_string, ";", vertex.x(), ":", vertex.y());
          }
        }
        if (annotation_segment_end_points.empty()) {
          annotation_segment_end_points = polyline_string;
        } else {
          absl::StrAppend(&annotation_segment_end_points, "-", polyline_string);
        }
      } break;
      default:
        return absl::InvalidArgumentError(
            "Please specify either line or zone annotations.");
        break;
    }
  }
  return annotation_segment_end_points;
}

absl::StatusOr<std::string> BuildSerializedStreamAnnotations(
    std::string polylines_string, StreamAnnotationType annotation_type,
    bool use_web_base64_string) {
  auto video_annotations = std::make_unique<StreamAnnotations>();
  std::vector<std::vector<std::string>> end_points_group =
      util::ParsePolyLineSegments(polylines_string);
  for (int j = 0; j < end_points_group.size(); ++j) {
    StreamAnnotation* single_annotation =
        video_annotations->add_stream_annotations();
    *single_annotation->mutable_id() = std::to_string(j);
    *single_annotation->mutable_display_name() = std::to_string(j);
    single_annotation->set_type(annotation_type);
    switch (single_annotation->type()) {
      case StreamAnnotationType::STREAM_ANNOTATION_TYPE_ACTIVE_ZONE: {
        NormalizedPolygon normalized_polygon;
        for (int i = 0; i < end_points_group[j].size(); ++i) {
          float x, y;
          std::vector<std::string> end_points =
              absl::StrSplit(end_points_group[j][i], ':');
          RET_CHECK(absl::SimpleAtof(end_points[0], &x));
          RET_CHECK(absl::SimpleAtof(end_points[1], &y));
          NormalizedVertex* normalized_vertex =
              normalized_polygon.add_normalized_vertices();
          normalized_vertex->set_x(x);
          normalized_vertex->set_y(y);
        }
        *single_annotation->mutable_active_zone() = normalized_polygon;
      } break;
      case StreamAnnotationType::STREAM_ANNOTATION_TYPE_CROSSING_LINE: {
        NormalizedPolyline normalized_polyline;
        for (int i = 0; i < end_points_group[j].size(); ++i) {
          float x, y;
          std::vector<std::string> end_points =
              absl::StrSplit(end_points_group[j][i], ':');
          RET_CHECK(absl::SimpleAtof(end_points[0], &x));
          RET_CHECK(absl::SimpleAtof(end_points[1], &y));
          NormalizedVertex* normalized_vertex =
              normalized_polyline.add_normalized_vertices();
          normalized_vertex->set_x(x);
          normalized_vertex->set_y(y);
        }
        *single_annotation->mutable_crossing_line() = normalized_polyline;
      } break;
      default:
        break;
    }
  }
  std::string annotation_proto_string;
  video_annotations->SerializeToString(&annotation_proto_string);
  if (use_web_base64_string) {
    std::string annotation_proto_string_web_base64;
    absl::WebSafeBase64Escape(annotation_proto_string,
                              &annotation_proto_string_web_base64);
    return annotation_proto_string_web_base64;
  } else {
    return annotation_proto_string;
  }
}

void ForceLinkAnnotationProtos() {
  google::cloud::visionai::v1::PersonalProtectiveEquipmentDetectionOutput
      personal_protective_equipment_detection_instance;
  google::cloud::visionai::v1::ObjectDetectionPredictionResult
      object_detection_instance;
  google::cloud::visionai::v1::ImageObjectDetectionPredictionResult
      image_object_detection_instance;
  google::cloud::visionai::v1::ClassificationPredictionResult
      image_classification_instance;
  google::cloud::visionai::v1::ImageSegmentationPredictionResult
      image_segmentation_instance;
  google::cloud::visionai::v1::VideoActionRecognitionPredictionResult
      video_action_recognition_instance;
  google::cloud::visionai::v1::VideoObjectTrackingPredictionResult
      video_object_tracking_instance;
  google::cloud::visionai::v1::VideoClassificationPredictionResult
      video_classification_instance;
  google::cloud::visionai::v1::OccupancyCountingPredictionResult
      occupancy_count_instance;
}

}  // namespace visionai
