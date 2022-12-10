// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/gtl/circularbuffer.h"

#include <map>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/strings/str_cat.h"

namespace visionai {
namespace gtl {

namespace {

using testing::ElementsAre;
using testing::Pointee;

struct NonCopyablePair {
  explicit NonCopyablePair(int first, int second)
      : first(first), second(second) {}

  NonCopyablePair(const NonCopyablePair&) = delete;
  NonCopyablePair& operator=(const NonCopyablePair&) = delete;

  friend std::ostream& operator<<(std::ostream& os, const NonCopyablePair& p) {
    return os << "(" << p.first << ", " << p.second << ")";
  }

  int first;
  int second;
};

// Matcher for NonCopyablePair.
MATCHER_P2(PairIs, first, second, "") {
  return arg.first == first && arg.second == second;
}

template <typename C>
void PushBackSequence(C* c, int lo, int hi) {
  for (; lo != hi; ++lo) {
    c->push_back(lo);
  }
}

template <typename C>
void PushFrontSequence(C* c, int lo, int hi) {
  for (; lo != hi; ++lo) {
    c->push_front(lo);
  }
}

template <typename C>
void EmplaceBackSequence(C* c, int lo, int hi) {
  for (; lo != hi; ++lo) {
    c->emplace_back(lo, hi);
  }
}

template <typename C>
void EmplaceFrontSequence(C* c, int lo, int hi) {
  for (; lo != hi; ++lo) {
    c->emplace_front(lo, hi);
  }
}

class CircularBufferTest : public ::testing::Test {
 public:
  void SetUp() { count_ = 0; }

 protected:
  struct Canary {
    explicit Canary(int val = 0) : val_(val) { ++(*count_); }
    Canary(const Canary& o) : val_(o.val_) { ++(*count_); }
    Canary& operator=(const Canary& o) = default;
    ~Canary() { --(*count_); }

    friend std::ostream& operator<<(std::ostream& os, const Canary& v) {
      return os << v.val_;
    }

    friend bool operator==(const Canary& a, const Canary& b) {
      return a.val_ == b.val_;
    }

    int val_;
    static int* count_;
  };

  static int count() { return count_; }
  static int count_;
};
int CircularBufferTest::count_ = 0;
int* CircularBufferTest::Canary::count_ = &CircularBufferTest::count_;

TEST_F(CircularBufferTest, PushBack) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 0, 2);
  EXPECT_THAT(cb, ElementsAre(0, 1));
  PushBackSequence(&cb, 2, 6);
  EXPECT_TRUE(cb.full());
  EXPECT_THAT(cb, ElementsAre(3, 4, 5));
}

TEST_F(CircularBufferTest, EmplaceBack) {
  CircularBuffer<NonCopyablePair> cb(3);
  EmplaceBackSequence(&cb, 0, 2);
  EXPECT_THAT(cb, ElementsAre(PairIs(0, 2), PairIs(1, 2)));
  EmplaceBackSequence(&cb, 2, 6);
  EXPECT_TRUE(cb.full());
  EXPECT_THAT(cb, ElementsAre(PairIs(3, 6), PairIs(4, 6), PairIs(5, 6)));
}

TEST_F(CircularBufferTest, EmplaceReturnsReference) {
  CircularBuffer<int> cb(3);
  int& ref_back = cb.emplace_back(511);
  EXPECT_EQ(511, ref_back);
  int& ref_front = cb.emplace_front(1963);
  EXPECT_EQ(1963, ref_front);
}

TEST_F(CircularBufferTest, At) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 3, 6);
  EXPECT_EQ(3, cb.at(0));
  EXPECT_EQ(4, cb.at(1));
  EXPECT_EQ(5, cb.at(2));
  EXPECT_EQ(5, cb.at(-1));
  EXPECT_EQ(4, cb.at(-2));
  EXPECT_EQ(3, cb.at(-3));
}

TEST_F(CircularBufferTest, PopFront) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 3, 6);
  EXPECT_EQ(3, cb.front());
  cb.pop_front();
  EXPECT_EQ(4, cb.front());
  cb.pop_front();
  EXPECT_EQ(5, cb.front());
  cb.pop_front();
  EXPECT_TRUE(cb.empty());
}

TEST_F(CircularBufferTest, PopBack) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 3, 6);
  EXPECT_EQ(5, cb.back());
  cb.pop_back();
  EXPECT_EQ(4, cb.back());
  cb.pop_back();
  EXPECT_EQ(3, cb.back());
  cb.pop_back();
  EXPECT_TRUE(cb.empty());
}

