/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_HEALTH_SERVICE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_HEALTH_SERVICE_H_

#include "google/cloud/visionai/v1/health_service.grpc.pb.h"
#include "google/cloud/visionai/v1/health_service.pb.h"
#include "visionai/testing/grpc/mock_grpc.h"


namespace visionai {
class MockHealthCheckService
    : public ::google::cloud::visionai::v1::HealthCheckService::Service {
 public:
  using ServiceType = ::google::cloud::visionai::v1::HealthCheckService;
  GRPC_UNARY_MOCK(HealthCheck,
                 ::google::cloud::visionai::v1::HealthCheckRequest,
                 ::google::cloud::visionai::v1::HealthCheckResponse);
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_MOCK_HEALTH_SERVICE_H_
