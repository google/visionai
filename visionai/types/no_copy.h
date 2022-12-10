/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_TYPES_NO_COPY_H_
#define THIRD_PARTY_VISIONAI_TYPES_NO_COPY_H_

#include <string>

#include "absl/strings/string_view.h"

namespace visionai {

// A simple data structure which does not allow copies, useful for testing
// whether certain operations are efficiently moving objects.
struct NoCopy {
  explicit NoCopy(absl::string_view data) : data_(data) {}
  NoCopy() { data_ = "hello world"; }
  NoCopy(NoCopy&&) = default;
  NoCopy(const NoCopy&) = delete;

  NoCopy& operator=(NoCopy&&) = default;
  NoCopy& operator=(const NoCopy&) = delete;

  std::string data_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TYPES_NO_COPY_H_