TEST_F(CircularBufferTest, Clear) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 2, 6);
  cb.clear();
  EXPECT_TRUE(cb.empty());
}

TEST_F(CircularBufferTest, PushFront) {
  CircularBuffer<int> cb(3);
  PushFrontSequence(&cb, 0, 5);
  EXPECT_THAT(cb, ElementsAre(4, 3, 2));
  // Test at()
  EXPECT_EQ(4, cb.at(0));
  EXPECT_EQ(3, cb.at(1));
  EXPECT_EQ(2, cb.at(2));
  // Test mutating.
  typedef CircularBuffer<int>::iterator Iter;
  for (Iter ii = cb.begin(); ii != cb.end(); ++ii) {
    *ii *= 2;
  }
  EXPECT_THAT(cb, ElementsAre(8, 6, 4));
}

TEST_F(CircularBufferTest, EmplaceFront) {
  CircularBuffer<NonCopyablePair> cb(3);
  EmplaceFrontSequence(&cb, 0, 5);
  EXPECT_THAT(cb, ElementsAre(PairIs(4, 5), PairIs(3, 5), PairIs(2, 5)));
  // Test at()
  EXPECT_THAT(cb.at(0), PairIs(4, 5));
  EXPECT_THAT(cb.at(1), PairIs(3, 5));
  EXPECT_THAT(cb.at(2), PairIs(2, 5));
  // Test mutating.
  typedef CircularBuffer<NonCopyablePair>::iterator Iter;
  for (Iter ii = cb.begin(); ii != cb.end(); ++ii) {
    ii->first *= 2;
  }
  EXPECT_THAT(cb, ElementsAre(PairIs(8, 5), PairIs(6, 5), PairIs(4, 5)));
}

TEST_F(CircularBufferTest, Assignment) {
  CircularBuffer<int> cb1(3);
  PushBackSequence(&cb1, 0, 3);
  CircularBuffer<int> cb2(3);
  cb2 = cb1;
  EXPECT_THAT(cb1, ElementsAre(0, 1, 2));
  EXPECT_THAT(cb2, ElementsAre(0, 1, 2));
}

TEST_F(CircularBufferTest, ResizingLarger) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 0, 3);
  cb.resize(4);
  EXPECT_THAT(cb, ElementsAre(0, 1, 2));
}

TEST_F(CircularBufferTest, ResizingSmaller) {
  CircularBuffer<int> cb(3);
  PushBackSequence(&cb, 0, 3);
  cb.resize(2);
  EXPECT_THAT(cb, ElementsAre(0, 1));
}

TEST_F(CircularBufferTest, CanBePutIntoMaps) {
  std::map<int, CircularBuffer<int>> m;
  m[0] = CircularBuffer<int>();
}

TEST_F(CircularBufferTest, EmptyConstruction) {
  // Test that a CircularBuffer creates no live elements.
  CircularBuffer<Canary> cb(2);
  EXPECT_EQ(0, count());
}

TEST_F(CircularBufferTest, PartiallyFilledLifetimes) {
  CircularBuffer<Canary> cb(3);
  cb.push_back(Canary(1));
  EXPECT_EQ(1, count());
  cb.push_back(Canary(2));
  EXPECT_EQ(2, count());
  cb.clear();
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(0, count());
}

TEST_F(CircularBufferTest, ClearDestruction) {
  // Test that a cleared CircularBuffer has no live elements.
  CircularBuffer<Canary> cb(2);
  cb.push_back(Canary(1));
  EXPECT_EQ(1, count());
  cb.push_back(Canary(2));
  EXPECT_EQ(2, count());
  cb.push_back(Canary(3));
  EXPECT_EQ(2, count());
  cb.clear();
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(0, count());
}

TEST_F(CircularBufferTest, AllBeginPositionsPushBack) {
  for (int i = 0; i < 3; ++i) {
    SCOPED_TRACE(i);
    CircularBuffer<Canary> cb(3);
    // Rotate 'begin_' up to position i:
    for (int j = 0; j < i; ++j) {
      cb.push_back(Canary(j));
    }
    for (int j = 0; j < i; ++j) {
      cb.pop_front();
    }
    cb.push_back(Canary(10));
    EXPECT_THAT(cb, ElementsAre(Canary(10)));
    cb.pop_back();
    EXPECT_THAT(cb, ElementsAre());
  }
}

template <typename T>
void Accept(const T& x) {}

