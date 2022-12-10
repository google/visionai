/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_FILE_PATH_H_
#define VISIONAI_UTIL_FILE_PATH_H_

#include <initializer_list>
#include <string>

#include "absl/strings/string_view.h"

// A set of file pathname manipulation routines.
namespace visionai {
namespace file {
namespace internal {

// Not part of the public API.
std::string JoinPathImpl(bool honor_abs,
                         std::initializer_list<absl::string_view> paths);

}  // namespace internal

// Join multiple paths together.
// JoinPath and JoinPathRespectAbsolute have slightly different semantics.
// JoinPath unconditionally joins all paths together, whereas
// JoinPathRespectAbsolute ignores any segments prior to the last absolute
// path.  For example:
//
//  Arguments                  | JoinPath            | JoinPathRespectAbsolute
//  ---------------------------+---------------------+-----------------------
//  '/foo', 'bar'              | /foo/bar            | /foo/bar
//  '/foo/', 'bar'             | /foo/bar            | /foo/bar
//  '/foo', '/bar'             | /foo/bar            | /bar
//  '/foo', '/bar', '/baz'     | /foo/bar/baz        | /baz
//
// All paths will be treated as relative paths, regardless of whether or not
// they start with a leading '/'.  That is, all paths will be concatenated
// together, with the appropriate path separator inserted in between.
// Arguments must be convertible to absl::string_view.
//
// Usage:
// std::string path = file::JoinPath("/cns", dirname, filename);
// std::string path = file::JoinPath("./", filename);
//
// 0, 1, 2-path specializations exist to optimize common cases.
inline std::string JoinPath() { return std::string(); }
inline std::string JoinPath(absl::string_view path) {
  return std::string(path.data(), path.size());
}
std::string JoinPath(absl::string_view path1, absl::string_view path2);
template <typename... T>
inline std::string JoinPath(absl::string_view path1, absl::string_view path2,
                            absl::string_view path3, const T&... args) {
  return internal::JoinPathImpl(false, {path1, path2, path3, args...});
}

// Returns the part of the path before the final "/", EXCEPT:
// * If there is a single leading "/" in the path, the result will be the
//   leading "/".
// * If there is no "/" in the path, the result is the empty prefix of the
//   input std::string.
absl::string_view Dirname(absl::string_view path);

// Return the parts of the path, split on the final "/".  If there is no
// "/" in the path, the first part of the output is empty and the second
// is the input. If the only "/" in the path is the first character, it is
// the first part of the output.
std::pair<absl::string_view, absl::string_view> SplitPath(
    absl::string_view path);

// Returns the part of the path after the final "/".  If there is no
// "/" in the path, the result is the same as the input.
// Note that this function's behavior differs from the Unix basename
// command if path ends with "/". For such paths, this function returns the
// empty std::string.
absl::string_view Basename(absl::string_view path);

// Returns the part of the basename of path after the final ".".  If
// there is no "." in the basename, the result is empty.
absl::string_view Extension(absl::string_view path);

}  // namespace file
}  // namespace visionai

#endif  // VISIONAI_UTIL_FILE_PATH_H_
