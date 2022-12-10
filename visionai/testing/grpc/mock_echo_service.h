/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_TESTING_GRPC_MOCK_ECHO_SERVICE_H_
#define THIRD_PARTY_VISIONAI_TESTING_GRPC_MOCK_ECHO_SERVICE_H_

#include "gmock/gmock.h"
#include "visionai/testing/grpc/echo.grpc.pb.h"
#include "visionai/testing/grpc/mock_grpc.h"

namespace visionai {
namespace testing {

class MockEchoService : public EchoService::Service {
 public:
  using ServiceType = EchoService;
  GRPC_UNARY_MOCK(Echo, ::visionai::testing::EchoRequest,
                  ::visionai::testing::EchoResponse);
  GRPC_CLIENT_STREAMING_MOCK(ClientStreamingEcho,
                             ::visionai::testing::EchoRequest,
                             ::visionai::testing::EchoResponse);
  GRPC_SERVER_STREAMING_MOCK(ServerStreamingEcho,
                             ::visionai::testing::EchoRequest,
                             ::visionai::testing::EchoResponse);
  GRPC_BIDI_STREAMING_MOCK(BidiStreamingEcho, ::visionai::testing::EchoRequest,
                           ::visionai::testing::EchoResponse);
};

class MockFooService : public FooService::Service {
 public:
  using ServiceType = FooService;
  GRPC_UNARY_MOCK(Foo, ::visionai::testing::FooRequest,
                  ::visionai::testing::FooResponse);
};

}  // namespace testing
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TESTING_GRPC_MOCK_ECHO_SERVICE_H_
