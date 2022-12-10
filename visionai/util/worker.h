/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_WORKER_H_
#define VISIONAI_UTIL_WORKER_H_

#include <string>
#include <thread>  // NOLINT

#include "visionai/util/completion_signal.h"
#include "visionai/util/producer_consumer_queue.h"

namespace visionai {

namespace util {
// --------------------------------------------------------------------
// Basic worker-channel implementation (i.e. a tiny dataflow system).
//
// Workers run business logic and communicate values to the other workers
// through Channels. A Channel is a pcqueue for one Worker to send values
// to another. It also allows either worker to know whether each other have
// completed their processing.
//
// The typical workflow is to create a set of Workers, some Channels, and use
// the free function Attach to connect them together. See
// GstreamerVideoExporter::Run() later on for an example.

namespace {                                      // NOLINT
constexpr char kDefaultWorkerName[] = "Worker";  // NOLINT
}  // namespace

// A Channel carries values of type T.
template <typename T>
class Channel {
 public:
  // Create a channel of size `size`.
  //
  // CHECK-fails for non-positive sizes.
  explicit Channel(int size);

  // Set `worker` to be the value source/destination of this channel.
  template <typename U>
  void SetSrc(const U& worker);
  template <typename U>
  void SetDst(const U& worker);

  // Returns true if the channel has a value source/destination.
  //
  // CHECK-fails if you have not set a source/destination.
  bool HasSrc() const;
  bool HasDst() const;

  // Returns true if the source/destination worker has completed.
  //
  // CHECK-fails if you have not set a source/destination.
  bool IsSrcCompleted() const;
  bool IsDstCompleted() const;

  // Returns a reference to the pcqueue.
  //
  // Use this and the ProducerConsumerQueue API to send/receive values.
  std::shared_ptr<ProducerConsumerQueue<T>> pcqueue() { return pcqueue_; }

  ~Channel() = default;
  Channel(const Channel&) = delete;
  Channel& operator=(const Channel&) = delete;

 private:
  std::shared_ptr<CompletionSignal> src_completion_signal_ = nullptr;
  std::shared_ptr<CompletionSignal> dst_completion_signal_ = nullptr;
  std::shared_ptr<ProducerConsumerQueue<T>> pcqueue_ = nullptr;
};

template <typename T>
Channel<T>::Channel(int size) {
  CHECK_GT(size, 0);
  pcqueue_ = std::make_shared<ProducerConsumerQueue<T>>(size);
}

template <typename T>
template <typename U>
void Channel<T>::SetSrc(const U& worker) {
  src_completion_signal_ = worker.GetCompletionSignal();
}

template <typename T>
template <typename U>
void Channel<T>::SetDst(const U& worker) {
  dst_completion_signal_ = worker.GetCompletionSignal();
}

template <typename T>
bool Channel<T>::HasSrc() const {
  return src_completion_signal_ != nullptr;
}

template <typename T>
bool Channel<T>::HasDst() const {
  return dst_completion_signal_ != nullptr;
}

template <typename T>
bool Channel<T>::IsSrcCompleted() const {
  CHECK(src_completion_signal_ != nullptr);
  return src_completion_signal_->IsCompleted();
}

template <typename T>
bool Channel<T>::IsDstCompleted() const {
  CHECK(dst_completion_signal_ != nullptr);
  return dst_completion_signal_->IsCompleted();
}

// CRTP base class for Workers.
//
// A Worker consumes any number of inputs and produces any number of outputs.
// These input/output values all flow through Channels of the given type.
//
// For example, to create a new Worker that accepts int values from a Channel
// and produces double values to another Channel, do the following:
//
// class MySpecialWorker : Worker<MySpecialWorker,
//                                std::tuple<int>,
//                                std::tuple<double>> {
//    // your implementation goes here.
// };
//
// See StreamServerSource or LocalVideoSaver for more examples.
template <typename T, typename... Ts>
class Worker;

template <typename T, typename... Is, typename... Os>
class Worker<T, std::tuple<Is...>, std::tuple<Os...>> {
 public:
  // Async call to start working in the background.
  void Work();

