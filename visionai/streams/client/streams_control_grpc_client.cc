// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streams_control_grpc_client.h"

#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

#include "glog/logging.h"
#include "google/cloud/visionai/v1/streams_service.pb.h"
#include "google/longrunning/operations.grpc.pb.h"
#include "google/longrunning/operations.pb.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "include/grpcpp/grpcpp.h"
#include "visionai/util/net/exponential_backoff.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/net/grpc/status_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
using google::cloud::visionai::v1::Cluster;
using google::cloud::visionai::v1::CreateClusterRequest;
using google::cloud::visionai::v1::CreateEventRequest;
using google::cloud::visionai::v1::CreateSeriesRequest;
using google::cloud::visionai::v1::CreateStreamRequest;
using google::cloud::visionai::v1::DeleteClusterRequest;
using google::cloud::visionai::v1::DeleteEventRequest;
using google::cloud::visionai::v1::DeleteSeriesRequest;
using google::cloud::visionai::v1::DeleteStreamRequest;
using google::cloud::visionai::v1::Event;
using google::cloud::visionai::v1::GetClusterRequest;
using google::cloud::visionai::v1::GetEventRequest;
using google::cloud::visionai::v1::GetSeriesRequest;
using google::cloud::visionai::v1::GetStreamRequest;
using google::cloud::visionai::v1::ListClustersRequest;
using google::cloud::visionai::v1::ListClustersResponse;
using google::cloud::visionai::v1::ListEventsRequest;
using google::cloud::visionai::v1::ListEventsResponse;
using google::cloud::visionai::v1::ListSeriesRequest;
using google::cloud::visionai::v1::ListSeriesResponse;
using google::cloud::visionai::v1::ListStreamsRequest;
using google::cloud::visionai::v1::ListStreamsResponse;
using google::cloud::visionai::v1::Series;
using google::cloud::visionai::v1::Stream;
using google::cloud::visionai::v1::UpdateStreamRequest;
using google::longrunning::Operation;
using google::longrunning::Operations;

constexpr char kGrpcMetadata[] = "x-goog-request-params";

// These correspond to symbols: ⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏
constexpr const char *kSpinSymbols[] = {"\u280B", "\u2819", "\u2839", "\u2838",
                                        "\u283C", "\u2834", "\u2826", "\u2827",
                                        "\u2807", "\u280F"};
constexpr size_t kNumSpinSymbols = sizeof(kSpinSymbols) / sizeof(char *);
constexpr int kSpinIntervalMs = 80;

// A spinner class that just prints a spinning symbol in the background.
class Spinner {
 public:
  explicit Spinner(const std::string &text) : text_(text) {
    keep_spinning_.store(true);
    t_ = std::thread([this]() {
      size_t i = 0;
      std::cout << text_ << "   ";
      while (keep_spinning_.load()) {
        i = (i == kNumSpinSymbols - 1) ? 0 : i + 1;
        std::cout << "\b\b\b"
                  << " " << kSpinSymbols[i] << " ";
        std::cout.flush();
        absl::SleepFor(absl::Milliseconds(kSpinIntervalMs));
      }
      std::cout << std::endl;
    });
  }

  ~Spinner() {
    keep_spinning_.store(false);
    if (t_.joinable()) {
      t_.join();
    } else {
      t_.detach();
    }
  }

 private:
  std::atomic<bool> keep_spinning_;
  std::thread t_;
  const std::string text_;
};

}  // namespace

absl::StatusOr<std::unique_ptr<StreamsControlGrpcClient>>
StreamsControlGrpcClient::Create(const Options &options) {
  auto client = absl::WrapUnique(new StreamsControlGrpcClient());
  client->options_ = options;
  VAI_RETURN_IF_ERROR(SetAuthorizationHeaderFromJsonKey(
      client->options_.target_address, client->options_.connection_options))
      << "while configuring authorization information";
  auto channel =
      CreateChannel(options.target_address, options.connection_options);
  client->streams_stub_ =
      google::cloud::visionai::v1::StreamsService::NewStub(channel);
  return std::move(client);
}

