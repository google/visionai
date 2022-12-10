// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// CircularBuffer<T> is a double-ended circular buffer with some
// configurable capacity for holding elements of type T.
//
// When a CircularBuffer<T> has reached capacity and a new element is
// pushed in at either end, an element from the opposite end will be
// evicted to make space for it.
//
// It is thread-compatible. Users must provide thread safety externally if
// needed.

#ifndef VISIONAI_UTIL_GTL_CIRCULARBUFFER_H_
#define VISIONAI_UTIL_GTL_CIRCULARBUFFER_H_

#include <stddef.h>

#include <algorithm>
#include <cassert>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include "glog/logging.h"
#include "absl/algorithm/container.h"
#include "visionai/util/gtl/container_logging.h"

namespace visionai {
namespace gtl {

// The double-ended circular buffer class.
template <typename T>
class CircularBuffer {
  // CircularBuffer contains raw storage for capacity() objects of type T.
  //
  // When the CircularBuffer is full(), i.e. when size() == capacity(),
  // pushing a new element to one end will evict an element from the
  // opposite end.
  //
  // When the CircularBuffer is not full(), pushing an element will place
  // a copy of the pushed object into the appropriate uninitialized buffer
  // slot via placement new.
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  template <bool IsConst>
  class Iterator;
  typedef Iterator<false> iterator;
  typedef Iterator<true> const_iterator;
  typedef typename std::reverse_iterator<iterator> reverse_iterator;
  typedef typename std::reverse_iterator<const_iterator> const_reverse_iterator;

  CircularBuffer() : CircularBuffer(0) {}
  explicit CircularBuffer(size_type c)
      : capacity_(c), begin_(0), size_(0), space_(Allocate(capacity_)) {}

  CircularBuffer(const CircularBuffer& o)
      : capacity_(o.capacity_),
        begin_(0),
        size_(o.size_),
        space_(Allocate(capacity_)) {
    pointer p = space_;
    for (const auto& e : o) Construct(p++, e);
  }

  CircularBuffer(CircularBuffer&& o) noexcept
      : capacity_(o.capacity_),
        begin_(o.begin_),
        size_(o.size_),
        space_(o.space_) {
    o.capacity_ = 0;
    o.begin_ = 0;
    o.size_ = 0;
    o.space_ = nullptr;
  }

  CircularBuffer& operator=(CircularBuffer&& o) noexcept(
      noexcept(std::declval<CircularBuffer>().clear())) {
    clear();
    Deallocate(space_, capacity_);
    capacity_ = o.capacity_;
    begin_ = o.begin_;
    size_ = o.size_;
    space_ = o.space_;
    o.capacity_ = 0;
    o.begin_ = 0;
    o.size_ = 0;
    o.space_ = nullptr;
    return *this;
  }

  CircularBuffer& operator=(const CircularBuffer& o) {
    return *this = CircularBuffer(o);
  }

  ~CircularBuffer() {
    clear();
    Deallocate(space_, capacity_);
  }

  void swap(CircularBuffer& b) noexcept {
    using std::swap;
    swap(capacity_, b.capacity_);
    swap(begin_, b.begin_);
    swap(size_, b.size_);
    swap(space_, b.space_);
  }

  friend void swap(CircularBuffer& a, CircularBuffer& b) noexcept { a.swap(b); }

  // Reallocates and sets capacity. Items from the front up to the
  // new_capacity are kept, e.g. [0, new_capacity).
  // Post: capacity() == new_capacity
  // Post: size() == min(old_size, new_capacity)
  void ChangeCapacity(size_type new_capacity) {
    if (new_capacity == capacity_) return;
    CircularBuffer tmp(new_capacity);
    std::copy(std::make_move_iterator(begin()),
              std::make_move_iterator(begin() + std::min(size_, new_capacity)),
              std::back_inserter(tmp));
    *this = std::move(tmp);
  }

  // DEPRECATED(billydonahue): Use 'ChangeCapacity' instead.
  // Note that the name is misleading. This resets capacity() and might
  // decrease size(). size() will not increase.
  void resize(size_type new_capacity) { ChangeCapacity(new_capacity); }

  // Push an item onto the beginning of the buffer. begin_ is moved
  // circularly to the left.
  // Requires: value_type is CopyConstructible and CopyAssignable.
  void push_front(const value_type& item) { PushFrontInternal(item); }
  // Requires: value_type is MoveConstructible and MoveAssignable.
  void push_front(value_type&& item) { PushFrontInternal(std::move(item)); }

  // Push an item onto the end of the buffer. begin_ is moved
  // circularly to the right if the buffer is full.
  // Requires: value_type is CopyConstructible and CopyAssignable.
  void push_back(const value_type& item) { PushBackInternal(item); }
  // Requires: value_type is MoveConstructible and MoveAssignable.
  void push_back(value_type&& item) { PushBackInternal(std::move(item)); }

  // Emplace an item onto the front of the buffer. begin_ is moved circularly to
  // the left.
  template <typename... Args>
  reference emplace_front(Args&&... args) {
    assert(capacity_);
    begin_ = prevpos(begin_);
    if (full()) {
      Destroy(space_ + begin_);
    } else {
      ++size_;
    }
    return Construct(space_ + begin_, std::forward<Args>(args)...);
  }

