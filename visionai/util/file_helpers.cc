// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/file_helpers.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <cerrno>
#include <string>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/substitute.h"
#include "re2/re2.h"
#include "visionai/util/file_path.h"

namespace visionai {
namespace {
constexpr inline int KMaxLocalDirLength = 256;
}  // namespace

absl::Status GetFileContents(absl::string_view file_name, std::string* output) {
  FILE* fp = fopen(file_name.data(), "r");
  if (fp == nullptr) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Can't find file: %s", file_name));
  }

  output->clear();
  while (!feof(fp)) {
    char buf[4096];
    size_t ret = fread(buf, 1, 4096, fp);
    if (ret == 0 && ferror(fp)) {
      return absl::InternalError(
          absl::StrFormat("Error while reading file: %s", file_name));
    }
    output->append(std::string(buf, ret));
  }
  fclose(fp);
  return absl::OkStatus();
}

absl::Status SetFileContents(absl::string_view file_name,
                             absl::string_view content, bool should_append) {
  FILE* fp = fopen(file_name.data(), should_append ? "a" : "w");
  if (fp == nullptr) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Can't open file: %s. \n fopen errno:\n%s\n", file_name,
                        strerror(errno)));
  }

  size_t written = fwrite(content.data(), sizeof(char), content.size(), fp);
  if (written != content.size()) {
    return absl::InternalError(
        absl::StrFormat("Error while fwrite file: %s", file_name));
  }
  if (fclose(fp)) {
    return absl::InternalError(
        absl::StrFormat("Error while fclose file: %s. \n fclose errno:\n%s\n",
                        file_name, strerror(errno)));
  }
  return absl::OkStatus();
}

absl::Status FileExists(absl::string_view file_name) {
  struct stat buffer;
  int status;
  status = stat(file_name.data(), &buffer);
  if (status == 0) {
    return absl::OkStatus();
  }
  switch (errno) {
    case EACCES:
      return absl::PermissionDeniedError("Insufficient permissions.");
    default:
      return absl::NotFoundError("The path does not exist.");
  }
}

absl::Status DeleteFile(absl::string_view file_name) {
  if (remove(file_name.data()) != 0) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Can't delete file: %s. \n remove errno:\n%s\n",
                        file_name, strerror(errno)));
  }
  return absl::OkStatus();
}

absl::StatusOr<int64_t> GetFileSize(absl::string_view file_path) {
  struct stat buf;
  if (stat(file_path.data(), &buf) == 0) {
    return buf.st_size;
  }
  return absl::UnknownError(
      absl::StrFormat("Could not stat file: %s.", file_path));
}

absl::Status Exists(absl::string_view path) {
  struct stat path_stat;
  if (stat(std::string(path).c_str(), &path_stat) != EXIT_SUCCESS) {
    return absl::NotFoundError("");
  }
  return absl::OkStatus();
}

absl::Status IsDirectory(absl::string_view path) {
  struct stat path_stat;
  if (stat(std::string(path).c_str(), &path_stat) != EXIT_SUCCESS) {
    return absl::NotFoundError(
        absl::Substitute("Path '$0' does not exist.", path));
  }
  if (!S_ISDIR(path_stat.st_mode)) {
    return absl::InternalError(absl::Substitute(
        "Path '$0' exists, but does not refer to directory.", path));
  }
  return absl::OkStatus();
}

absl::Status CreateDir(absl::string_view path) {
  if (mkdir(std::string(path).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
    switch (errno) {
      case EACCES:
        return absl::PermissionDeniedError("Insufficient permissions.");
      default:
        return absl::UnavailableError("Failed to create directory.");
    }
  }
  return absl::OkStatus();
}

absl::Status RecursivelyCreateDir(absl::string_view path) {
  if (Exists(path).ok()) {
    return IsDirectory(path);
  }
  std::pair<absl::string_view, absl::string_view> split_path =
      file::SplitPath(path);
  if (split_path.first == path) {
    return absl::InvalidArgumentError(
        "Failed to parse path. Maybe no directory in the path exists?");
  }
  const absl::Status status = RecursivelyCreateDir(split_path.first);
  if (!status.ok()) {
    return status;
  }
  return CreateDir(path);
}

absl::Status DeleteFilesInDir(const std::string& path) {
  DIR* d = opendir(path.c_str());
  if (d == nullptr) {
    return absl::NotFoundError(absl::StrFormat("Can't open dir: %s. errno: %s",
                                               path, strerror(errno)));
  }

  absl::Status status;
  struct dirent* dir = NULL;

  while (true) {
    errno = 0;
    if ((dir = readdir(d)) == NULL) {
      if (errno != 0) {
        status = absl::InternalError(
            absl::StrFormat("readdir errno: %s", strerror(errno)));
      }
      break;
    }
    if ((!strcmp(dir->d_name, ".")) || (!strcmp(dir->d_name, ".."))) {
      continue;
    }
    std::string fpath = file::JoinPath(path, dir->d_name);
    if (remove(fpath.c_str()) != 0) {
      LOG(WARNING) << absl::StrFormat("Can't delete file: %s. remove errno: %s",
                                      fpath, strerror(errno));
    }
  }
  if (closedir(d) != 0) {
    status = absl::InternalError(absl::StrFormat(
        "Can't close dir: %s. errno: %s", path, strerror(errno)));
  }

  return status;
}

absl::Status ValidateLocalDirectory(const std::string& path) {
  if (path.size() > KMaxLocalDirLength) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "The local directory (%s) has exceeded the max local dir length %d",
        path, KMaxLocalDirLength));
  }
  std::string invalid_dir_chars("[^a-zA-Z0-9-_\\/]");
  RE2 pattern(invalid_dir_chars);
  if (RE2::PartialMatch(path, pattern)) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Found invalid characters in the local directory path "
                        "(%s). The allowed "
                        "characters are numbers, alphabets, `-`, `_`, and `/`.",
                        path));
  }
  return IsDirectory(path);
}

}  // namespace visionai
