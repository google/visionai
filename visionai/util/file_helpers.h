// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_UTIL_FILE_HELPERS_H_
#define VISIONAI_UTIL_FILE_HELPERS_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"

namespace visionai {

// Reads the contents of the file `file_name` into `output`.
//
// REQUIRES: `output` is non-null.
absl::Status GetFileContents(absl::string_view file_name, std::string* output);

// Writes the data provided in `content` to the file `file_name`. It overwrites
// any existing content unless 'should_append' is true.
absl::Status SetFileContents(absl::string_view file_name,
                             absl::string_view content,
                             bool should_append = false);

// Decides if the named path `file_name` exists.
absl::Status FileExists(absl::string_view file_name);

// Deletes file in path `file_name`.
absl::Status DeleteFile(absl::string_view file_name);

// Get the size of the file in bytes.
absl::StatusOr<int64_t> GetFileSize(absl::string_view file_path);

absl::Status CreateDir(absl::string_view path);

absl::Status Exists(absl::string_view path);

absl::Status IsDirectory(absl::string_view path);

absl::Status RecursivelyCreateDir(absl::string_view path);

// Deletes all the files in a directory. Does not delete the sub-directories.
//
// Possible return status codes:
// + NOT_FOUND: failure to open the directory.
// + INTERNAL: failure to read or close the directory.
// + OK: failure to delete the nested sub-directory only logs warning messages.
absl::Status DeleteFilesInDir(const std::string& path);

// Validates the directory path. If the input path exceeds the max length
// or contains invalid characters, will throw `InvalidArgumentError`.
// If the directory path does not exist, will throw `NotFoundError`.
absl::Status ValidateLocalDirectory(const std::string& path);

}  // namespace visionai

#endif  // VISIONAI_UTIL_FILE_HELPERS_H_