absl::StatusOr<std::vector<Cluster>> StreamsControlGrpcClient::ListClusters(
    const std::string &parent, const std::string &filter) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  ListClustersRequest request;
  request.set_parent(parent);
  if (!filter.empty()) {
    request.set_filter(filter);
  }
  ListClustersResponse response;
  VAI_RETURN_IF_ERROR(
      streams_stub_->ListClusters(context.get(), request, &response));

  return std::vector<Cluster>{response.clusters().begin(),
                              response.clusters().end()};
}

absl::StatusOr<Cluster> StreamsControlGrpcClient::GetCluster(
    const std::string &parent, const std::string &cluster_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  GetClusterRequest request;
  request.set_name(absl::StrFormat("%s/clusters/%s", parent, cluster_id));
  Cluster cluster;
  VAI_RETURN_IF_ERROR(streams_stub_->GetCluster(context.get(), request, &cluster));
  return cluster;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::CreateCluster(
    const std::string &parent, const std::string &cluster_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  CreateClusterRequest request;
  request.set_parent(parent);
  request.set_cluster_id(cluster_id);
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->CreateCluster(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::DeleteCluster(
    const std::string &parent, const std::string &cluster_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  DeleteClusterRequest request;
  request.set_name(absl::StrFormat("%s/clusters/%s", parent, cluster_id));
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->DeleteCluster(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<std::vector<Stream>> StreamsControlGrpcClient::ListStreams(
    const std::string &parent, const std::string &filter) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  ListStreamsRequest request;
  request.set_parent(parent);
  if (!filter.empty()) {
    request.set_filter(filter);
  }
  ListStreamsResponse response;
  VAI_RETURN_IF_ERROR(
      streams_stub_->ListStreams(context.get(), request, &response));

  return std::vector<Stream>{response.streams().begin(),
                             response.streams().end()};
}

absl::StatusOr<Stream> StreamsControlGrpcClient::GetStream(
    const std::string &parent, const std::string &stream_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  GetStreamRequest request;
  request.set_name(absl::StrFormat("%s/streams/%s", parent, stream_id));
  Stream stream;
  VAI_RETURN_IF_ERROR(streams_stub_->GetStream(context.get(), request, &stream));
  return stream;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::CreateStream(
    const std::string &parent, const std::string &stream_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  CreateStreamRequest request;
  request.set_parent(parent);
  request.set_stream_id(stream_id);
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->CreateStream(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::UpdateStream(
    const std::string &parent, const Stream &stream,
    const google::protobuf::FieldMask &field_mask) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  UpdateStreamRequest request;
  *request.mutable_stream() = stream;
  *request.mutable_update_mask() = field_mask;
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->UpdateStream(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::DeleteStream(
    const std::string &parent, const std::string &stream_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  DeleteStreamRequest request;
  request.set_name(absl::StrFormat("%s/streams/%s", parent, stream_id));
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->DeleteStream(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<std::vector<Event>> StreamsControlGrpcClient::ListEvents(
    const std::string &parent, const std::string &filter) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  ListEventsRequest request;
  request.set_parent(parent);
  if (!filter.empty()) {
    request.set_filter(filter);
  }
  ListEventsResponse response;
  VAI_RETURN_IF_ERROR(streams_stub_->ListEvents(context.get(), request, &response));

  return std::vector<Event>{response.events().begin(), response.events().end()};
}

absl::StatusOr<Event> StreamsControlGrpcClient::GetEvent(
    const std::string &parent, const std::string &event_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  GetEventRequest request;
  request.set_name(absl::StrFormat("%s/events/%s", parent, event_id));
  Event event;
  VAI_RETURN_IF_ERROR(streams_stub_->GetEvent(context.get(), request, &event));
  return event;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::CreateEvent(
    const std::string &parent, const std::string &event_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  CreateEventRequest request;
  request.set_parent(parent);
  request.set_event_id(event_id);
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->CreateEvent(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::DeleteEvent(
    const std::string &parent, const std::string &event_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  DeleteEventRequest request;
  request.set_name(absl::StrFormat("%s/events/%s", parent, event_id));
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->DeleteEvent(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<std::vector<Series>> StreamsControlGrpcClient::ListSeries(
    const std::string &parent, const std::string &filter) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  ListSeriesRequest request;
  request.set_parent(parent);
  if (!filter.empty()) {
    request.set_filter(filter);
  }
  ListSeriesResponse response;
  VAI_RETURN_IF_ERROR(streams_stub_->ListSeries(context.get(), request, &response));

  return std::vector<Series>{response.series().begin(),
                             response.series().end()};
}

absl::StatusOr<Series> StreamsControlGrpcClient::GetSeries(
    const std::string &parent, const std::string &series_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  GetSeriesRequest request;
  request.set_name(absl::StrFormat("%s/series/%s", parent, series_id));
  Series series;
  VAI_RETURN_IF_ERROR(streams_stub_->GetSeries(context.get(), request, &series));
  return series;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::CreateSeries(
    const std::string &parent, const std::string &series_id,
    const std::string &stream_id, const std::string &event_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  CreateSeriesRequest request;
  request.set_parent(parent);
  request.mutable_series()->set_event(
      absl::StrFormat("%s/events/%s", parent, event_id));
  request.mutable_series()->set_stream(
      absl::StrFormat("%s/streams/%s", parent, stream_id));
  request.set_series_id(series_id);
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->CreateSeries(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<Operation> StreamsControlGrpcClient::DeleteSeries(
    const std::string &parent, const std::string &series_id) {
  auto context = CreateClientContext(options_.connection_options);
  context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));
  DeleteSeriesRequest request;
  request.set_name(absl::StrFormat("%s/series/%s", parent, series_id));
  Operation operation;
  VAI_RETURN_IF_ERROR(
      streams_stub_->DeleteSeries(context.get(), request, &operation));
  return operation;
}

absl::StatusOr<google::protobuf::Any> StreamsControlGrpcClient::Wait(
    const std::string &parent, const Operation &operation) {
  if (operation.has_error()) {
    return absl::InternalError(absl::StrFormat("Operation failed, response: %s",
                                               operation.DebugString()));
  }
  if (operation.done()) {
    return operation.response();
  }
  auto channel =
      CreateChannel(options_.target_address, options_.connection_options);
  if (channel == nullptr) {
    return absl::UnknownError("Failed to create a gRPC channel");
  }
  std::unique_ptr<Operations::Stub> stub = Operations::NewStub(channel);
  if (stub == nullptr) {
    return absl::UnknownError("Failed to create a gRPC stub");
  }

  // Gives around 30 minutes for a 20 trial limit.
  ExponentialBackoff exponential_backoff(absl::Seconds(2), absl::Minutes(2),
                                         2.0f);
  int remaining_trials = 20;
  Spinner spinner(absl::StrFormat("Waiting for long running operation %s",
                                  operation.name()));
  do {
    google::longrunning::GetOperationRequest request;
    request.set_name(operation.name());
    google::longrunning::Operation response;
    auto context = CreateClientContext(options_.connection_options);
    context->AddMetadata(kGrpcMetadata, absl::StrFormat("parent=%s", parent));

    auto grpc_status = stub->GetOperation(context.get(), request, &response);
    if (!grpc_status.ok()) {
      return ToAbseilStatus(grpc_status);
    }
    if (response.has_error()) {
      return absl::InternalError(absl::StrFormat(
          "Operation failed, response: %s", response.DebugString()));
    }
    if (response.done()) {
      return response.response();
    }
    if (--remaining_trials <= 0) {
      break;
    }
    exponential_backoff.Wait();
  } while (true);
  return absl::DeadlineExceededError("Failed waiting for operation.");
}

}  // namespace visionai
