// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/tuple/components/iterate.h"

#include <stddef.h>

#include <string>
#include <tuple>
#include <utility>

#include "gtest/gtest.h"
#include "absl/strings/str_cat.h"
#include "visionai/util/tuple/components/std_tuple.h"
#include "visionai/util/tuple/test_util.h"

namespace visionai {
namespace {

using ::std::make_tuple;
using ::std::tuple;

struct AppendReturnIndex {
  template <::size_t I>
  ::std::string operator()(::std::string state) const {
    absl::StrAppend(&state, I);
    return state;
  }
};

class IterateIndex : public TestValues {};

TEST_F(IterateIndex, ReturnByValue) {
  EXPECT_EQ("N", iterate_index<0>(AppendReturnIndex(), "N"));
  EXPECT_EQ("N0", iterate_index<1>(AppendReturnIndex(), "N"));
  EXPECT_EQ("N01", iterate_index<2>(AppendReturnIndex(), "N"));
  EXPECT_EQ("N012345678", iterate_index<9>(AppendReturnIndex(), "N"));
  EXPECT_EQ("N0123456789", iterate_index<10>(AppendReturnIndex(), "N"));
  EXPECT_EQ("N012345678910", iterate_index<11>(AppendReturnIndex(), "N"));
}

struct IdentityIndex {
  template <::size_t I, class T>
  const T& operator()(const T& state) const {
    return state;
  }
};

TEST_F(IterateIndex, ReturnByReference) {
  int n = 0;
  EXPECT_EQ(&n, &iterate_index<0>(IdentityIndex(), n));
  EXPECT_EQ(&n, &iterate_index<1>(IdentityIndex(), n));
  EXPECT_EQ(&n, &iterate_index<2>(IdentityIndex(), n));
  EXPECT_EQ(&n, &iterate_index<9>(IdentityIndex(), n));
  EXPECT_EQ(&n, &iterate_index<10>(IdentityIndex(), n));
  EXPECT_EQ(&n, &iterate_index<11>(IdentityIndex(), n));
}

struct PackIndex {
  template <::size_t N, class T>
  ::std::tuple<::size_t, T> operator()(const T& t) const {
    return make_tuple(N, t);
  }
};

TEST_F(IterateIndex, CallTree) {
  EXPECT_EQ(a, iterate_index<0>(PackIndex(), a));
  EXPECT_EQ(make_tuple(0, a), iterate_index<1>(PackIndex(), a));
  EXPECT_EQ(make_tuple(1, make_tuple(0, a)), iterate_index<2>(PackIndex(), a));
}

struct CopyTracker {
  CopyTracker(int) {}
  CopyTracker(const CopyTracker& other) {
    ADD_FAILURE() << "CopyTracker has been copied (unexpected)";
  }
};

struct PassCopyTrackerIndex {
  template <::size_t N>
  CopyTracker operator()(CopyTracker state) const {
    return 0;
  }
};

TEST_F(IterateIndex, NoCopies) {
  // iterate_index<9>() incurs no copies but iterate_index<10> incurs one copy.
  // Total number of copies performed by this call is floor((N - 1) / 9).
  iterate_index<9>(PassCopyTrackerIndex(), 0);
}

::std::string AppendReturn(::std::string state) {
  absl::StrAppend(&state, 0);
  return state;
}

TEST(Iterate, ReturnByValue) {
  EXPECT_EQ("N", iterate<0>(AppendReturn, std::string("N")));
  EXPECT_EQ("N0", iterate<1>(AppendReturn, "N"));
  EXPECT_EQ("N00", iterate<2>(AppendReturn, "N"));
  EXPECT_EQ("N000000000", iterate<9>(AppendReturn, "N"));
  EXPECT_EQ("N0000000000", iterate<10>(AppendReturn, "N"));
  EXPECT_EQ("N00000000000", iterate<11>(AppendReturn, "N"));
}

struct Identity {
  template <class T>
  const T& operator()(const T& state) const {
    return state;
  }
};

TEST(Iterate, ReturnByReference) {
  int n = 0;
  EXPECT_EQ(&n, &iterate<0>(Identity(), n));
  EXPECT_EQ(&n, &iterate<1>(Identity(), n));
  EXPECT_EQ(&n, &iterate<2>(Identity(), n));
  EXPECT_EQ(&n, &iterate<9>(Identity(), n));
  EXPECT_EQ(&n, &iterate<10>(Identity(), n));
  EXPECT_EQ(&n, &iterate<11>(Identity(), n));
}

struct PassCopyTracker {
  CopyTracker operator()(CopyTracker state) const { return 0; }
};

TEST(Iterate, NoCopies) { iterate<9>(PassCopyTracker(), 0); }

struct AppendIndex {
  template <::size_t I>
  void operator()() const {
    absl::StrAppend(s, I);
  }
  std::string* s;
};

template <::size_t N>
std::string DoAppendIndex(std::string state) {
  iterate_index<N>(AppendIndex{&state});
  return state;
}

TEST(IterateVoidIndex, Functional) {
  EXPECT_EQ("N", DoAppendIndex<0>("N"));
  EXPECT_EQ("N0", DoAppendIndex<1>("N"));
  EXPECT_EQ("N01", DoAppendIndex<2>("N"));
  EXPECT_EQ("N012345678", DoAppendIndex<9>("N"));
  EXPECT_EQ("N0123456789", DoAppendIndex<10>("N"));
  EXPECT_EQ("N012345678910", DoAppendIndex<11>("N"));
}

struct Increment {
  void operator()() const { ++*n; }
  int* n;
};

template <::size_t N>
int DoIncrement() {
  int n = 0;
  iterate<N>(Increment{&n});
  return n;
}

TEST(IterateVoid, Functional) {
  EXPECT_EQ(0, DoIncrement<0>());
  EXPECT_EQ(1, DoIncrement<1>());
  EXPECT_EQ(2, DoIncrement<2>());
  EXPECT_EQ(9, DoIncrement<9>());
  EXPECT_EQ(10, DoIncrement<10>());
  EXPECT_EQ(11, DoIncrement<11>());
}

}  // namespace
}  // namespace visionai
