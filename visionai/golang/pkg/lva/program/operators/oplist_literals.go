// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

const (
	// MainEmbeddedOperatorListLiteral is the hard embed oplist.
	// In go 1.16, we would've used the embed package.
	// However, we're stuck at 1.14 until
	// https://github.com/bazelbuild/rules_go/issues/1986
	// is resolved.
	//
	// This will be replaced by an actual remote registry in the future anyway.
	// Just do this hard embed until that lands.
	MainEmbeddedOperatorListLiteral = `
operators: <
  operator: "AutomlVideoActionRecognition"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult"
  >
  attributes: <
    attribute: "model_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "model_uri"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "graph_path"
    type: "string"
    default_value: <
      s: "/google/inference_graph_action_recognition_streaming_edge.pbtxt"
    >
  >
  attributes: <
    attribute: "confidence_threshold"
    type: "float"
    default_value: <
      f: 0
    >
  >
  attributes: <
    attribute: "max_predictions"
    type: "int"
    default_value: <
      i: 0
    >
  >
  resources: <
    cpu: "7"
    memory: "8Gi"
  >
>
operators: <
  operator: "AutomlVideoClassification"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult"
  >
  attributes: <
    attribute: "model_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "model_uri"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "graph_path"
    type: "string"
    default_value: <
      s: "/google/inference_graph_classification_streaming_edge.pbtxt"
    >
  >
  attributes: <
    attribute: "confidence_threshold"
    type: "float"
    default_value: <
      f: 0
    >
  >
  attributes: <
    attribute: "max_predictions"
    type: "int"
    default_value: <
      i: 0
    >
  >
  resources: <
    cpu: "4"
    memory: "4Gi"
    latency_budget_ms: 10000
  >
>
operators: <
  operator: "AutomlVideoObjectTracking"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult"
  >
  attributes: <
    attribute: "model_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "model_uri"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "graph_path"
    type: "string"
    default_value: <
      s: "/google/inference_graph_object_tracking_streaming_edge.pbtxt"
    >
  >
  attributes: <
    attribute: "confidence_threshold"
    type: "float"
    default_value: <
      f: 0
    >
  >
  attributes: <
    attribute: "max_predictions"
    type: "int"
    default_value: <
      i: 0
    >
  >
  attributes: <
    attribute: "min_bounding_box_size"
    type: "float"
    default_value: <
      f: 0
    >
  >
  resources: <
    cpu: "6"
    memory: "6Gi"
  >
>
operators: <
  operator: "AutomlVisionClassification"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult"
  >
  attributes: <
    attribute: "model_gcs_path"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "confidence_threshold"
    type: "float"
    default_value: <
      f: 0
    >
  >
  attributes: <
    attribute: "max_predictions"
    type: "int"
    default_value: <
      i: 0
    >
  >
  attributes: <
    attribute: "model_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "6"
    memory: "6Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "AutomlVisionObjectDetection"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult"
  >
  attributes: <
    attribute: "model_gcs_path"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "confidence_threshold"
    type: "float"
    default_value: <
      f: 0
    >
  >
  attributes: <
    attribute: "max_predictions"
    type: "int"
    default_value: <
      i: 0
    >
  >
  attributes: <
    attribute: "model_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "6"
    memory: "6Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "VertexAiCustom"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.aiplatform.v1.PredictResponse"
  >
  attributes <
    attribute: "vertex_endpoint_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "input_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "vertex_online_prediction_service_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "max_prediction_fps"
    type: "int"
    default_value: <
      i: 0
    >
  >
  resources <
    cpu: "6"
    gpus: 0
    memory: "6Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "Concat"
  input_args: <
    argument: "string_0"
    type: "string"
  >
  input_args: <
    argument: "string_1"
    type: "string"
  >
  output_args: <
    argument: "concat_string"
    type: "string"
  >
  attributes: <
    attribute: "lowercase"
    type: "bool"
    default_value: <
      b: false
    >
  >
  resources: <
    cpu: "100m"
    memory: "100Mi"
  >
>
operators: <
  operator: "DeID"
  input_args: <
    argument: "input_stream"
    type: "gst/video"
  >
  output_args: <
    argument: "output_stream"
    type: "gst/video"
  >
  attributes: <
    attribute: "distort_faces"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "distortion_mode"
    type: "string"
    default_value: <
      s: "BLURRING"
    >
  >
  resources: <
    cpu: "3"
    memory: "1Gi"
    latency_budget_ms: 600000
  >
>
operators: <
  operator: "StreamSink"
  input_args: <
    argument: "input"
    type: "special/any"
  >
  resources: <
    cpu: "100m"
    memory: "100Mi"
  >
>
operators: <
  operator: "StreamSource"
  output_args: <
    argument: "output"
    type: "special/any"
  >
  resources: <
    cpu: "100m"
    memory: "100Mi"
  >
>
operators: <
  operator: "OccupancyCounting"
  input_args: <
    argument: "input_stream"
    type: "gst/video"
  >
  output_args: <
    argument: "output_stream"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  attributes: <
    attribute: "detect_person"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "detect_vehicle"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "detect_dwelling"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "lines"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "zones"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "4"
    memory: "1500Mi"
    latency_budget_ms: 120000
  >
>
operators: <
  operator: "StringSplit"
  input_args: <
    argument: "input_string"
    type: "string"
  >
  output_args: <
    argument: "string_0"
    type: "string"
  >
  output_args: <
    argument: "string_1"
    type: "string"
  >
  attributes: <
    attribute: "delimiter"
    type: "string"
    default_value: <
      s: "/"
    >
  >
  resources: <
    cpu: "100m"
    memory: "100Mi"
  >
>
operators: <
  operator: "ClipClassification"
  input_args: <
    argument: "string_0"
    type: "string"
  >
  input_args: <
    argument: "string_1"
    type: "string"
  >
  output_args: <
    argument: "concat_string"
    type: "string"
  >
  attributes: <
    attribute: "lowercase"
    type: "bool"
    default_value: <
      b: false
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "ImageClassificationWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult"
  >
  attributes: <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes: <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "image_classification_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "ImageObjectDetectionWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult"
  >
  attributes: <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes: <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "image_object_detection_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "OccupancyCountingWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  attributes: <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes: <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "person_occupancy_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "vehicle_occupancy_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "VideoActionRecognitionWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult"
  >
  attributes: <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes: <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "video_action_recognition_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "VideoClassificationWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult"
  >
  attributes: <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes: <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "video_classification_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "VideoObjectTrackingWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult"
  >
  attributes: <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes: <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes: <
    attribute: "video_object_tracking_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources: <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "VertexAiCustomWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.aiplatform.v1.PredictResponse"
  >
  attributes <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "vertex_ai_custom_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "GenericObjectDetectorWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult"
  >
  attributes <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "general_object_detection_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "PersonalProtectiveEquipmentDetectorWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput"
  >
  attributes <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "personal_protective_equipment_detection_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
  operator: "BigQueryCustomModelSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.aiplatform.v1.PredictResponse"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryGenericObjectDetectorSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryPersonalProtectiveEquipmentDetectorSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryImageClassificationSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryImageObjectDetectionSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >

>
operators: <
  operator: "BigQueryOccupancyCountSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryPassthroughSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryStreamMetadataSink"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryVideoActionRecognitionSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryVideoClassificationSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "BigQueryVideoObjectTrackingSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqPredictResponse"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.aiplatform.v1.PredictResponse"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqGod"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqPpe"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqIcn"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqIod"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqOc"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqStream"
  input_args: <
    argument: "video"
    type: "gst/video"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqVar"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqVcn"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqVot"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionPredictResponse"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.aiplatform.v1.PredictResponse"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.aiplatform.v1.PredictResponse"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionOccupancyCount"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  output_args: <
    argument: "custom_event"
    type: "protobuf/google.cloud.visionai.v1.AppPlatformCloudFunctionResponse"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "GenericObjectDetection"
  input_args: <
    argument: "input_stream"
    type: "gst/video"
  >
  output_args: <
    argument: "output_stream"
    type: "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult"
  >
  resources <
    cpu: "4"
    gpus: 0
    memory: "1Gi"
    latency_budget_ms: 20000
  >
>
operators: <
  operator: "PersonVehicleDetection"
  input_args: <
    argument: "input_stream"
    type: "gst/video"
  >
  output_args: <
    argument: "output_stream"
    type: "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult"
  >
  attributes: <
    attribute: "detect_person"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "detect_vehicle"
    type: "bool"
    default_value: <
      b: true
    >
  >
  resources: <
    cpu: "1"
    gpus: 0
    memory: "600Mi"
  >
>

operators: <
  operator: "PersonalProtectiveEquipmentDetection"
  input_args: <
    argument: "input_stream"
    type: "gst/video"
  >
  output_args: <
    argument: "output_stream"
    type: "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput"
  >
  attributes: <
    attribute: "enable_face_coverage_detection"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "enable_head_coverage_detection"
    type: "bool"
    default_value: <
      b: true
    >
  >
  attributes: <
    attribute: "enable_hands_coverage_detection"
    type: "bool"
    default_value: <
      b: true
    >
  >
  resources: <
    cpu: "4"
    gpus: 0
    memory: "1Gi"
    latency_budget_ms: 3000
  >
>
operators <
  operator: "SvaiProductRecognizer"
  input_args: <
    argument: "input_stream"
    type: "gst/video"
  >
  output_args: <
    argument: "prediction_result"
    type: "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults"
  >
  attributes <
    attribute: "retail_service_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "retail_endpoint_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "feature"
    type: "string"
    default_value: <
      s: "CAESEAoOYnVpbHRpbi9zdGFibGU"
    >
  >
  attributes <
    attribute: "gcs_destination"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    memory: "1Gi"
    latency_budget_ms: 180000
  >
>
operators: <
  operator: "BigQuerySvaiRecognitionSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults"
  >
  attributes <
    attribute: "big_query_table"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "big_query_service_address"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
>
operators: <
  operator: "CloudFunctionBqRetailPR"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults"
  >
  output_args: <
    argument: "processed_annotation"
    type: "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest"
  >
  attributes <
    attribute: "cloud_function_http_trigger"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "2"
    gpus: 0
    memory: "2Gi"
    latency_budget_ms: 60000
  >
  >
operators: <
operator: "SvaiProductRecognitionWarehouseSink"
  input_args: <
    argument: "annotation"
    type: "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults"
  >
  attributes <
    attribute: "use_insecure_channel"
    type: "bool"
    default_value: <
      b: false
    >
  >
  attributes <
    attribute: "warehouse_endpoint"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "asset_name"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "product_recognizer_data_schema_id"
    type: "string"
    default_value: <
      s: ""
    >
  >
  resources <
    cpu: "1"
    memory: "200Mi"
  >
>
operators: <
operator: "PubSubEvent"
  input_args: <
    argument: "events"
    type: "protobuf/google.cloud.visionai.v1.AppPlatformCloudFunctionResponse"
  >
  attributes <
    attribute: "app_platform_metadata"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "channel"
    type: "string"
    default_value: <
      s: ""
    >
  >
  attributes <
    attribute: "deliver_interval_ms"
    type: "int"
    default_value: <
      i: 5000
    >
  >
  resources <
    cpu: "1"
    memory: "200Mi"
  >
>

`
)
