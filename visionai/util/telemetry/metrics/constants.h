/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_CONSTANTS_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_CONSTANTS_H_

namespace visionai {

// Tag containing the resource name of an LVA analysis
inline constexpr char kTagKeyAnalysisName[] = "analysisName";
// Tag containing the resource name of an LVA analyzer
inline constexpr char kTagKeyAnalyzerName[] = "analyzerName";
// Tag containing the resource name of a stream
inline constexpr char kTagKeyStreamName[] = "streamName";
// Tag containing the resource name of an event
inline constexpr char kTagKeyEventName[] = "eventName";
// Tag containing the resource name of a channel
inline constexpr char kTagKeyChannelName[] = "channelName";
// Tag containing the resource name of a consumer
inline constexpr char kTagKeyConsumerName[] = "consumer_name";
// Tag containing the name of an external service
inline constexpr char kTagKeyServiceName[] = "serviceName";
// Tag containing the response code from the external service
inline constexpr char kTagKeyResponseCode[] = "responseCode";
// Tag containing the project ID.
inline constexpr char kTagKeyProjectID[] = "project_id";
// Tag containing the location ID.
inline constexpr char kTagKeyLocationID[] = "location_id";
// Tag containing the cluster ID.
inline constexpr char kTagKeyClusterID[] = "cluster_id";
// Tag containing the stream ID.
inline constexpr char kTagKeyStreamID[] = "stream_id";
// Tag containing the event ID.
inline constexpr char kTagKeyEventID[] = "event_id";

// Measures
// The latency of an RPC request from an operator
inline constexpr char kMeasureOperatorRequestLatency[] =
    "app_platform_request_latency";
// The RPC request count from an operator
inline constexpr char kMeasureOperatorRequestCount[] =
    "app_platform_request_count";
// The latency of a prediction from a custom model operator
inline constexpr char kMeasureCustomModelPredictLatency[] =
    "app_platform_custom_model_predict_latency";
// The predict count from a custom model operator
inline constexpr char kMeasureCustomModelPredictCount[] =
    "app_platform_custom_model_predict_count";
// The uptime of an instance
inline constexpr char kMeasureInstanceUptime[] = "app_platform_uptime";

inline constexpr char kMeasureStreamsSentBytesCount[] =
    "streams_sent_bytes_count";
inline constexpr char kMeasureStreamsReceivedBytesCount[] =
    "streams_received_bytes_count";
inline constexpr char kMeasureStreamsSentPacketsCount[] =
    "streams_sent_packets_count";
inline constexpr char kMeasureStreamsReceivedPacketsCount[] =
    "streams_received_packets_count";
inline constexpr char kMeasureProcessStartTime[] = "process_start_time";

// Views
// Views corresponding to each measure
inline constexpr char kViewOperatorRequestCount[] =
    "app_platform_request_count";
inline constexpr char kViewOperatorRequestLatency[] =
    "app_platform_request_latency";
inline constexpr char kViewCustomModelPredictCount[] =
    "app_platform_custom_model_predict_count";
inline constexpr char kViewCustomModelPredictLatency[] =
    "app_platform_custom_model_predict_latency";
inline constexpr char kViewInstanceUptime[] = "app_platform_instance_uptime";

inline constexpr char kViewStreamsSentBytesCount[] = "streams_sent_bytes_count";
inline constexpr char kViewStreamsReceivedBytesCount[] =
    "streams_received_bytes_count";
inline constexpr char kViewStreamsSentPacketsCount[] =
    "streams_sent_packets_count";
inline constexpr char kViewStreamsReceivedPacketsCount[] =
    "streams_received_packets_count";
inline constexpr char kViewProcessStartTime[] = "process_start_time";

// Units
// Dimensionless
inline constexpr char kCount[] = "1";
// Bytes
inline constexpr char kBy[] = "By";
// Milliseconds
inline constexpr char kMs[] = "ms";
// Seconds
inline constexpr char kSecond[] = "seconds";

// Service names
// BigQuery
inline constexpr char kServiceNameBigQuery[] = "BigQuery";
// CloudFunction
inline constexpr char kServiceNameCloudFunction[] = "CloudFunction";
// MediaWarehouse
inline constexpr char kServiceNameMediaWarehouse[] = "MediaWarehouse";
// Pub/Sub
inline constexpr char kServiceNamePubSub[] = "Pub/Sub";

// Metrics-related environment variables
// Analysis name
inline constexpr char kAnalysisNameEnv[] = "ANALYSIS_NAME";
// Analyzer name
inline constexpr char kAnalyzerNameEnv[] = "ANALYZER_NAME";

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_CONSTANTS_H_
