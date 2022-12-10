// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/gtl/container_logging.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/types/span.h"

namespace visionai {
namespace gtl {

class ContainerLoggingTest : public ::testing::Test {
 protected:
  ContainerLoggingTest() : stream_(new std::stringstream) {}
  std::ostream& stream() { return *stream_; }
  std::string logged() {
    std::string r = stream_->str();
    stream_.reset(new std::stringstream);
    return r;
  }

 private:
  std::unique_ptr<std::stringstream> stream_;
};

TEST_F(ContainerLoggingTest, ShortRange) {
  std::vector<std::string> words = {"hi", "hello"};
  gtl::LogRangeToStream(stream(), words.begin(), words.end(),
                        gtl::LogMultiline());
  EXPECT_EQ("[\nhi\nhello\n]", logged());
}

TEST_F(ContainerLoggingTest, LegacyRange) {
  std::vector<int> lengths = {1, 2};
  gtl::LogRangeToStream(stream(), lengths.begin(), lengths.end(),
                        gtl::LogLegacyUpTo100());
  EXPECT_EQ("1 2", logged());
}

TEST_F(ContainerLoggingTest, ToString) {
  std::vector<int> lengths = {1, 2, 3, 4, 5};
  EXPECT_EQ(gtl::LogContainer(lengths).str(), "[1, 2, 3, 4, 5]");
}

class UserDefFriend {
 public:
  explicit UserDefFriend(int i) : i_(i) {}

 private:
  friend std::ostream& operator<<(std::ostream& str, const UserDefFriend& i) {
    return str << i.i_;
  }
  int i_;
};

TEST_F(ContainerLoggingTest, RangeOfUserDefined) {
  std::vector<UserDefFriend> ints = {UserDefFriend(1), UserDefFriend(2),
                                     UserDefFriend(3)};
  gtl::LogRangeToStream(stream(), ints.begin(), ints.begin() + 1,
                        gtl::LogDefault());
  gtl::LogRangeToStream(stream(), ints.begin() + 1, ints.begin() + 2,
                        gtl::LogMultiline());
  gtl::LogRangeToStream(stream(), ints.begin() + 2, ints.begin() + 3,
                        gtl::LogDefault());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.begin(),
                        gtl::LogMultiline());

  EXPECT_EQ("[1][\n2\n][3][\n]", logged());
}

TEST_F(ContainerLoggingTest, FullContainer) {
  std::vector<int> ints;
  std::vector<int> ints100;
  std::vector<int> ints123;
  int64 max_elements = 123;
  std::string expected1;
  std::string expected2;
  std::string expected3;
  std::string expected4;
  std::string expected5;
  std::string expected6;
  std::string expected7;
  std::string expected8;
  std::string expected9;
  for (int i = 0; i < 1000; ++i) {
    ints.push_back(i);
    if (i < 100) {
      ints100.push_back(i);
    }
    if (i < max_elements) {
      ints123.push_back(i);
    }
  }
  expected1 = "[\n" + absl::StrJoin(ints, "\n") + "\n]";
  expected2 = "[" + absl::StrJoin(ints, ", ") + "]";
  expected3 = "[\n" + absl::StrJoin(ints100, "\n") + "\n...\n]";
  expected4 = "[" + absl::StrJoin(ints100, ", ") + ", ...]";
  expected5 = absl::StrJoin(ints100, " ") + " ...";
  expected6 = "[\n" + absl::StrJoin(ints, "\n") + "\n]";
  expected7 = "[\n" + absl::StrJoin(ints123, "\n") + "\n...\n]";
  expected8 = "[" + absl::StrJoin(ints, ", ") + "]";
  expected9 = "[" + absl::StrJoin(ints123, ", ") + ", ...]";

  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogMultiline());
  EXPECT_EQ(expected1, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(), gtl::LogShort());
  EXPECT_EQ(expected2, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogMultilineUpTo100());
  EXPECT_EQ(expected3, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogShortUpTo100());
  EXPECT_EQ(expected4, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogLegacyUpTo100());
  EXPECT_EQ(expected5, logged());

  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogMultilineUpToN(ints.size()));
  EXPECT_EQ(expected6, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogMultilineUpToN(max_elements));
  EXPECT_EQ(expected7, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogShortUpToN(ints.size()));
  EXPECT_EQ(expected8, logged());
  gtl::LogRangeToStream(stream(), ints.begin(), ints.end(),
                        gtl::LogShortUpToN(max_elements));
  EXPECT_EQ(expected9, logged());
}

TEST_F(ContainerLoggingTest, LogContainer) {
  std::set<int> ints = {1, 2, 3};
  stream() << gtl::LogContainer(ints, gtl::LogMultiline());
  EXPECT_EQ("[\n1\n2\n3\n]", logged());

  stream() << gtl::LogContainer(ints);
  EXPECT_EQ("[1, 2, 3]", logged());

  stream() << gtl::LogContainer(std::vector<int>(ints.begin(), ints.end()),
                                gtl::LogLegacyUpTo100());
  EXPECT_EQ("1 2 3", logged());
}

TEST_F(ContainerLoggingTest, LogMutableSpan) {
  std::vector<int> ints = {1, 2, 3};
  absl::Span<int> int_span(ints);
  stream() << gtl::LogContainer(int_span);
  EXPECT_EQ("[1, 2, 3]", logged());
}

TEST_F(ContainerLoggingTest, LogRange) {
  std::set<int> ints = {1, 2, 3};
  stream() << gtl::LogRange(ints.begin(), ints.end(), gtl::LogMultiline());
  EXPECT_EQ("[\n1\n2\n3\n]", logged());

  stream() << gtl::LogRange(ints.begin(), ints.end());
  EXPECT_EQ("[1, 2, 3]", logged());
}

class LogEnumTest : public ContainerLoggingTest {
 protected:
  enum Unscoped { kUnscoped0, kUnscoped1, kUnscoped2 };

  enum StreamableUnscoped {
    kStreamableUnscoped0,
    kStreamableUnscoped1,
    kStreamableUnscoped2
  };

  enum class Scoped { k0, k1, k2 };

  enum class StreamableScoped { k0, k1, k2 };

  friend std::ostream& operator<<(std::ostream& os, StreamableUnscoped v) {
    return os << gtl::LogEnum(v);
  }

  friend std::ostream& operator<<(std::ostream& os, StreamableScoped v) {
    return os << gtl::LogEnum(v);
  }
};

TEST_F(LogEnumTest, Unscoped) {
  stream() << gtl::LogEnum(kUnscoped0) << "," << gtl::LogEnum(kUnscoped1) << ","
           << gtl::LogEnum(kUnscoped2);
  EXPECT_EQ("0,1,2", logged());
}

TEST_F(LogEnumTest, StreamableUnscoped) {
  stream() << kStreamableUnscoped0 << "," << kStreamableUnscoped1 << ","
           << kStreamableUnscoped2;
  EXPECT_EQ("0,1,2", logged());
}

TEST_F(LogEnumTest, Scoped) {
  stream() << gtl::LogEnum(Scoped::k0) << "," << gtl::LogEnum(Scoped::k1) << ","
           << gtl::LogEnum(Scoped::k2);
  EXPECT_EQ("0,1,2", logged());
}

TEST_F(LogEnumTest, StreamableScoped) {
  // Test using LogEnum to implement an operator<<.
  stream() << StreamableScoped::k0 << "," << StreamableScoped::k1 << ","
           << StreamableScoped::k2;
  EXPECT_EQ("0,1,2", logged());
}

}  // namespace gtl
}  // namespace visionai
