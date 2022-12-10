// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_FLAGS_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_FLAGS_H_

#include <string>

#include "absl/flags/declare.h"

ABSL_DECLARE_FLAG(std::string, service_endpoint);
ABSL_DECLARE_FLAG(std::string, project_id);
ABSL_DECLARE_FLAG(std::string, location_id);
ABSL_DECLARE_FLAG(std::string, cluster_id);
ABSL_DECLARE_FLAG(std::string, stream_id);
ABSL_DECLARE_FLAG(std::string, event_id);

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_FLAGS_H_
