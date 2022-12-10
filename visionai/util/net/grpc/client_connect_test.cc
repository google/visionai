// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/net/grpc/client_connect.h"

#include <cstdlib>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace visionai {
namespace {

TEST(ClientConnectTest, SetAuthorizationHeaderFromJsonKey) {
  {
    setenv("VISIONAI_OAUTH_TOKEN", "foo", 1);
    ConnectionOptions options;
    EXPECT_EQ(
        SetAuthorizationHeaderFromJsonKey("visionai.googleapis.com", options)
            .ok(),
        true);
    unsetenv("VISIONAI_OAUTH_TOKEN");
    EXPECT_THAT((*options.mutable_client_context_options()
                      ->mutable_metadata())["authorization"],
                testing::HasSubstr("foo"));
  }
  {
    ConnectionOptions options;
    options.mutable_client_context_options()->mutable_metadata()->insert(
        {"authorization", "bar"});

    EXPECT_EQ(
        SetAuthorizationHeaderFromJsonKey("visionai.googleapis.com", options)
            .ok(),
        true);
    EXPECT_THAT((*options.mutable_client_context_options()
                      ->mutable_metadata())["authorization"],
                testing::HasSubstr("bar"));
  }
}

}  // namespace
}  // namespace visionai
