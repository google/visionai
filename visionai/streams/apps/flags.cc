// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/flags.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_format.h"

ABSL_FLAG(std::string, service_endpoint, "visionai.googleapis.com",
          "The Anaheim service endpoint.");
ABSL_FLAG(std::string, project_id, "", "ID of the GCP project.");
ABSL_FLAG(std::string, location_id, "", "ID of the location.");
ABSL_FLAG(std::string, cluster_id, "", "ID of the cluster.");
ABSL_FLAG(std::string, stream_id, "", "ID of the stream.");
ABSL_FLAG(std::string, event_id, "", "ID of the event.");