  // Wait for the worker to complete up to a `timeout`.
  //
  // If the worker has completed, reclaim the background thread and return true.
  // Otherwise, let the worker keep working and return false.
  bool Join(absl::Duration timeout);

  // Get/Set the name of this worker.
  void SetName(const std::string& name);
  std::string GetName();

  // Get the return Status of the specific logic executed by this Worker.
  //
  // Call this after you have successfully Join()'d.
  absl::Status GetStatus();

  ~Worker();
  Worker() = default;
  Worker(const Worker&) = delete;
  Worker& operator=(const Worker&) = delete;

  std::shared_ptr<CompletionSignal> GetCompletionSignal() const {
    return completion_signal_;
  }

  template <int N>
  std::shared_ptr<
      Channel<typename std::tuple_element<N, std::tuple<Is...>>::type>>&
  GetInputChannel() {
    return std::get<N>(in_channels_);
  }

  template <int N>
  std::shared_ptr<
      Channel<typename std::tuple_element<N, std::tuple<Os...>>::type>>&
  GetOutputChannel() {
    return std::get<N>(out_channels_);
  }

 private:
  T* derived() { return static_cast<T*>(this); }

  std::string name_ = kDefaultWorkerName;
  std::thread worker_thread_;
  std::shared_ptr<CompletionSignal> completion_signal_ =
      std::make_shared<CompletionSignal>();

 protected:
  std::tuple<std::shared_ptr<Channel<Is>>...> in_channels_;
  std::tuple<std::shared_ptr<Channel<Os>>...> out_channels_;
};

template <typename T, typename... Is, typename... Os>
void Worker<T, std::tuple<Is...>, std::tuple<Os...>>::Work() {
  completion_signal_->Start();
  worker_thread_ = std::thread([this]() {
    auto status = derived()->WorkImpl();
    completion_signal_->SetStatus(status);
    completion_signal_->End();
    return;
  });
}

template <typename T, typename... Is, typename... Os>
bool Worker<T, std::tuple<Is...>, std::tuple<Os...>>::Join(
    absl::Duration timeout) {
  if (!completion_signal_->WaitUntilCompleted(timeout)) {
    return false;
  }
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
  return true;
}

template <typename T, typename... Is, typename... Os>
void Worker<T, std::tuple<Is...>, std::tuple<Os...>>::SetName(
    const std::string& name) {
  name_ = name;
}

template <typename T, typename... Is, typename... Os>
std::string Worker<T, std::tuple<Is...>, std::tuple<Os...>>::GetName() {
  return name_;
}

template <typename T, typename... Is, typename... Os>
absl::Status Worker<T, std::tuple<Is...>, std::tuple<Os...>>::GetStatus() {
  return completion_signal_->GetStatus();
}

template <typename T, typename... Is, typename... Os>
Worker<T, std::tuple<Is...>, std::tuple<Os...>>::~Worker() {
  if (worker_thread_.joinable()) {
    worker_thread_.detach();
  }
}

// Attach `channel` from the `M`th output of `src_worker` to the `N`th
// input of the `dst_worker`.
template <int M, int N, typename T, typename SrcT, typename DstT>
absl::Status Attach(std::shared_ptr<Channel<T>> channel,
                    std::shared_ptr<SrcT> src_worker,
                    std::shared_ptr<DstT> dst_worker) {
  if (channel == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to the Channel.");
  }
  if (src_worker == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to the source Worker.");
  }
  if (dst_worker == nullptr) {
    return absl::InvalidArgumentError(
        "Given a nullptr to the destination Worker.");
  }
  channel->SetSrc(*src_worker);
  channel->SetDst(*dst_worker);
  src_worker->template GetOutputChannel<M>() = channel;
  dst_worker->template GetInputChannel<N>() = channel;
  return absl::OkStatus();
}

}  // namespace util
}  // namespace visionai

#endif  // VISIONAI_UTIL_WORKER_H_
