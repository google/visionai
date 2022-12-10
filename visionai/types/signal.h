// Copyright 2020 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_TYPES_SIGNAL_H_
#define THIRD_PARTY_VISIONAI_TYPES_SIGNAL_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace visionai {

// A class to represent in-band signals.
class Signal {
 public:
  // All possible signal types.
  enum class SignalCode : int {
    kUnknown = 0,
    kPhantom,
  };

  // The default constructor constructs an unknown signal.
  Signal();

  // Construct a signal with code `code`.
  explicit Signal(SignalCode code);

  // Returns the code of this signal.
  SignalCode code() const;

 private:
  SignalCode code_;
};

// Returns a string representation of the signal code.
absl::StatusOr<std::string> ToString(Signal::SignalCode code);

// Returns the signal code from a string.
absl::StatusOr<Signal::SignalCode> ToSignalCode(const std::string &s);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TYPES_SIGNAL_H_
