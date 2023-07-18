// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_STATS_H_
#define THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_STATS_H_

#include <prometheus/counter.h>
#include <prometheus/histogram.h>
#include <prometheus/registry.h>

#include <string>
#include <utility>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "opencensus/stats/stats.h"
#include "opencensus/tags/tag_key.h"
#include "prometheus/histogram.h"
#include "visionai/util/telemetry/metrics/stats_macros.h"

namespace visionai {
class MetricsTagKey {
 public:
  static absl::flat_hash_map<std::string, opencensus::tags::TagKey> tag_keys_;

  static opencensus::tags::TagKey AnalysisName();
  static opencensus::tags::TagKey AnalyzerName();
  static opencensus::tags::TagKey StreamName();
  static opencensus::tags::TagKey EventName();
  static opencensus::tags::TagKey ChannelName();
  static opencensus::tags::TagKey ConsumerName();
  static opencensus::tags::TagKey ServiceName();
  static opencensus::tags::TagKey ResponseCode();
  static opencensus::tags::TagKey ProjectID();
  static opencensus::tags::TagKey LocationID();
  static opencensus::tags::TagKey ClusterID();
  static opencensus::tags::TagKey StreamID();
  static opencensus::tags::TagKey EventID();

  static void RegisterTagKeys(
      const absl::flat_hash_map<std::string, std::string>& global_tags);

  static std::pair<opencensus::tags::TagKey, std::string> AnalysisNameTag();
  static std::pair<opencensus::tags::TagKey, std::string> AnalyzerNameTag();
};

class MetricsMeasure {
 public:
  // LVA metrics
  // Measures used by Streams/LVA system.
  static opencensus::stats::MeasureInt64 StreamsSentBytesCount();
  static opencensus::stats::MeasureInt64 StreamsReceivedBytesCount();
  static opencensus::stats::MeasureInt64 StreamsSentPacketsCount();
  static opencensus::stats::MeasureInt64 StreamsReceivedPacketsCount();

  // App Platform metrics
  // Measures used by operators.
  static opencensus::stats::MeasureDouble OperatorRequestLatency();
  static opencensus::stats::MeasureInt64 OperatorRequestCount();
  static opencensus::stats::MeasureDouble CustomModelPredictLatency();
  static opencensus::stats::MeasureInt64 CustomModelPredictCount();

  // Measure used by Streams/LVA system
  static opencensus::stats::MeasureDouble InstanceUptime();

  // This is the measurement required by use_start_time_metric.
  static opencensus::stats::MeasureInt64 ProcessStartTime();

  static void RegisterMeasures() {
    StreamsSentBytesCount();
    StreamsReceivedBytesCount();
    StreamsSentPacketsCount();
    StreamsReceivedPacketsCount();

    OperatorRequestLatency();
    OperatorRequestCount();
    CustomModelPredictLatency();
    CustomModelPredictCount();
    InstanceUptime();

    ProcessStartTime();
  }
};

class MetricsView {
 public:
  static void RegisterViews(
      const absl::flat_hash_map<std::string, std::string>& global_tags);

 private:
  static opencensus::stats::ViewDescriptor StreamsReceivedBytesCount();
  static opencensus::stats::ViewDescriptor StreamsReceivedPacketsCount();
  static opencensus::stats::ViewDescriptor StreamsSentBytesCount();
  static opencensus::stats::ViewDescriptor StreamsSentPacketsCount();

  static opencensus::stats::ViewDescriptor OperatorRequestCount();
  static opencensus::stats::ViewDescriptor OperatorRequestLatency();
  static opencensus::stats::ViewDescriptor CustomModelPredictCount();
  static opencensus::stats::ViewDescriptor CustomModelPredictLatency();
  static opencensus::stats::ViewDescriptor InstanceUptime();

