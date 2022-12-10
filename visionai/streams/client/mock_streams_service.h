/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_STREAMS_SERVICE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_STREAMS_SERVICE_H_

#include "google/cloud/visionai/v1/streams_resources.pb.h"
#include "google/cloud/visionai/v1/streams_service.grpc.pb.h"
#include "google/cloud/visionai/v1/streams_service.pb.h"
#include "google/longrunning/operations.pb.h"
#include "gmock/gmock.h"
#include "visionai/testing/grpc/mock_grpc.h"

namespace visionai {

class MockStreamsService
    : public ::google::cloud::visionai::v1::StreamsService::Service {
 public:
  using ServiceType = ::google::cloud::visionai::v1::StreamsService;

  GRPC_UNARY_MOCK(ListClusters,
                  google::cloud::visionai::v1::ListClustersRequest,
                  google::cloud::visionai::v1::ListClustersResponse);
  GRPC_UNARY_MOCK(GetCluster,
                  google::cloud::visionai::v1::GetClusterRequest,
                  google::cloud::visionai::v1::Cluster);
  GRPC_UNARY_MOCK(CreateCluster,
                  google::cloud::visionai::v1::CreateClusterRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(UpdateCluster,
                  google::cloud::visionai::v1::UpdateClusterRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(DeleteCluster,
                  google::cloud::visionai::v1::DeleteClusterRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(ListStreams,
                  google::cloud::visionai::v1::ListStreamsRequest,
                  google::cloud::visionai::v1::ListStreamsResponse);
  GRPC_UNARY_MOCK(GetStream,
                  google::cloud::visionai::v1::GetStreamRequest,
                  google::cloud::visionai::v1::Stream);
  GRPC_UNARY_MOCK(CreateStream,
                  google::cloud::visionai::v1::CreateStreamRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(UpdateStream,
                  google::cloud::visionai::v1::UpdateStreamRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(DeleteStream,
                  google::cloud::visionai::v1::DeleteStreamRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(GetStreamThumbnail,
                  google::cloud::visionai::v1::GetStreamThumbnailRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(ListEvents,
                  google::cloud::visionai::v1::ListEventsRequest,
                  google::cloud::visionai::v1::ListEventsResponse);
  GRPC_UNARY_MOCK(GetEvent, google::cloud::visionai::v1::GetEventRequest,
                  google::cloud::visionai::v1::Event);
  GRPC_UNARY_MOCK(CreateEvent,
                  google::cloud::visionai::v1::CreateEventRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(UpdateEvent,
                  google::cloud::visionai::v1::UpdateEventRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(DeleteEvent,
                  google::cloud::visionai::v1::DeleteEventRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(ListSeries,
                  google::cloud::visionai::v1::ListSeriesRequest,
                  google::cloud::visionai::v1::ListSeriesResponse);
  GRPC_UNARY_MOCK(GetSeries,
                  google::cloud::visionai::v1::GetSeriesRequest,
                  google::cloud::visionai::v1::Series);
  GRPC_UNARY_MOCK(CreateSeries,
                  google::cloud::visionai::v1::CreateSeriesRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(UpdateSeries,
                  google::cloud::visionai::v1::UpdateSeriesRequest,
                  google::longrunning::Operation);
  GRPC_UNARY_MOCK(DeleteSeries,
                  google::cloud::visionai::v1::DeleteSeriesRequest,
                  google::longrunning::Operation);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_STREAMS_SERVICE_H_