  // Emplace an item onto the back of the buffer. begin_ is moved circularly to
  // the right if the buffer is full.
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    assert(capacity_);
    if (full()) {
      Destroy(space_ + begin_);
      T& t = Construct(space_ + begin_, std::forward<Args>(args)...);
      begin_ = nextpos(begin_);
      return t;
    }
    ++size_;
    return Construct(space_ + logical_to_absolute(size_ - 1),
                     std::forward<Args>(args)...);
  }

  // Remove the front element.
  void pop_front() {
    CHECK_GT(size_, 0u);
    Destroy(&front());
    begin_ = nextpos(begin_);
    --size_;
  }

  // Remove the back element.
  void pop_back() {
    CHECK_GT(size_, 0u);
    Destroy(&back());
    --size_;
  }

  iterator begin() { return iterator(this, begin_); }
  const_iterator begin() const { return const_iterator(this, begin_); }

  iterator end() { return iterator(this); }
  const_iterator end() const { return const_iterator(this); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reference front() const { return space_[begin_]; }
  reference front() { return space_[begin_]; }

  const_reference back() const {
    return space_[logical_to_absolute(size_ - 1)];
  }
  reference back() { return space_[logical_to_absolute(size_ - 1)]; }

  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }

  bool empty() const { return size_ == 0; }
  bool full() const { return size_ == capacity_; }

  // For pos >= 0, returns the item at logical position 'pos'.
  // For pos < 0, returns the item at logical position 'pos + size()'
  reference at(difference_type pos) {
    size_type logical = pos + (pos < 0) * size_;
    DCHECK_LT(logical, size_);
    return space_[logical_to_absolute(logical)];
  }
  const_reference at(difference_type pos) const {
    size_type logical = pos + (pos < 0) * size_;
    DCHECK_LT(logical, size_);
    return space_[logical_to_absolute(logical)];
  }

  reference& operator[](size_type pos) { return at(pos); }
  const_reference& operator[](size_type pos) const { return at(pos); }

  void clear() noexcept(noexcept(
      std::declval<CircularBuffer>().Destroy(std::declval<pointer>()))) {
    for (size_type i = 0; i < size_; ++i) {
      Destroy(space_ + logical_to_absolute(i));
    }
    begin_ = 0;
    size_ = 0;
  }

 private:
  // U is 'value_type' or 'const value_type&'
  template <typename U>
  void PushFrontInternal(U&& item) {
    assert(capacity_);
    begin_ = prevpos(begin_);
    if (full()) {
      space_[begin_] = std::forward<U>(item);
      return;
    }
    ++size_;
    Construct(space_ + begin_, std::forward<U>(item));
  }

  // U is 'value_type' or 'const value_type&'
  template <typename U>
  void PushBackInternal(U&& item) {
    assert(capacity_);
    if (full()) {
      space_[begin_] = std::forward<U>(item);
      begin_ = nextpos(begin_);
      return;
    }
    ++size_;
    Construct(space_ + logical_to_absolute(size_ - 1), std::forward<U>(item));
  }

  template <typename... U>
  reference Construct(pointer p, U&&... v) {
    return *new (p) value_type(std::forward<U>(v)...);
  }
  void Destroy(pointer p) noexcept(noexcept(p->~value_type())) {
    p->~value_type();
  }

  pointer Allocate(size_type n) {
    return std::allocator<value_type>().allocate(n);
  }

  void Deallocate(pointer p, size_type n) {
    std::allocator<value_type>().deallocate(p, n);
  }

  reference at_absolute(size_type pos) { return space_[pos]; }
  const_reference at_absolute(size_type pos) const { return space_[pos]; }

  // Pre: logical in [0, size).
  size_type logical_to_absolute(size_type logical) const {
    DCHECK_LT(logical, size_);
    size_type absolute = begin_ + logical;
    if (absolute >= capacity_) absolute -= capacity_;
    DCHECK_LT(absolute, capacity_);
    return absolute;
  }

  // Pre: absolute in [0, capacity).
  size_type absolute_to_logical(size_type absolute) const {
    DCHECK_LT(absolute, capacity_);
    size_type logical = capacity_ - begin_ + absolute;
    if (logical >= capacity_) logical -= capacity_;
    DCHECK_LE(logical, capacity_);
    return logical;
  }

  // Pre: absolute in [0, capacity).
  size_type nextpos(size_type absolute) const {
    CHECK_LT(absolute, capacity_);
    ++absolute;
    if (absolute == capacity_) absolute = 0;
    return absolute;
  }

  // Pre: absolute in [0, capacity).
  size_type prevpos(size_type absolute) const {
    CHECK_LT(absolute, capacity_);
    if (absolute == 0) absolute += capacity_;
    --absolute;
    return absolute;
  }

