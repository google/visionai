// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_READ_WRITE_CHANNEL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_READ_WRITE_CHANNEL_H_

#include <deque>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/streams/client/picked_notification.h"
#include "visionai/streams/client/read_write_channel_internal.h"

namespace visionai {
namespace streams_internal {

template <typename T>
class ReadWriteChannel;

// This is the reader interface to a `ReadWriteChannel`.
template <typename T>
class Reader {
 public:
  // Read a new message.
  //
  // The return boolean indicates whether the `timeout` has expired, and `ok`
  // indicates whether the read contains a new element.
  //
  // Blocks until either
  //
  // 1. `timeout` expires.
  //
  //    `*item` left unchanged.
  //    `*ok` left unchanged.
  //    Returns `false`.
  //
  // 2. Successfully read a new item.
  //
  //    `*item` stores the newly read item.
  //    `*ok` set to `true`.
  //    Returns `true`.
  //
  // 3. The writer has finished its writes by calling `Close`.
  //
  //    `*item` left unchanged.
  //    `*ok` set to `false`.
  //    Returns `true`.
  bool Read(absl::Duration timeout, T* item, bool* ok) {
    return rep_->TryPop(timeout, item, ok);
  }

 private:
  ReadWriteChannelState<T>* rep_ = nullptr;

  friend class ReadWriteChannel<T>;
  explicit Reader(ReadWriteChannelState<T>* rep) : rep_(rep) {}

  // Not copyable or assignable.
  Reader(const Reader&) = delete;
  Reader& operator=(const Reader&) = delete;

 public:
  // (Experimental) Set a `PickedNotification` to be signaled when a write
  // event has occurred. The following are write events:
  //  * When the channel is closed.
  //  * When the channel has an element for reading.
  //
  // `pick_id` will be set on the `PickedNotification` if it is signaled
  // through this instance.
  //
  // Important: The notification will only indicate that an event has occurred
  // since a successful call, not that the event still holds.
  //
  // Calls will be successful if all notifications set previously have been
  // `Notify`'d. Unset happens automatically.
  //
  // Returns true if set successfully, otherwise false.
  bool SetWriteEventNotification(std::shared_ptr<PickedNotification> n,
                                 size_t pick_id) {
    return rep_->SetWriteEventNotification(n, pick_id);
  }
};

// This is the writer interface to a `ReadWriteChannel`.
template <typename T>
class Writer {
 public:
  // Write a message.
  //
  // The return boolean indicates whether the `timeout` has expired, and `ok`
  // indicates whether the write succeeded.
  //
  // Blocks until either
  //
  // 1. `timeout` expires.
  //
  //    `*ok` left unchanged.
  //    Returns `false`.
  //
  // 2. Successfully wrote `item`.
  //
  //    `*ok` set to `true`.
  //    Returns `true`.
  //
  // 3. Writes on the channel have beeen cancelled, i.e. `CancelWriters` has
  //    been issued on the channel.
  //
  //    `*ok` set to `false`.
  //    Returns `true`.
  bool Write(absl::Duration timeout, const T& item, bool* ok) {
    return rep_->TryEmplace(timeout, ok, item);
  }
  bool Write(absl::Duration timeout, T&& item, bool* ok) {
    return rep_->TryEmplace(timeout, ok, std::move(item));
  }

  // Signals to the readers that writes have completed.
  //
  // All reader's `Read`s will return false once all messages have been
  // consumed.
  void Close() { rep_->CloseWrites(); }

 private:
  ReadWriteChannelState<T>* rep_ = nullptr;

  friend class ReadWriteChannel<T>;
  explicit Writer(ReadWriteChannelState<T>* rep) : rep_(rep) {}

  // Not copyable or assignable.
  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;
};

// This is a producer consumer queue that buffers `capacity` items.
//
// The reader may keep consuming items, so long as the writer has not signaled
// that it is done.
//
// The writer may keep producing elements until it is done, at which
// point they will explicitly close the channel.
//
// Example usage:
//
//   // Construct a channel. In this case, it is a one element buffer.
//   ReadWriteChannel<int> channel(1);
//
//   // The reader just keeps reading until the writer closes.
//   // This example prints 0 up to 9.
//   std::thread reader([&channel] () {
//     int result;
//     while (channel.reader()->Read(&result)) {
//       LOG(INFO) << result;
//     }
//   });
//
//   // The writer produces 0 up to 9. Afterwards, it explicitly closes the
//   // its writes to unblock any waiting readers.
//   for (int i = 0; i < 10; ++i) {
//     channel.writer()->Write(i);
//   }
//   channel.writer()->Close();
//
//   // Join the reader. Since the writer has closed, this unblocks eventually.
//   reader.join();
template <typename T>
class ReadWriteChannel {
 public:
  explicit ReadWriteChannel(size_t capacity)
      : state_(capacity), reader_(&state_), writer_(&state_) {}

  // Get the reader and writer endpoints for this channel.
  // They are valid as long as the channel is.
  Reader<T>* reader() { return &reader_; }
  Writer<T>* writer() { return &writer_; }

  // Cancels all writers. Any writers that are currently blocked attempting to
  // write will unblock, and any future writers will no longer able to write.
  void CancelWriters() { state_.CancelWriters(); }

 private:
  ReadWriteChannelState<T> state_;
  Reader<T> reader_;
  Writer<T> writer_;

  // Not copyable or assignable.
  ReadWriteChannel(const ReadWriteChannel&) = delete;
  ReadWriteChannel& operator=(const ReadWriteChannel&) = delete;
};

// ----------------------------------------------------------------------------
// Experimental
// ----------------------------------------------------------------------------

template <typename T>
using ReaderArray = absl::InlinedVector<streams_internal::Reader<T>*, 4>;

template <typename T>
int SelectForWriteEvent(const ReaderArray<T>& readers) {
  auto picked_notification =
      std::make_shared<streams_internal::PickedNotification>();
  for (size_t i = 0; i < readers.size(); ++i) {
    readers[i]->SetWriteEventNotification(picked_notification, i);
  }
  picked_notification->WaitForNotification();
  return picked_notification->GetPicked();
}

template <typename T>
int SelectForWriteEvent(
    std::initializer_list<streams_internal::Reader<T>*> list) {
  ReaderArray<T> readers(std::move(list));
  return SelectForWriteEvent(readers);
}

}  // namespace streams_internal
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_READ_WRITE_CHANNEL_H_
