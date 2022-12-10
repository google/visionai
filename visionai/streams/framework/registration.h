// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_REGISTRATION_H_
#define THIRD_PARTY_VISIONAI_STREAMS_REGISTRATION_H_

#include <utility>

namespace visionai {

// InitOnStartupMarker is an object meant to be used at global static scope so
// that it is initialized on program startup. Its initialization is really used
// to trigger side effects; e.g. register plugins to a global static registry.
//
// You may dial in side effects by using the output operator like so:
// ```
// // Some function that contains side effects that returns a
// // InitOnStartupMarker.
// InitOnStartupMarker SomeSideEffect() { ...; }
// InitOnStartupMarker const marker = InitOnStartupMarker{} << SomeSideEffect();
//
// // Or a callable return an InitOnStartupMarker.
// InitOnStartupMarker const marker =
//   InitOnStartupMarker{} <<
//     [] () { AnotherSideEffect(); return InitOnStartupMarker{}; }
struct InitOnStartupMarker {
  constexpr InitOnStartupMarker operator<<(InitOnStartupMarker) const {
    return *this;
  }
  template <typename T>
  constexpr InitOnStartupMarker operator<<(T&& v) const {
    return std::forward<T>(v)();
  }
};

// Wrapper for generating unique IDs (for 'anonymous' InitOnStartup definitions)
// using __COUNTER__. The new ID (__COUNTER__ already expanded) is provided as a
// macro argument.
//
// Usage:
//   #define M_IMPL(id, a, b) ...
//   #define M(a, b) VAI_NEW_ID_FOR_INIT(M_IMPL, a, b)
#define VAI_NEW_ID_FOR_INIT_2(m, c, ...) m(c, __VA_ARGS__)
#define VAI_NEW_ID_FOR_INIT_1(m, c, ...) \
  VAI_NEW_ID_FOR_INIT_2(m, c, __VA_ARGS__)
#define VAI_NEW_ID_FOR_INIT(m, ...) \
  VAI_NEW_ID_FOR_INIT_1(m, __COUNTER__, __VA_ARGS__)

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_REGISTRATION_H_
