// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/net/grpc/auth_token.h"

#include "src/core/lib/security/credentials/jwt/json_token.h"
#include "include/grpc/support/alloc.h"

namespace visionai {

namespace {
// Token should expire in 1 hour.
const gpr_timespec TOKEN_LIFETIME = {3600, 0, GPR_TIMESPAN};
}  // namespace

absl::StatusOr<std::string> GetJwt(const std::string& json_secret,
                                   const std::string& audience) {
  grpc_auth_json_key json_key =
      grpc_auth_json_key_create_from_string(json_secret.c_str());

  if (grpc_auth_json_key_is_valid(&json_key) == 0) {
    // No need to destruct `json_key`, the create function automatically
    // destructs it if it's invalid.
    return absl::InvalidArgumentError("JSON key is invalid.");
  }

  char* token = grpc_jwt_encode_and_sign(&json_key, audience.c_str(),
                                         TOKEN_LIFETIME, nullptr);
  const std::string ret(token);
  grpc_auth_json_key_destruct(&json_key);
  gpr_free(token);
  return ret;
}

}  // namespace visionai
