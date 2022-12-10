/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_WAREHOUSE_WAREHOUSE_INGESTER_H_
#define VISIONAI_WAREHOUSE_WAREHOUSE_INGESTER_H_

#include <atomic>
#include <vector>

#include "google/cloud/visionai/v1/warehouse.grpc.pb.h"
#include "google/cloud/visionai/v1/warehouse.pb.h"
#include "absl/status/status.h"
#include "absl/types/optional.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/telemetry/metrics/labels.h"
#include "visionai/warehouse/warehouse_streaming_grpc_client.h"

namespace visionai {

class WarehouseIngester {
 public:
  explicit WarehouseIngester(const std::string& endpoint,
                             const MetricStreamResourceLabels& labels)
      : WarehouseIngester(endpoint, labels, DefaultConnectionOptions()) {}

  explicit WarehouseIngester(const std::string& endpoint,
                             const MetricStreamResourceLabels& labels,
                             const ConnectionOptions& connection_options)
      : endpoint_(endpoint),
        metric_resource_labels_(labels),
        connection_options_(connection_options) {
    uncompleted_ingestion_count_.store(0, std::memory_order_release);
  }
  virtual ~WarehouseIngester() = default;

  // WarehouseIngester is neither copyable nor movable.
  WarehouseIngester(const WarehouseIngester&) = delete;
  WarehouseIngester& operator=(const WarehouseIngester&) = delete;

  // Initializes GRPC client for warehouse and send initial streaming call to
  // warehouse. Assuming GOOGLE_APPLICATION_CREDENTIALS has been set.
  // asset name format:
  // projects/${project_number}/locations/${location_id}/corpora/${corpus_id}/assets/${asset_id}
  virtual absl::Status Initialize(
      const std::string& asset_name,
      absl::optional<google::protobuf::Duration> timeout);

  // Sends the file in file_path to media warehouse with the start time and end
  // time of the input temporal_partition.
  //
  // If error status is returned, please finish the stream by calling Finish to
  // check out the real error. If would like to retry, please restart the stream
  // by calling Initialize after Finish.
  virtual absl::Status IngestFile(
      const std::string& file_path,
      const google::cloud::visionai::v1::Partition::TemporalPartition&
          temporal_partition);

  // Returns succeeded ingested (absolute) start time and (absolute) end
  // time of the file if ingesting succeeds. Otherwise, it returns the error.
  //
  // If error status is returned, please finish the stream by calling Finish to
  // check out the real error. If would like to retry, please restart the stream
  // by calling Initialize after Finish.
  virtual absl::StatusOr<
      google::cloud::visionai::v1::Partition::TemporalPartition>
  GetIngestResponse();

  // Returns list of succeeded ingested (absolute) start time and (absolute) end
  // time of the file if ingesting succeeds. Otherwise, it returns the error.
  //
  // If error status is returned, please finish the stream by calling Finish to
  // check out the real error. If would like to retry, please restart the stream
  // by calling Initialize after Finish.
  virtual std::vector<
      absl::StatusOr<google::cloud::visionai::v1::Partition::TemporalPartition>>
  GetAllIngestResponse();

  // Ends the streams.
  virtual absl::Status Finish();

  // Sets up grpc channel for warehouse service.
  absl::StatusOr<std::shared_ptr<grpc::Channel>> SetUpChannel(
      google::protobuf::Duration timeout);

  // Initializes client for streaming warehouse and sends first config request.
  absl::Status InitializeClient(std::shared_ptr<grpc::Channel> channel);

  // Sends first config request to open streaming.
  absl::Status SendConfigRequest(const std::string& asset_name);

 private:
  // Returns default duration for channel timeout.
  google::protobuf::Duration GetDefaultTimeout();

  std::atomic<int> uncompleted_ingestion_count_;
  std::string endpoint_ = "";
  std::unique_ptr<WarehouseStreamingGrpcClient<
      google::cloud::visionai::v1::Warehouse,
      google::cloud::visionai::v1::IngestAssetRequest,
      google::cloud::visionai::v1::IngestAssetResponse>>
      client_;

  MetricStreamResourceLabels metric_resource_labels_;
  std::string asset_name_;
  ConnectionOptions connection_options_;
};

}  // namespace visionai

#endif  // VISIONAI_WAREHOUSE_WAREHOUSE_INGESTER_H_