  static opencensus::stats::ViewDescriptor ProcessStartTime();
};

std::string GetAnalysisName();
std::string GetAnalyzerName();
std::string GetAnalysisId();
std::string GetProcessName();
std::string GetInputStream();

// Seperation between Opencensus and Prometheus client library
//
// Prometheus exporter will translate the opencensus SUM metrics to prometheus
// UNTYPED metrics. Prometheus receiver will then convert the untyped metrics to
// opentelemetry Gauge which is not a good fit for metrics like bytes_received.
// So we introduced the prometheus client library. Currently, both opencensus
// and prometheus are registered. You can use either of them to expose metrics.

std::shared_ptr<prometheus::Registry> GlobalRegistry();

COUNTER(stream_received_packets_total,
        "Total number of packets the stream received.", *GlobalRegistry());
COUNTER(stream_received_bytes_total,
        "Total number of bytes the stream received.", *GlobalRegistry());
COUNTER(stream_sent_packets_total, "Total number of packets the stream sent.",
        *GlobalRegistry());
COUNTER(stream_sent_bytes_total, "Total number of bytes the stream received.",
        *GlobalRegistry());

HISTOGRAM(streaming_server_rpc_latency, "The rpc latency of streaming server.",
          *GlobalRegistry());

HISTOGRAM(stream_receive_first_packet_latency,
          "The latency of receiving the first packet from the streaming server",
          *GlobalRegistry());

COUNTER(
    capture_received_packets_from_stream_total,
    "Total number of packets the downstream consumer received from the stream.",
    *GlobalRegistry());
COUNTER(
    capture_received_bytes_from_stream_total,
    "Total number of bytes the downstream consumer received from the stream",
    *GlobalRegistry());
COUNTER(capture_received_events_total,
        "Total number of events the downstream consumer received from the "
        "event discovery server",
        *GlobalRegistry());

COUNTER(hls_segments_count_total,
        "Total number of the video segments generated for HLS livestream.",
        *GlobalRegistry());
COUNTER(hls_segments_duration_sec_total,
        "Total duration of the video segments generated for HLS livestream.",
        *GlobalRegistry());
GAUGE(hls_first_segment_latency_sec,
      "The processing latency to generate the first video segment for HLS "
      "livestream .",
      *GlobalRegistry());
GAUGE(hls_packet_delay_sec,
      "The processing delay of the packets in HLS livestream pipeline",
      *GlobalRegistry());

COUNTER(mwh_grpc_client_ingest_file_success_count_total,
        "Total number of the files successfully ingested to MWH.",
        *GlobalRegistry());
COUNTER(mwh_grpc_client_ingest_file_success_bytes_total,
        "Total number of bytes successfully ingested to MWH.",
        *GlobalRegistry());
COUNTER(mwh_grpc_client_ingest_asset_status,
        "The Warehouse IngestAsset GRPC finish status.", *GlobalRegistry());
COUNTER(mwh_exporter_segments_local_count_total,
        "Total number of the video segments created locally to export to MWH.",
        *GlobalRegistry());
COUNTER(mwh_exporter_segments_dropped_count_total,
        "Total number of the video segments failed to export to MWH.",
        *GlobalRegistry());

// Metrics for LVA runtime io.
COUNTER(lva_ais_input_packets_total,
        "Total number of the packets successfully received by lva ais channel.",
        *GlobalRegistry());
COUNTER(lva_ais_input_bytes_total,
        "Total number of bytes successfully received by lva ais channel.",
        *GlobalRegistry());
COUNTER(lva_ais_output_packets_total,
        "Total number of the packets successfully sent by lva ais channel.",
        *GlobalRegistry());
COUNTER(lva_ais_output_bytes_total,
        "Total number of bytes successfully sent by lva ais channel.",
        *GlobalRegistry());

COUNTER(
    lva_grpc_input_packets_total,
    "Total number of the packets successfully received by lva grpc channel.",
    *GlobalRegistry());
COUNTER(lva_grpc_input_bytes_total,
        "Total number of bytes successfully received by lva grpc channel.",
        *GlobalRegistry());
COUNTER(lva_grpc_output_packets_total,
        "Total number of the packets successfully sent by lva grpc channel.",
        *GlobalRegistry());
COUNTER(lva_grpc_output_bytes_total,
        "Total number of bytes successfully sent by lva grpc channel.",
        *GlobalRegistry());

COUNTER(lva_gcs_input_packsts_total,
        "Total number of the packets successfully received by lva gcs channel.",
        *GlobalRegistry());
COUNTER(lva_gcs_input_bytes_total,
        "Total number of bytes successfully received by lva gcs channel.",
        *GlobalRegistry());

COUNTER(lva_warehouse_video_input_video_packsts_total,
        "Total number of the video packets successfully received by "
        "lva warehouse vidoe channel.",
        *GlobalRegistry());
COUNTER(lva_warehouse_video_input_audio_packsts_total,
        "Total number of the audio packets successfully received by "
        "lva warehouse vidoe channel.",
        *GlobalRegistry());
COUNTER(lva_warehouse_video_input_video_bytes_total,
        "Total number of the video bytes successfully received by "
        "lva warehouse vidoe channel.",
        *GlobalRegistry());
COUNTER(lva_warehouse_video_input_audio_bytes_total,
        "Total number of the audio bytes successfully received by "
        "lva warehouse vidoe channel.",
        *GlobalRegistry());

COUNTER(lva_runtime_packets_drop_total,
        "Total number of packets dropped by lva runtime", *GlobalRegistry());

HISTOGRAM(lva_runtime_latency,
        "Packets processing latency of lva operator.",
        *GlobalRegistry());

// Metrics for pretrained operators.
// Metrics for generic object detector.
COUNTER(
    lva_god_input_packets_total,
    "Total number of the packets successfully received by god operator.",
    *GlobalRegistry());
COUNTER(lva_god_input_bytes_total,
        "Total number of bytes successfully received by god operator.",
        *GlobalRegistry());
COUNTER(lva_god_output_packets_total,
        "Total number of the packets successfully sent by god operator.",
        *GlobalRegistry());
COUNTER(lva_god_output_bytes_total,
        "Total number of bytes successfully sent by lva god operator.",
        *GlobalRegistry());
HISTOGRAM(lva_god_packet_processing_latency,
          "The processing latency of generic object detection operator.",
          *GlobalRegistry());

// Metrics for person vehicle detector.
COUNTER(lva_pvd_input_packets_total,
        "Total number of the packets successfully received by person vehicle "
        "detection operator.",
        *GlobalRegistry());
COUNTER(lva_pvd_input_bytes_total,
        "Total number of bytes successfully received by person vehicle "
        "detection operator.",
        *GlobalRegistry());
COUNTER(lva_pvd_output_packets_total,
        "Total number of the packets successfully sent by person vehicle "
        "detection operator.",
        *GlobalRegistry());
COUNTER(lva_pvd_output_bytes_total,
        "Total number of bytes successfully sent by lva person vehicle "
        "detection operator.",
        *GlobalRegistry());

// Metrics for occupancy counting detector.
COUNTER(lva_oc_input_packets_total,
        "Total number of the packets successfully received by occupancy "
        "counting operator.",
        *GlobalRegistry());
COUNTER(lva_oc_input_bytes_total,
        "Total number of bytes successfully received by occupancy counting "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_oc_output_packets_total,
        "Total number of the packets successfully sent by occupancy counting "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_oc_output_bytes_total,
        "Total number of bytes successfully sent by lva occupancy counting "
        "operator.",
        *GlobalRegistry());

// Metrics for motion detector.
COUNTER(lva_md_input_packets_total,
        "Total number of the packets successfully received by motion "
        "detection operator.",
        *GlobalRegistry());
COUNTER(lva_md_input_bytes_total,
        "Total number of bytes successfully received by motion detection "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_md_output_packets_total,
        "Total number of the packets successfully sent by motion detection "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_md_output_bytes_total,
        "Total number of bytes successfully sent by lva motion detection "
        "operator.",
        *GlobalRegistry());

// Metrics for text detection.
COUNTER(lva_text_input_packets_total,
        "Total number of the packets successfully received by text detection "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_text_input_bytes_total,
        "Total number of bytes successfully received by text detection "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_text_output_packets_total,
        "Total number of the packets successfully sent by text detection "
        "operator.",
        *GlobalRegistry());
COUNTER(lva_text_output_bytes_total,
        "Total number of bytes successfully sent by lva text detection "
        "operator.",
        *GlobalRegistry());

COUNTER(lva_text_proxy_server_error_total,
        "Total number of errors calling proxy server received by lva text "
        "detection operator",
        *GlobalRegistry());

COUNTER(lva_text_proxy_server_request_total,
        "Total number of requests calling proxy server by lva text detection "
        "operator",
        *GlobalRegistry());

const prometheus::Histogram::BucketBoundaries latency_boundaries_ms{
    0,   0.01, 0.05, 0.1,  0.3,   0.6,   0.8,   1,     2,   3,   4,
    5,   6,    8,    10,   13,    16,    20,    25,    30,  40,  50,
    65,  80,   100,  130,  160,   200,   250,   300,   400, 500, 650,
    800, 1000, 2000, 5000, 10000, 20000, 50000, 100000};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_TELEMETRY_METRICS_STATS_H_
