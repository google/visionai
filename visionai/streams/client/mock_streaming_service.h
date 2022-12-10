/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_STREAMING_SERVICE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_STREAMING_SERVICE_H_

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "google/cloud/visionai/v1/streaming_service.grpc.pb.h"
#include "gmock/gmock.h"
#include "visionai/testing/grpc/mock_grpc.h"

namespace visionai {
class MockStreamingService
    : public ::google::cloud::visionai::v1::StreamingService::Service {
 public:
  using ServiceType = ::google::cloud::visionai::v1::StreamingService;
  GRPC_BIDI_STREAMING_MOCK(
      SendPackets, ::google::cloud::visionai::v1::SendPacketsRequest,
      ::google::cloud::visionai::v1::SendPacketsResponse);
  GRPC_BIDI_STREAMING_MOCK(
      ReceivePackets,
      ::google::cloud::visionai::v1::ReceivePacketsRequest,
      ::google::cloud::visionai::v1::ReceivePacketsResponse);
  GRPC_BIDI_STREAMING_MOCK(
      ReceiveEvents, ::google::cloud::visionai::v1::ReceiveEventsRequest,
      ::google::cloud::visionai::v1::ReceiveEventsResponse);
  GRPC_UNARY_MOCK(AcquireLease,
                  ::google::cloud::visionai::v1::AcquireLeaseRequest,
                  ::google::cloud::visionai::v1::Lease);
  GRPC_UNARY_MOCK(RenewLease,
                  ::google::cloud::visionai::v1::RenewLeaseRequest,
                  ::google::cloud::visionai::v1::Lease);
  GRPC_UNARY_MOCK(ReleaseLease,
                  ::google::cloud::visionai::v1::ReleaseLeaseRequest,
                  ::google::cloud::visionai::v1::ReleaseLeaseResponse);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_STREAMING_SERVICE_H_
