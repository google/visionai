// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/warehouse/warehouse_grpc_client.h"

#include "google/cloud/visionai/v1alpha1/warehouse.grpc.pb.h"
#include "google/cloud/visionai/v1alpha1/warehouse.pb.h"
#include "google/cloud/visionai/v1alpha1/warehouse.pb.h"
#include "google/cloud/visionai/v1alpha1/warehouse_mock.grpc.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace visionai {
namespace {

using ::google::cloud::visionai::v1alpha1::Annotation;
using ::google::cloud::visionai::v1alpha1::Asset;
using ::google::cloud::visionai::v1alpha1::ClipAssetRequest;
using ::google::cloud::visionai::v1alpha1::ClipAssetResponse;
using ::google::cloud::visionai::v1alpha1::CreateAnnotationRequest;
using ::google::cloud::visionai::v1alpha1::CreateAssetRequest;
using ::google::cloud::visionai::v1alpha1::MockWarehouseStub;
using ::google::cloud::visionai::v1alpha1::Warehouse;
using ::testing::_;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::Return;

TEST(WarehouseGrpcClientTest, SetStubCreateAnnotationTestOk) {
  MockWarehouseStub* mock_stub = new MockWarehouseStub();
  WarehouseGrpcClient<Warehouse> client(60);
  client.SetStub(absl::WrapUnique(mock_stub));
  CreateAnnotationRequest create_annotation_req;
  create_annotation_req.set_parent("test-parent");
  Annotation annotation;
  EXPECT_CALL(*mock_stub, CreateAnnotation(_, _, _))
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_TRUE(client.CreateAnnotation(create_annotation_req, &annotation).ok());
}

// CreateAnnotation tests.
TEST(WarehouseGrpcClientTest, CreateAnnotationTestOk) {
  MockWarehouseStub mock_stub;
  CreateAnnotationRequest create_annotation_req;
  create_annotation_req.set_parent("test-parent");
  Annotation annotation;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_CALL(mock_stub, CreateAnnotation(_, _, _))
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_TRUE(
      client.CreateAnnotation(&mock_stub, create_annotation_req, &annotation)
          .ok());
}

TEST(WarehouseGrpcClientTest, CreateAnnotationTestNullStub) {
  MockWarehouseStub mock_stub;
  CreateAnnotationRequest create_annotation_req;
  create_annotation_req.set_parent("test-parent");
  Annotation annotation;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_THAT(
      client.CreateAnnotation(nullptr, create_annotation_req, &annotation)
          .code(),
      Eq(absl::StatusCode::kInvalidArgument));
  EXPECT_THAT(
      client.CreateAnnotation(nullptr, create_annotation_req, &annotation)
          .message(),
      HasSubstr("stub cannot be null"));
}

TEST(WarehouseGrpcClientTest,
     CreateAnnotationTestEmptyParentReturnsInvalidArg) {
  MockWarehouseStub mock_stub;
  CreateAnnotationRequest create_annotation_req;
  Annotation annotation;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_CALL(mock_stub, CreateAnnotation(_, _, _))
      .WillOnce(Return(grpc::Status::CANCELLED));
  EXPECT_THAT(
      client.CreateAnnotation(&mock_stub, create_annotation_req, &annotation)
          .code(),
      Eq(absl::StatusCode::kCancelled));
}

TEST(WarehouseGrpcClientTest, SetStubCreateAssetTestOk) {
  MockWarehouseStub* mock_stub = new MockWarehouseStub();
  WarehouseGrpcClient<Warehouse> client(60);
  client.SetStub(absl::WrapUnique(mock_stub));
  CreateAssetRequest create_asset_req;
  create_asset_req.set_parent("test-parent");
  Asset asset;
  EXPECT_CALL(*mock_stub, CreateAsset(_, _, _))
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_TRUE(client.CreateAsset(create_asset_req, &asset).ok());
}

TEST(WarehouseGrpcClientTest, CreateAssetTestOk) {
  MockWarehouseStub mock_stub;
  CreateAssetRequest create_asset_req;
  create_asset_req.set_parent("test-parent");
  Asset asset;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_CALL(mock_stub, CreateAsset(_, _, _))
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_TRUE(client.CreateAsset(&mock_stub, create_asset_req, &asset).ok());
}

TEST(WarehouseGrpcClientTest, CreateAssetTestNullStub) {
  MockWarehouseStub mock_stub;
  CreateAssetRequest create_asset_req;
  create_asset_req.set_parent("test-parent");
  Asset asset;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_THAT(client.CreateAsset(nullptr, create_asset_req, &asset).code(),
              Eq(absl::StatusCode::kInvalidArgument));
  EXPECT_THAT(client.CreateAsset(nullptr, create_asset_req, &asset).message(),
              HasSubstr("stub cannot be null"));
}

TEST(WarehouseGrpcClientTest, CreateAssetTestEmptyParentReturnsInvalidArg) {
  MockWarehouseStub mock_stub;
  CreateAssetRequest create_asset_req;
  Asset asset;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_THAT(client.CreateAsset(&mock_stub, create_asset_req, &asset).code(),
              Eq(absl::StatusCode::kInvalidArgument));
}

TEST(WarehouseGrpcClientTest, SetStubClipAssetTestOk) {
  MockWarehouseStub* mock_stub = new MockWarehouseStub();
  WarehouseGrpcClient<Warehouse> client(60);
  client.SetStub(absl::WrapUnique(mock_stub));
  ClipAssetRequest clip_asset_req;
  clip_asset_req.set_name("test-asset");
  ClipAssetResponse resp;
  EXPECT_CALL(*mock_stub, ClipAsset(_, _, _))
      .WillOnce(Return(grpc::Status::OK));
  EXPECT_TRUE(client.ClipAsset(clip_asset_req, &resp).ok());
}

TEST(WarehouseGrpcClientTest, ClipAssetTestOk) {
  MockWarehouseStub mock_stub;
  ClipAssetRequest clip_asset_req;
  clip_asset_req.set_name("test-asset");
  ClipAssetResponse resp;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_CALL(mock_stub, ClipAsset(_, _, _)).WillOnce(Return(grpc::Status::OK));
  EXPECT_TRUE(client.ClipAsset(&mock_stub, clip_asset_req, &resp).ok());
}

TEST(WarehouseGrpcClientTest, ClipAssetTestNullStub) {
  MockWarehouseStub mock_stub;
  ClipAssetRequest clip_asset_req;
  clip_asset_req.set_name("test-asset");
  ClipAssetResponse resp;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_THAT(client.ClipAsset(nullptr, clip_asset_req, &resp).code(),
              Eq(absl::StatusCode::kInvalidArgument));
  EXPECT_THAT(client.ClipAsset(nullptr, clip_asset_req, &resp).message(),
              HasSubstr("stub cannot be null"));
}

TEST(WarehouseGrpcClientTest, ClipAssetTestEmptyNameReturnsInvalidArg) {
  MockWarehouseStub mock_stub;
  ClipAssetRequest clip_asset_req;
  ClipAssetResponse resp;
  WarehouseGrpcClient<Warehouse> client(60);
  EXPECT_THAT(client.ClipAsset(&mock_stub, clip_asset_req, &resp).code(),
              Eq(absl::StatusCode::kInvalidArgument));
}

}  // namespace

}  // namespace visionai
