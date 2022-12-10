/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_FLAGS_H_
#define THIRD_PARTY_VISIONAI_UTIL_FLAGS_H_

namespace visionai {

// VisionAI specifict init function for adapting binaries built from google3
// running on GCP.
void VisionAIInit();

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_FLAGS_H_
