// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/thread/sync_queue.h"

#include <future>
#include <numeric>
#include <vector>

#include "gtest/gtest.h"

namespace visionai {
namespace {

template <typename T>
void PushAndPop() {
  SyncQueue<T> q;
  auto push_queue = [&q](int n) {
    for (int i = 0; i < n; ++i) {
      q.Push(T{});
    }
  };
  auto pop_queue = [&q](int &pop_count) {
    while (true) {
      if (q.Pop().ok()) {
        ++pop_count;
      } else {
        return;
      }
    }
  };

  std::vector<std::future<void>> f_push;
  std::vector<std::future<void>> f_pop;
  int push_count = 10'000;
  int thread_count = 100;
  std::vector<int> pop_count_arr(thread_count);

  // Start push and pop threads.
  for (int i = 0; i < thread_count; ++i) {
    f_push.push_back(std::async(std::launch::async, push_queue, push_count));
    f_pop.push_back(
        std::async(std::launch::async, pop_queue, std::ref(pop_count_arr[i])));
  }

  std::chrono::milliseconds span(push_count * thread_count / 100);
  // Wait for all the push threads to finish.
  for (int i = 0; i < thread_count; ++i) {
    ASSERT_EQ(f_push[i].wait_for(span), std::future_status::ready);
  }

  // Wait for pop threads to pop all.
  while (q.Size() > 0) {
    sleep(1);
  }
  // Unblock pop threads.
  q.Cancel();
  // Wait for all the pop threads to finish.
  for (int i = 0; i < thread_count; ++i) {
    ASSERT_EQ(f_pop[i].wait_for(span), std::future_status::ready);
  }
  // check "number of pop" = "number of push"
  EXPECT_EQ(std::accumulate(pop_count_arr.begin(), pop_count_arr.end(), 0),
            push_count * thread_count);
  EXPECT_TRUE(q.Empty());
}

template <typename T>
void PushMoreAndPopLess() {
  SyncQueue<T> q;
  auto push_queue = [&q](int n) {
    for (int i = 0; i < n; ++i) {
      q.Push(T{});
    }
  };
  auto pop_queue = [&q](int n) {
    for (int i = 0; i < n; ++i) {
      auto item = q.Pop();
    }
  };

  std::vector<std::future<void>> f_push;
  std::vector<std::future<void>> f_pop;
  int push_count = 1000;
  int pop_count = 999;
  int thread_count = 10;

  // Start push threads.
  for (int i = 0; i < thread_count; ++i) {
    f_push.push_back(std::async(std::launch::async, push_queue, push_count));
  }

  std::chrono::milliseconds span(push_count * thread_count / 100);
  // Wait for all the push threads to finish.
  for (int i = 0; i < thread_count; ++i) {
    ASSERT_EQ(f_push[i].wait_for(span), std::future_status::ready);
  }

  // Start pop threads.
  for (int i = 0; i < thread_count; ++i) {
    f_pop.push_back(std::async(std::launch::async, pop_queue, pop_count));
  }

  // Wait for all the pop threads to finish.
  for (int i = 0; i < thread_count; ++i) {
    ASSERT_EQ(f_pop[i].wait_for(span), std::future_status::ready);
  }

  EXPECT_TRUE(!q.Empty());
}

// Test multiple threads pushing and poping elements that can be copied.
TEST(SyncQueueTest, CopyElement) { PushAndPop<int>(); }

// Test multiple threads pushing and poping elements that can only be moved.
TEST(SyncQueueTest, MoveOnlyElement) {
  struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly &&) = default;
    MoveOnly &operator=(MoveOnly &&) = default;
    MoveOnly(MoveOnly &) = delete;
    MoveOnly &operator=(const MoveOnly &) = delete;
  };
  PushAndPop<MoveOnly>();
}

TEST(SyncQueueTest, NonEmptyQueue) { PushMoreAndPopLess<int>(); }

}  // namespace
}  // namespace visionai
