// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

 #include "visionai/types/signal.h"

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"

namespace visionai {

absl::StatusOr<std::string> ToString(Signal::SignalCode code) {
  switch (code) {
    case Signal::SignalCode::kUnknown:
      return "unknown";
    case Signal::SignalCode::kPhantom:
      return "phantom";
    default:
      return absl::UnimplementedError(absl::StrFormat(
          "Signal code %d does not have a string representation.", code));
  }
}

absl::StatusOr<Signal::SignalCode> ToSignalCode(const std::string &s) {
  if (s == "phantom") {
    return Signal::SignalCode::kPhantom;
  } else if (s == "unknown") {
    return Signal::SignalCode::kUnknown;
  } else {
    return absl::InvalidArgumentError(
        absl::StrFormat("Given an unrecognized signal code string \"%s\"", s));
  }
}

Signal::Signal() : Signal(SignalCode::kUnknown) {}

Signal::Signal(SignalCode code) : code_(code) {}

Signal::SignalCode Signal::code() const { return code_; }

}  // namespace visionai
