/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_EXAMINE_STACK_H_
#define VISIONAI_UTIL_EXAMINE_STACK_H_

#include <string>

namespace visionai {

// Type of function used for printing in stack trace dumping, etc.
// We avoid closures to keep things simple.
typedef void DebugWriter(const char*, void*);

// Dump current stack trace omitting the topmost 'skip_count' stack frames.
void DumpStackTrace(int skip_count, DebugWriter* w, void* arg,
                    bool short_format = false);

// Dump given pc and stack trace.
void DumpPCAndStackTrace(void* pc, void* const stack[], int depth,
                         DebugWriter* writerfn, void* arg,
                         bool short_format = false);

// Return the current stack trace as a string (on multiple lines, beginning with
// "Stack trace:\n").
std::string CurrentStackTrace(bool short_format = false);

}  // namespace visionai

#endif  // VISIONAI_UTIL_EXAMINE_STACK_H_
