// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/warehouse/warehouse_ingester.h"

#include "glog/logging.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/telemetry/metrics/stats.h"
#include "visionai/warehouse/warehouse_streaming_grpc_client.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

using ::google::cloud::visionai::v1::IngestAssetRequest;
using ::google::cloud::visionai::v1::IngestAssetResponse;
using ::google::cloud::visionai::v1::Partition;
using ::google::cloud::visionai::v1::Warehouse;
using ::visionai::ConnectionOptions;

#define METRIC_WAREHOUSE_INGEST_ASSET_STATUS(labels, asset, status) \
  mwh_grpc_client_ingest_asset_status()                             \
      .Add({{"project_id", labels.project_id},                      \
            {"location_id", labels.location_id},                    \
            {"cluster_id", labels.cluster_id},                      \
            {"stream_id", labels.stream_id},                        \
            {"asset_name", asset},                                  \
            {"status", status}})                                    \
      .Increment();

#define METRIC_WAREHOUSE_INGEST_FILE_SUCCESS_COUNT(labels, asset) \
  mwh_grpc_client_ingest_file_success_count_total()               \
      .Add({{"project_id", labels.project_id},                    \
            {"location_id", labels.location_id},                  \
            {"cluster_id", labels.cluster_id},                    \
            {"stream_id", labels.stream_id},                      \
            {"asset_name", asset}})                               \
      .Increment();

absl::Status WarehouseIngester::Initialize(
    const std::string& asset_name,
    absl::optional<google::protobuf::Duration> timeout) {
  VAI_ASSIGN_OR_RETURN(std::shared_ptr<grpc::Channel> channel,
                   SetUpChannel(timeout.has_value() ? timeout.value()
                                                    : GetDefaultTimeout()));
  VAI_RETURN_IF_ERROR(InitializeClient(channel)).LogError();
  uncompleted_ingestion_count_.store(0, std::memory_order_release);
  asset_name_ = asset_name;
  return SendConfigRequest(asset_name);
}

absl::Status WarehouseIngester::IngestFile(
    const std::string& file_path,
    const Partition::TemporalPartition& temporal_partition) {
  if (client_ == nullptr) {
    return absl::FailedPreconditionError("Client Not Initialized");
  }

  std::string data;
  absl::Status file_status = visionai::GetFileContents(file_path, &data);
  if (!file_status.ok()) {
    LOG(ERROR) << "Failed to get content of the file " << file_status.message();
    return file_status;
  }

  IngestAssetRequest request;
  request.mutable_time_indexed_data()->set_data(data);
  *request.mutable_time_indexed_data()->mutable_temporal_partition() =
      temporal_partition;

  VAI_RETURN_IF_ERROR(client_->SendRequest(request))
      << " fail file: " << file_path << " temporal partition "
      << temporal_partition.ShortDebugString();
  uncompleted_ingestion_count_.fetch_add(1, std::memory_order_release);
  return absl::OkStatus();
}

absl::StatusOr<Partition::TemporalPartition>
WarehouseIngester::GetIngestResponse() {
  if (client_ == nullptr) {
    return absl::FailedPreconditionError("Client Not Initialized");
  }

  uncompleted_ingestion_count_.fetch_sub(1, std::memory_order_release);
  IngestAssetResponse response;
  VAI_RETURN_IF_ERROR(client_->GetResponse(&response));

  return response.successfully_ingested_partition();
}

std::vector<absl::StatusOr<Partition::TemporalPartition>>
WarehouseIngester::GetAllIngestResponse() {
  std::vector<absl::StatusOr<Partition::TemporalPartition>> response;

  while (uncompleted_ingestion_count_.load(std::memory_order_acquire) > 0) {
    response.push_back(GetIngestResponse());
  }

  return response;
}

absl::Status WarehouseIngester::Finish() {
  if (client_ == nullptr) {
    return absl::FailedPreconditionError("Client Not Initialized");
  }

  if (uncompleted_ingestion_count_.load(std::memory_order_acquire) > 0) {
    return absl::FailedPreconditionError(
        "Please get all response before finish");
  }

  client_->WritesDone();
  auto status = client_->Finish();
  return status;
}

absl::StatusOr<std::shared_ptr<grpc::Channel>> WarehouseIngester::SetUpChannel(
    google::protobuf::Duration timeout) {
  *connection_options_.mutable_channel_options()->mutable_keepalive_timeout() =
      timeout;
  return visionai::CreateChannel(endpoint_, connection_options_);
}

absl::Status WarehouseIngester::InitializeClient(
    std::shared_ptr<grpc::Channel> channel) {
  if (channel == nullptr) {
    return absl::FailedPreconditionError("Null Warehouse Channel");
  }

  ConnectionOptions connection_options = visionai::DefaultConnectionOptions();
  connection_options.mutable_ssl_options()->set_use_insecure_channel(false);
  client_ = std::make_unique<WarehouseStreamingGrpcClient<
      Warehouse, IngestAssetRequest, IngestAssetResponse>>(connection_options);
  client_->Initialize(channel);

  return absl::OkStatus();
}

absl::Status WarehouseIngester::SendConfigRequest(
    const std::string& asset_name) {
  IngestAssetRequest request;
  request.mutable_config()->set_asset(asset_name);
  request.mutable_config()->mutable_video_type()->set_container_format(
      IngestAssetRequest::Config::VideoType::CONTAINER_FORMAT_MP4);
  if (!client_->SendRequest(request).ok()) {
    LOG(ERROR) << "Fail to send request: " << request.ShortDebugString();
    client_->WritesDone();
    return client_->Finish();
  }

  return absl::OkStatus();
}

google::protobuf::Duration WarehouseIngester::GetDefaultTimeout() {
  google::protobuf::Duration duration;
  duration.set_seconds(60);
  return duration;
}

}  // namespace visionai
