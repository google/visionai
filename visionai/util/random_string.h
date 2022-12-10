// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_UTIL_RANDOM_STRING_H_
#define VISIONAI_UTIL_RANDOM_STRING_H_

#include <string>

#include "absl/strings/string_view.h"

namespace visionai {

// Generate a random string of length `length`.
//
// The returned string contains only alphanumeric charactecters.
std::string RandomString(size_t length);

// Generate a random string of length `length`.
//
// The returned string contains only alphanumeric charactecters.
std::string RandomString(size_t length, absl::string_view alphabet);

}  // namespace visionai

#endif  // VISIONAI_UTIL_RANDOM_STRING_H_
