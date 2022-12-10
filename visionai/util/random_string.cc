// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/random_string.h"

#include <string>

#include "absl/random/random.h"
#include "absl/strings/string_view.h"

namespace visionai {

namespace {
constexpr char kRandomChars[] =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
}  // namespace

std::string RandomString(size_t length) {
  return RandomString(length, kRandomChars);
}

std::string RandomString(size_t length, absl::string_view alphabet) {
  thread_local static absl::BitGen bitgen;
  size_t k = alphabet.size();
  std::string result;
  result.resize(length);
  for (size_t i = 0; i < length; ++i) {
    size_t rand_i = absl::Uniform(bitgen, 0u, k);
    result[i] = alphabet[rand_i];
  }
  return result;
}

}  // namespace visionai