// The default constructor makes a CircularBuffer of capacity 0.
// It has to be resized before it can hold elements, but it should
// otherwise be functional.
TEST_F(CircularBufferTest, DefaultConstructedTrivial) {
  // Try some different syntax
  { CircularBuffer<Canary> cb; }
  { CircularBuffer<Canary> cb{}; }
  { CircularBuffer<Canary> cb = {}; }
  { auto cb = CircularBuffer<Canary>{}; }
  { auto cb = CircularBuffer<Canary>(); }
  Accept<CircularBuffer<Canary>>({});
  CircularBuffer<Canary> cb;
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(0, cb.size());
  EXPECT_EQ(0, cb.capacity());
  EXPECT_EQ(0, std::distance(cb.begin(), cb.end()));
  EXPECT_EQ(0, cb.begin() - cb.end());
  EXPECT_EQ(0, (cb.begin() + 0) - cb.end());
}

TEST_F(CircularBufferTest, DefaultConstructed) {
  CircularBuffer<Canary> cb;

  cb.resize(1);
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(0, cb.size());
  EXPECT_EQ(1, cb.capacity());
  EXPECT_EQ(0, count());

  cb.push_back(Canary(1));
  EXPECT_EQ(1, cb.size());
  EXPECT_THAT(cb, ElementsAre(Canary(1)));
  EXPECT_EQ(1, count());

  cb.pop_back();
  EXPECT_THAT(cb, ElementsAre());
  EXPECT_EQ(0, count());
}

TEST_F(CircularBufferTest, AllBeginPositionsPushFront) {
  for (int i = 0; i < 3; ++i) {
    SCOPED_TRACE(i);
    CircularBuffer<Canary> cb(3);
    // Rotate 'begin_' up to position i:
    for (int j = 0; j < i; ++j) {
      cb.push_back(Canary(j));
    }
    for (int j = 0; j < i; ++j) {
      cb.pop_front();
    }
    cb.push_front(Canary(10));
    EXPECT_THAT(cb, ElementsAre(Canary(10)));
    cb.pop_front();
    EXPECT_THAT(cb, ElementsAre());
  }
}

TEST(CircularBufferIteratorTest, Iterators) {
  CircularBuffer<int> cb(10);
  // Position begin() roughly in the middle
  PushFrontSequence(&cb, 0, 7);
  for (int i = 0; i < 6; i++) {
    cb.pop_back();
  }
  // Fill up the circular buffer
  PushFrontSequence(&cb, 0, 10);
  CircularBuffer<int>::iterator ibegin = cb.begin();
  // Test assignment
  CircularBuffer<int>::iterator icurrent;
  icurrent = ibegin;
  EXPECT_TRUE(ibegin == icurrent);
  EXPECT_FALSE(ibegin != icurrent);
  EXPECT_EQ(0, ibegin - icurrent);
  EXPECT_EQ(0, icurrent - ibegin);

  // Test various operators
  icurrent++;
  EXPECT_EQ(1, icurrent - ibegin);
  icurrent--;
  EXPECT_EQ(0, ibegin - icurrent);
  icurrent += 3;
  EXPECT_EQ(3, icurrent - ibegin);
  icurrent -= 3;
  EXPECT_EQ(0, ibegin - icurrent);
  icurrent += 5;
  --icurrent;
  EXPECT_EQ(4, icurrent - ibegin);
  ++icurrent;
  EXPECT_EQ(5, icurrent - ibegin);
  EXPECT_EQ(-5, ibegin - icurrent);
  EXPECT_TRUE(ibegin < icurrent);
}

TEST(CircularBufferIteratorTest, ConstIterator) {
  CircularBuffer<int> cb(10);
  PushFrontSequence(&cb, 0, 10);
  const CircularBuffer<int>& ccb = cb;
  CircularBuffer<int>::const_iterator cit = ccb.begin();
  CircularBuffer<int>::iterator it = cb.begin();

  // Test assignment of const_iterator from iterator.
  CircularBuffer<int>::const_iterator ABSL_ATTRIBUTE_UNUSED test = it;

  // Test Comparisons between iterator and const_iterator.
  EXPECT_TRUE(it == cit);
  EXPECT_TRUE(cit == it);
  EXPECT_FALSE(it != cit);
  EXPECT_FALSE(cit != it);
  EXPECT_FALSE(it < cit);
  EXPECT_FALSE(cit < it);
  EXPECT_FALSE(it > cit);
  EXPECT_FALSE(cit > it);
  EXPECT_TRUE(it <= cit);
  EXPECT_TRUE(cit <= it);
  EXPECT_TRUE(it >= cit);
  EXPECT_TRUE(cit >= it);
}

