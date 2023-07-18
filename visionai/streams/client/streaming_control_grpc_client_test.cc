// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/streaming_control_grpc_client.h"

// #include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/client/mock_streaming_service.h"
#include "visionai/testing/status/status_matchers.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/time_util.h"

namespace visionai {
namespace testing {
namespace {

using ::google::cloud::visionai::v1::AcquireLeaseRequest;
using ::google::cloud::visionai::v1::Lease;
using ::google::cloud::visionai::v1::ReleaseLeaseRequest;
using ::google::cloud::visionai::v1::ReleaseLeaseResponse;
using ::google::cloud::visionai::v1::RenewLeaseRequest;
using ::testing::_;
using ::testing::DoAll;
using ::testing::EqualsProto;
using ::testing::HasSubstr;
using ::testing::Return;
using ::testing::SetArgPointee;

class StreamingControlGrpcClientTest : public ::testing::Test {
 protected:
  StreamingControlGrpcClientTest()
      : mock_streaming_server_(),
        mock_streaming_service_(mock_streaming_server_.service()),
        local_credentials_address_(
            mock_streaming_server_.local_credentials_server_address()) {}

  MockGrpcServer<MockStreamingService> mock_streaming_server_;
  MockStreamingService* mock_streaming_service_;

  std::string local_credentials_address_;
};

TEST_F(StreamingControlGrpcClientTest, AcquireReadersLease) {
  const std::string channel_id = "foo";
  const std::string lessee = "bar";
  absl::Duration term = absl::Seconds(1);
  Lease lease;
  lease.set_lease_type(google::cloud::visionai::v1::LEASE_TYPE_READER);
  lease.set_id("123");
  lease.set_series(channel_id);
  lease.set_owner(lessee);
  *lease.mutable_expire_time() = ToProtoTimestamp(absl::Now() + term);

  EXPECT_CALL(*mock_streaming_service_,
              AcquireLease(_,
                           EqualsProto<AcquireLeaseRequest>(
                               R"pb(series: "foo",
                                    owner: "bar",
                                    term { seconds: 1 },
                                    lease_type: LEASE_TYPE_READER)pb"),
                           _))
      .Times(2)
      .WillOnce(DoAll(SetArgPointee<2>(lease), Return(grpc::Status::OK)))
      .WillOnce(Return(grpc::Status(grpc::StatusCode::CANCELLED, "foo")));

  StreamingControlGrpcClient::Options options;
  options.target_address = local_credentials_address_;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      true);
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       StreamingControlGrpcClient::Create(options));
  EXPECT_THAT(control_client->AcquireReadersLease(channel_id, lessee, term),
              IsOkAndHolds(EqualsProto(lease)));
  EXPECT_THAT(control_client->AcquireReadersLease(channel_id, lessee, term),
              StatusIs(absl::StatusCode::kCancelled, HasSubstr("foo")));
}

TEST_F(StreamingControlGrpcClientTest, AcquireWritersLease) {
  const std::string channel_id = "foo";
  const std::string lessee = "bar";
  absl::Duration term = absl::Seconds(1);
  Lease lease;
  lease.set_lease_type(google::cloud::visionai::v1::LEASE_TYPE_WRITER);
  lease.set_id("123");
  lease.set_series(channel_id);
  lease.set_owner(lessee);
  *lease.mutable_expire_time() = ToProtoTimestamp(absl::Now() + term);

  EXPECT_CALL(*mock_streaming_service_,
              AcquireLease(_,
                           EqualsProto<AcquireLeaseRequest>(
                               R"pb(series: "foo",
                                    owner: "bar",
                                    term { seconds: 1 },
                                    lease_type: LEASE_TYPE_WRITER)pb"),
                           _))
      .Times(2)
      .WillOnce(DoAll(SetArgPointee<2>(lease), Return(grpc::Status::OK)))
      .WillOnce(Return(grpc::Status(grpc::StatusCode::CANCELLED, "foo")));

  StreamingControlGrpcClient::Options options;
  options.target_address = local_credentials_address_;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      true);
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       StreamingControlGrpcClient::Create(options));
  EXPECT_THAT(control_client->AcquireWritersLease(channel_id, lessee, term),
              IsOkAndHolds(EqualsProto(lease)));
  EXPECT_THAT(control_client->AcquireWritersLease(channel_id, lessee, term),
              StatusIs(absl::StatusCode::kCancelled, HasSubstr("foo")));
}

TEST_F(StreamingControlGrpcClientTest, RenewLease) {
  const std::string lease_id = "123";
  const std::string channel_id = "foo";
  const std::string lessee = "bar";
  absl::Duration term = absl::Seconds(1);
  Lease lease;
  lease.set_lease_type(google::cloud::visionai::v1::LEASE_TYPE_WRITER);
  lease.set_id(lease_id);
  lease.set_series(channel_id);
  lease.set_owner(lessee);
  *lease.mutable_expire_time() = ToProtoTimestamp(absl::Now() + term);

  EXPECT_CALL(*mock_streaming_service_,
              RenewLease(_,
                         EqualsProto<RenewLeaseRequest>(
                             R"pb(id: "123",
                                  series: "foo",
                                  owner: "bar",
                                  term { seconds: 1 })pb"),
                         _))
      .Times(2)
      .WillOnce(DoAll(SetArgPointee<2>(lease), Return(grpc::Status::OK)))
      .WillOnce(Return(grpc::Status(grpc::StatusCode::CANCELLED, "foo")));

  StreamingControlGrpcClient::Options options;
  options.target_address = local_credentials_address_;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      true);
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       StreamingControlGrpcClient::Create(options));
  EXPECT_THAT(control_client->RenewLease(lease_id, channel_id, lessee, term),
              IsOkAndHolds(EqualsProto(lease)));
  EXPECT_THAT(control_client->RenewLease(lease_id, channel_id, lessee, term),
              StatusIs(absl::StatusCode::kCancelled, HasSubstr("foo")));
}

TEST_F(StreamingControlGrpcClientTest, ReleaseLease) {
  const std::string lease_id = "123";
  const std::string channel_id = "foo";
  const std::string lessee = "bar";

  ReleaseLeaseResponse resp;
  EXPECT_CALL(
      *mock_streaming_service_,
      ReleaseLease(_, EqualsProto<ReleaseLeaseRequest>(R"pb(id: "123",
                                                            series: "foo",
                                                            owner: "bar")pb"),
                   _))
      .Times(2)
      .WillOnce(DoAll(SetArgPointee<2>(resp), Return(grpc::Status::OK)))
      .WillOnce(Return(grpc::Status(grpc::StatusCode::CANCELLED, "foo")));

  StreamingControlGrpcClient::Options options;
  options.target_address = local_credentials_address_;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      true);
  VAI_ASSERT_OK_AND_ASSIGN(auto control_client,
                       StreamingControlGrpcClient::Create(options));
  EXPECT_THAT(control_client->ReleaseLease(lease_id, channel_id, lessee),
              IsOkAndHolds(EqualsProto(resp)));
  EXPECT_THAT(control_client->ReleaseLease(lease_id, channel_id, lessee),
              StatusIs(absl::StatusCode::kCancelled, HasSubstr("foo")));
}

}  // namespace

}  // namespace testing
}  // namespace visionai