  size_type capacity_;
  size_type begin_;
  size_type size_;
  value_type* space_;
};

// Iterators are invalidated by modification to the circular buffer.
template <typename T>
template <bool IsConst>
class CircularBuffer<T>::Iterator {
 private:
  typedef typename std::conditional<IsConst, const CircularBuffer,
                                    CircularBuffer>::type container_type;

 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef typename std::remove_cv<T>::type value_type;
  typedef
      typename std::conditional<IsConst, typename container_type::const_pointer,
                                typename container_type::pointer>::type pointer;
  typedef typename std::conditional<
      IsConst, typename container_type::const_reference,
      typename container_type::reference>::type reference;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::difference_type difference_type;

  Iterator() : cb_(nullptr), pos_(kEnd) {}

  explicit Iterator(container_type* cb) : cb_(cb), pos_(kEnd) {}

  Iterator(container_type* cb, size_type pos) : cb_(cb) {
    pos_ = (cb_->empty() ? kEnd : pos);
  }

  friend class Iterator<!IsConst>;

  // For const_iterator, this defines an implicit conversion from iterator.
  // For iterator, this defines a copy constructor.
  // See Matt Austern, "The Standard Librarian : Defining Iterators and
  // Const Iterators", Dr. Dobbs' Journal, January 01, 2001.
  // http://www.drdobbs.com/the-standard-librarian-defining-iterato/184401331
  Iterator(const Iterator<false>& o)  // NOLINT(runtime/explicit)
      : cb_(o.cb_), pos_(o.pos_) {}

  void swap(Iterator& o) {
    using std::swap;
    swap(cb_, o.cb_);
    swap(pos_, o.pos_);
  }

  Iterator& operator=(Iterator another) {
    swap(another);
    return *this;
  }
  reference operator*() const { return Deref(); }
  pointer operator->() const { return &**this; }
  reference operator[](size_type n) const { return *(*this + n); }
  Iterator& operator+=(difference_type n) { return Incr(n); }
  Iterator& operator-=(difference_type n) { return *this += -n; }
  Iterator& operator++() { return *this += 1; }
  Iterator& operator--() { return *this -= 1; }
  Iterator operator++(int) {
    Iterator t = *this;
    ++*this;
    return t;
  }
  Iterator operator--(int) {
    Iterator t = *this;
    --*this;
    return t;
  }
  friend Iterator operator+(Iterator v, difference_type n) { return v += n; }
  friend Iterator operator+(difference_type n, Iterator v) { return v += n; }
  friend Iterator operator-(Iterator v, difference_type n) { return v -= n; }
  friend difference_type operator-(const Iterator& a, const Iterator& b) {
    return Subtract(a, b);
  }
  friend bool operator==(Iterator a, Iterator b) { return a.pos_ == b.pos_; }
  friend bool operator!=(Iterator a, Iterator b) { return !(a == b); }
  friend bool operator<(Iterator a, Iterator b) { return b - a > 0; }
  friend bool operator>(Iterator a, Iterator b) { return b < a; }
  friend bool operator<=(Iterator a, Iterator b) { return !(a > b); }
  friend bool operator>=(Iterator a, Iterator b) { return !(a < b); }

  friend void swap(Iterator& a, Iterator& b) { a.swap(b); }

 private:
  // When an iterator would reach the size() of its CircularBuffer, its
  // pos_ is set to kEnd.
  enum { kEnd = -1 };

  reference Deref() const { return cb_->at_absolute(pos_); }

  Iterator& Incr(difference_type n) {
    const size_type logical = static_cast<size_type>(logical_pos() + n);
    CHECK_LE(logical, cb_->size())
        << "bad Incr by n=" << n << " from " << logical_pos();
    pos_ = (logical == cb_->size() ? kEnd : cb_->logical_to_absolute(logical));
    return *this;
  }

  static difference_type Subtract(const Iterator& a, const Iterator& b) {
    return static_cast<difference_type>(a.logical_pos()) -
           static_cast<difference_type>(b.logical_pos());
  }

  size_type logical_pos() const {
    return pos_ == kEnd ? cb_->size() : cb_->absolute_to_logical(pos_);
  }

  container_type* cb_;  // not owned
  size_type pos_;       // absolute position in *cb_.
};

template <typename T>
inline std::ostream& operator<<(std::ostream& os,
                                const CircularBuffer<T>& obj) {
  return os << gtl::LogContainer(obj);
}

// relational operators
template <typename T>
inline bool operator==(const CircularBuffer<T>& a, const CircularBuffer<T>& b) {
  return absl::c_equal(a, b);
}

template <typename T>
inline bool operator<(const CircularBuffer<T>& a, const CircularBuffer<T>& b) {
  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename T>
inline bool operator!=(const CircularBuffer<T>& a, const CircularBuffer<T>& b) {
  return !(a == b);
}

template <typename T>
inline bool operator>(const CircularBuffer<T>& a, const CircularBuffer<T>& b) {
  return b < a;
}

template <typename T>
inline bool operator<=(const CircularBuffer<T>& a, const CircularBuffer<T>& b) {
  return !(a > b);
}

template <typename T>
inline bool operator>=(const CircularBuffer<T>& a, const CircularBuffer<T>& b) {
  return !(a < b);
}

}  // namespace gtl
}  // namespace visionai

#endif  // VISIONAI_UTIL_GTL_CIRCULARBUFFER_H_
