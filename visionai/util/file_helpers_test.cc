// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/file_helpers.h"

#include <string>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "visionai/util/file_path.h"

namespace visionai {
namespace {

// Test functions to write file, read file, delete and check file existing.
TEST(file_helpers, WriteReadDelete) {
  std::string file_name = file::JoinPath(getenv("HOME"), "test_file");
  std::string content = "test_file content ";

  ASSERT_TRUE(!FileExists(file_name).ok());

  ASSERT_TRUE(SetFileContents(file_name, content).ok());
  ASSERT_TRUE(FileExists(file_name).ok());

  std::string read_result;
  ASSERT_TRUE(GetFileContents(file_name, &read_result).ok());
  EXPECT_EQ(content, read_result);

  ASSERT_TRUE(DeleteFile(file_name).ok());
  ASSERT_TRUE(!FileExists(file_name).ok());
}

// Test append function.
TEST(file_helpers, Append) {
  std::string file_name = file::JoinPath(getenv("HOME"), "test_file");
  std::string content = "test_file content ";
  std::string extra_content = "extra content";

  ASSERT_TRUE(SetFileContents(file_name, content).ok());
  ASSERT_TRUE(SetFileContents(file_name, extra_content,
                              /*should_append=*/true)
                  .ok());
  std::string read_result;
  ASSERT_TRUE(GetFileContents(file_name, &read_result).ok());
  EXPECT_EQ(content + extra_content, read_result);
}

TEST(file_helpers, GetFileSize) {
  std::string file_name = file::JoinPath(getenv("HOME"), "test_size");
  std::string content = "0123456789";
  ASSERT_TRUE(SetFileContents(file_name, content).ok());
  auto file_size = GetFileSize(file_name);
  ASSERT_TRUE(file_size.ok());
  EXPECT_EQ(10, file_size.value());
}

class FileHelpersTest : public ::testing::Test {
 protected:
  void TearDown() override {
    if (rmdir(std::string(test_dir_).c_str()) != EXIT_SUCCESS) {
      LOG(INFO) << "Could not remove test_dir_ here. "
                << "Probably, it was not created in this testcase.";
    }
    if (remove(std::string(test_file_).c_str()) != EXIT_SUCCESS) {
      LOG(INFO) << "Could not remove test_file_ here. "
                << "Probably, it was not created in this testcase.";
    }
  }

  const std::string test_dir_ = file::JoinPath(testing::TempDir(), "test_dir");
  const std::string test_file_ =
      file::JoinPath(testing::TempDir(), "test_file.txt");
  const std::string file_contents_ = "viserving test";
};

TEST_F(FileHelpersTest, ExistsDir) {
  ASSERT_TRUE(CreateDir(test_dir_).ok());
  EXPECT_TRUE(Exists(test_dir_).ok());
}

TEST_F(FileHelpersTest, ExistsFile) {
  ASSERT_TRUE(SetFileContents(test_file_, file_contents_).ok());
  EXPECT_TRUE(Exists(test_file_).ok());
}

TEST_F(FileHelpersTest, ExistsNotFound) {
  EXPECT_THAT(Exists(test_file_).code(), absl::StatusCode::kNotFound);
}

TEST_F(FileHelpersTest, IsDirectorySuccess) {
  ASSERT_TRUE(CreateDir(test_dir_).ok());
  EXPECT_TRUE(IsDirectory(test_dir_).ok());
}

TEST_F(FileHelpersTest, FileIsNotDirectory) {
  ASSERT_TRUE(SetFileContents(test_file_, file_contents_).ok());
  EXPECT_THAT(IsDirectory(test_file_).code(), absl::StatusCode::kInternal);
}

TEST_F(FileHelpersTest, NonExistentIsNotDirectory) {
  EXPECT_THAT(IsDirectory(test_dir_).code(), absl::StatusCode::kNotFound);
}

TEST_F(FileHelpersTest, RecursivelyCreateDirSuccess) {
  // Path is "TempDir/a/b/c".
  const std::string path = file::JoinPath(testing::TempDir(), "a", "b", "c");
  EXPECT_TRUE(RecursivelyCreateDir(path).ok());
  EXPECT_TRUE(IsDirectory(path).ok());
}

TEST_F(FileHelpersTest, RecursivelyCreateDirWithFileInPath) {
  EXPECT_TRUE(SetFileContents(test_file_, file_contents_).ok());
  // Path is "test_file_/a".
  const std::string invalid_path = file::JoinPath(test_file_, "a");
  EXPECT_THAT(RecursivelyCreateDir(invalid_path).code(),
              absl::StatusCode::kInternal);
}

TEST_F(FileHelpersTest, RecursivelyCreateDirInvalidPath) {
  const std::string invalid_path = file::JoinPath("a", "b", "c");
  EXPECT_THAT(RecursivelyCreateDir(invalid_path).code(),
              absl::StatusCode::kInvalidArgument);
}

TEST_F(FileHelpersTest, DeleteFilesInDir) {
  const std::string path = testing::TempDir() + "/file_deletion";
  // Deleting a non-existing dir should return not found error.
  EXPECT_TRUE(absl::IsNotFound(DeleteFilesInDir(path)));

  // Create the dir and the files.
  EXPECT_TRUE(RecursivelyCreateDir(path).ok());

  EXPECT_TRUE(SetFileContents(path + "/1.txt", "1").ok());
  EXPECT_TRUE(FileExists(path + "/1.txt").ok());

  EXPECT_TRUE(SetFileContents(path + "/2.txt", "2").ok());
  EXPECT_TRUE(FileExists(path + "/2.txt").ok());

  EXPECT_TRUE(RecursivelyCreateDir(path + "/nested").ok());
  EXPECT_TRUE(SetFileContents(path + "/nested/2.txt", "2").ok());
  EXPECT_TRUE(FileExists(path + "/nested/2.txt").ok());

  // Delete the files in the dir.
  EXPECT_TRUE(DeleteFilesInDir(path).ok());
  EXPECT_FALSE(FileExists(path + "/1.txt").ok());
  EXPECT_FALSE(FileExists(path + "/2.txt").ok());

  // Nested file won't be deleted.
  EXPECT_TRUE(FileExists(path + "/nested/2.txt").ok());
}

TEST_F(FileHelpersTest, DirectoryNotExist) {
  std::string invalid_path =
      file::JoinPath(testing::TempDir(), "file-helpers/does-not-exist");
  EXPECT_THAT(ValidateLocalDirectory(invalid_path).code(),
              absl::StatusCode::kNotFound);
}

TEST_F(FileHelpersTest, DirectoryStrLengthTooLong) {
  std::string long_string(256, 'a');
  std::string invalid_path =
      file::JoinPath(testing::TempDir(), "file-helpers/", long_string);
  EXPECT_THAT(ValidateLocalDirectory(invalid_path).code(),
              absl::StatusCode::kInvalidArgument);
}

TEST_F(FileHelpersTest, DirectoryStrInvalidChar) {
  std::string invalid_char = "a!b";
  std::string invalid_path =
      file::JoinPath(testing::TempDir(), "file-helpers/", invalid_char);
  EXPECT_THAT(ValidateLocalDirectory(invalid_path).code(),
              absl::StatusCode::kInvalidArgument);
}

TEST_F(FileHelpersTest, DirectoryIsValid) {
  std::string valid_path = file::JoinPath(testing::TempDir(), "file-helpers");
  CreateDir(valid_path).IgnoreError();
  EXPECT_TRUE(ValidateLocalDirectory(valid_path).ok());
}

}  // namespace
}  // namespace visionai
