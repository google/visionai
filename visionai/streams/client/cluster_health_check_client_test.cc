// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/cluster_health_check_client.h"
#include <string>

#include "google/cloud/visionai/v1/health_service.pb.h"
#include "gmock/gmock.h"
#include "google_specific/include/grpcpp/support/status.h"
#include "visionai/streams/client/mock_health_service.h"
#include "gtest/gtest.h"
#include "visionai/testing/grpc/mock_grpc.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/testing/status/status_matchers.h"

namespace visionai {
namespace testing{
namespace {

using ::google::cloud::visionai::v1::HealthCheckRequest;
using ::google::cloud::visionai::v1::HealthCheckResponse;
using ::testing::_;
using ::testing::DoAll;
using ::testing::EqualsProto;
using ::testing::Return;
using ::testing::SetArgPointee;

class ClusterHealthCheckClientTest : public ::testing::Test {
 protected:
  ClusterHealthCheckClientTest()
      : mock_health_server_(),
        mock_health_service_(mock_health_server_.service()),
        local_credentials_address_(
          mock_health_server_.local_credentials_server_address()) {}

  MockGrpcServer<MockHealthCheckService> mock_health_server_;
  MockHealthCheckService* mock_health_service_;
  std::string local_credentials_address_;
};

TEST_F(ClusterHealthCheckClientTest, HealthCheck) {
  HealthCheckResponse resp;
  resp.set_healthy(true);
  EXPECT_CALL(*mock_health_service_,
             HealthCheck(_, EqualsProto<HealthCheckRequest>(R"pb(
               cluster: "cluster-name")pb"), _))
             .Times(1)
             .WillOnce(DoAll(SetArgPointee<2>(resp),
                       Return(grpc::Status::OK)));

  ClusterHealthCheckClient::Options options;
  options.target_address = local_credentials_address_;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      true);
  VAI_ASSERT_OK_AND_ASSIGN(auto client, ClusterHealthCheckClient::Create(options));
  EXPECT_THAT(client->CheckClusterHealth("cluster-name"),
              IsOkAndHolds(EqualsProto(resp)));
}

}  // namespace
}  // namespace testing
}  // namespace visionai