TEST(CircularBufferIteratorTest, ReverseIterators) {
  CircularBuffer<int> cb(10);
  PushBackSequence(&cb, 0, 10);
  const CircularBuffer<int>& ccb = cb;
  std::vector<int> reversed(cb.rbegin(), cb.rend());
  std::vector<int> const_reversed(ccb.rbegin(), ccb.rend());
  EXPECT_THAT(reversed, ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
  EXPECT_THAT(const_reversed, ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
}

TEST(CircularBufferOrderingTest, Comparators) {
  CircularBuffer<int> cb_a(10);
  PushBackSequence(&cb_a, 0, 10);

  CircularBuffer<int> cb_b(5);
  PushBackSequence(&cb_b, 5, 10);

  EXPECT_TRUE(cb_a == cb_a);
  EXPECT_FALSE(cb_a == cb_b);
  EXPECT_TRUE(cb_a != cb_b);
  EXPECT_FALSE(cb_a != cb_a);
  EXPECT_TRUE(cb_a < cb_b);
  EXPECT_FALSE(cb_b < cb_a);
  EXPECT_TRUE(cb_a <= cb_b);
  EXPECT_FALSE(cb_b <= cb_a);
  EXPECT_TRUE(cb_b > cb_a);
  EXPECT_FALSE(cb_a > cb_b);
  EXPECT_TRUE(cb_b >= cb_a);
  EXPECT_FALSE(cb_a >= cb_b);
}

TEST(CircularBufferInAssociativeContainterTest, Map) {
  std::map<CircularBuffer<std::string>, int> m;
  CircularBuffer<std::string> cb(5);
  for (int i = 0; i < 10; ++i) {
    m.insert(std::make_pair(cb, i));
    cb.push_back(absl::StrCat(i));
  }

  EXPECT_EQ(m.size(), 10);

  cb.clear();
  for (int i = 0; i < 10; ++i) {
    const auto it = m.find(cb);
    ASSERT_TRUE(it != m.end());
    EXPECT_TRUE(cb == it->first);
    EXPECT_EQ(i, it->second);
    cb.push_back(absl::StrCat(i));
  }
}

TEST_F(CircularBufferTest, MoveOnly) {
  CircularBuffer<std::unique_ptr<Canary>> cb;

  cb.resize(1);
  EXPECT_TRUE(cb.empty());
  EXPECT_EQ(0, cb.size());
  EXPECT_EQ(1, cb.capacity());
  EXPECT_EQ(0, count());

  cb.push_back(std::unique_ptr<Canary>(new Canary(123)));
  EXPECT_EQ(1, cb.size());
  EXPECT_THAT(cb, ElementsAre(Pointee(Canary(123))));
  EXPECT_EQ(1, count());

  cb.pop_back();
  EXPECT_THAT(cb, ElementsAre());
  EXPECT_EQ(0, count());
}

TEST_F(CircularBufferTest, MoveConstruct) {
  CircularBuffer<std::unique_ptr<Canary>> cb1(1);
  cb1.push_back(std::unique_ptr<Canary>(new Canary(123)));
  CircularBuffer<std::unique_ptr<Canary>> cb2(std::move(cb1));
  EXPECT_THAT(cb1, ElementsAre());  // not specified, but test anyway
  EXPECT_THAT(cb2, ElementsAre(Pointee(Canary(123))));
  EXPECT_EQ(1, count());
  cb2.pop_back();
  EXPECT_THAT(cb2, ElementsAre());
  EXPECT_EQ(0, count());
}

TEST_F(CircularBufferTest, MoveAssign) {
  CircularBuffer<std::unique_ptr<Canary>> cb1(1);
  cb1.push_back(std::unique_ptr<Canary>(new Canary(123)));
  CircularBuffer<std::unique_ptr<Canary>> cb2(1);
  cb2.push_back(std::unique_ptr<Canary>(new Canary(456)));
  EXPECT_EQ(2, count());
  cb2 = std::move(cb1);
  EXPECT_EQ(1, count());
  EXPECT_THAT(cb1, ElementsAre());  // not specified, but test anyway
  EXPECT_THAT(cb2, ElementsAre(Pointee(Canary(123))));
  cb2.pop_back();
  EXPECT_THAT(cb2, ElementsAre());
  EXPECT_EQ(0, count());
}

}  // namespace

}  // namespace gtl
}  // namespace visionai
