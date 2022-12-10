// Copyright 2020 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/producer_consumer_queue.h"

#include <memory>
#include <numeric>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "gtest/gtest.h"
#include "absl/memory/memory.h"
#include "absl/synchronization/mutex.h"

namespace visionai {

TEST(ProducerConsumerQueue, TestBlockingProducerBlockingConsumer) {
  constexpr int kCapacity = 1000;
  constexpr int kProducerWorkload = 2000;
  constexpr char kProducedValue[] = "forty-two";
  constexpr char kStoppingValue[] = "";
  constexpr int kNumProducers = 2;
  constexpr int kNumConsumers = 2;

  ProducerConsumerQueue<std::unique_ptr<std::string>> pcqueue(kCapacity);
  EXPECT_EQ(pcqueue.capacity(), kCapacity);

  // Start the consumers.
  absl::Mutex n_consumed_mu;
  std::vector<int> n_consumed(kNumConsumers, 0);
  std::vector<std::thread> consumers;

  for (int i = 0; i < kNumConsumers; ++i) {
    consumers.emplace_back([i, &pcqueue, &n_consumed, &n_consumed_mu,
                            kProducedValue, kStoppingValue]() {
      int n_popped = 0;
      std::unique_ptr<std::string> item;

      while (true) {
        pcqueue.Pop(item);
        if (*item == kStoppingValue) {
          break;
        }
        EXPECT_EQ(*item, kProducedValue);
        n_popped++;
      }

      {
        absl::MutexLock lock(&n_consumed_mu);
        n_consumed[i] = n_popped;
      }
    });
  }

  // Start the producers.
  std::vector<std::thread> producers;
  for (int i = 0; i < kNumProducers; ++i) {
    producers.emplace_back([&pcqueue, kProducedValue]() {
      for (int i = 0; i < kProducerWorkload; ++i) {
        pcqueue.Emplace(std::make_unique<std::string>(kProducedValue));
      }
    });
  }

  // Stop the producers.
  for (int i = 0; i < kNumProducers; ++i) {
    producers[i].join();
  }

  // Stop the consumers.
  for (int i = 0; i < kNumConsumers; ++i) {
    pcqueue.Emplace(std::make_unique<std::string>(kStoppingValue));
  }
  for (int i = 0; i < kNumConsumers; ++i) {
    consumers[i].join();
  }

  int total_consumed = std::accumulate(n_consumed.begin(), n_consumed.end(), 0);
  EXPECT_EQ(total_consumed, kProducerWorkload * kNumProducers);
  EXPECT_EQ(pcqueue.count(), 0);
}

TEST(ProducerConsumerQueue, TestAsyncProducerBlockingConsumer) {
  constexpr int kCapacity = 10;
  constexpr int kProducerWorkload = 100;
  constexpr int kProducedValue = 42;
  constexpr int kStoppingValue = -1;
  constexpr int kNumConsumers = 3;
  constexpr int kConsumerSleepMilliSeconds = 2000;

  ProducerConsumerQueue<int> pcqueue(kCapacity);
  EXPECT_EQ(pcqueue.capacity(), kCapacity);

  // Start the consumers.
  absl::Mutex n_consumed_mu;
  std::vector<int> n_consumed(kNumConsumers, 0);
  std::vector<std::thread> consumers;

  for (int i = 0; i < kNumConsumers; ++i) {
    consumers.emplace_back([i, &pcqueue, &n_consumed, &n_consumed_mu,
                            kStoppingValue, kProducedValue,
                            kConsumerSleepMilliSeconds]() {
      int n_popped = 0;
      int item = 0;

      while (true) {
        pcqueue.Pop(item);
        if (item == kStoppingValue) {
          break;
        }
        EXPECT_EQ(item, kProducedValue);
        n_popped++;
      }

      // Simulate a long latency consumer.
      std::this_thread::sleep_for(
          std::chrono::milliseconds(kConsumerSleepMilliSeconds));

      {
        absl::MutexLock lock(&n_consumed_mu);
        n_consumed[i] = n_popped;
      }
    });
  }

  // Run the producer.
  std::thread producer([&pcqueue, kProducedValue, kProducerWorkload]() {
    int n_pushed = 0;
    while (n_pushed < kProducerWorkload) {
      if (pcqueue.TryEmplace(kProducedValue)) {
        ++n_pushed;
      }
    }
  });
  producer.join();

  // Stop the consumers.
  for (int i = 0; i < kNumConsumers; ++i) {
    pcqueue.Emplace(kStoppingValue);
  }
  for (int i = 0; i < kNumConsumers; ++i) {
    consumers[i].join();
  }

  int total_consumed = std::accumulate(n_consumed.begin(), n_consumed.end(), 0);
  EXPECT_EQ(total_consumed, kProducerWorkload);
  EXPECT_EQ(pcqueue.count(), 0);
}

TEST(ProducerConsumerQueue, TestBlockingProducerAsyncConsumer) {
  constexpr int kCapacity = 10;
  constexpr int kProducerWorkload = 100;
  constexpr int kProducedValue = 42;
  constexpr int kStoppingValue = -1;
  constexpr int kNumConsumers = 3;
  constexpr int kProducerSleepMilliSeconds = 10;

  ProducerConsumerQueue<int> pcqueue(kCapacity);
  EXPECT_EQ(pcqueue.capacity(), kCapacity);

  // Start the consumers.
  absl::Mutex n_consumed_mu;
  std::vector<int> n_consumed(kNumConsumers, 0);
  std::vector<std::thread> consumers;

  for (int i = 0; i < kNumConsumers; ++i) {
    consumers.emplace_back([i, &pcqueue, &n_consumed, &n_consumed_mu,
                            kStoppingValue, kProducedValue]() {
      int n_popped = 0;
      int item = 0;

      while (true) {
        if (pcqueue.TryPop(item)) {
          if (item == kStoppingValue) {
            break;
          }
          EXPECT_EQ(item, kProducedValue);
          n_popped++;
        }
      }

      {
        absl::MutexLock lock(&n_consumed_mu);
        n_consumed[i] = n_popped;
      }
    });
  }

  // Run the producer.
  std::thread producer([&pcqueue, kProducerWorkload, kProducedValue,
                        kProducerSleepMilliSeconds]() {
    int n_pushed = 0;
    while (n_pushed < kProducerWorkload) {
      pcqueue.Emplace(kProducedValue);
      ++n_pushed;

      // Simulate a long latency producer.
      std::this_thread::sleep_for(
          std::chrono::milliseconds(kProducerSleepMilliSeconds));
    }
  });
  producer.join();

  // Stop the consumers.
  for (int i = 0; i < kNumConsumers; ++i) {
    pcqueue.Emplace(kStoppingValue);
  }
  for (int i = 0; i < kNumConsumers; ++i) {
    consumers[i].join();
  }

  int total_consumed = std::accumulate(n_consumed.begin(), n_consumed.end(), 0);
  EXPECT_EQ(total_consumed, kProducerWorkload);
  EXPECT_EQ(pcqueue.count(), 0);
}

}  // namespace visionai
