// Copyright 2003 Google, Inc.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// We define an Array3D which we access through () operator.
// (Normally not allowed but the C++ style committee granted an exception.)
// Similar to Array2D -- see Array2D for more information.
//
// Example:
//   Array3D<T> my_array(d1, d2, d3);
//   my_array(i, j, k) = 117;

#ifndef UTIL_ARRAY_ARRAY3D_H_
#define UTIL_ARRAY_ARRAY3D_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <algorithm>

#include "glog/logging.h"
#include "visionai/util/array/array_common.h"  // IWYU pragma: export

template <typename T>
class Array3D {
 public:
  typedef std::ptrdiff_t PD;

  // Constructs a 3D array of dimension 0 x 0 x 0.
  Array3D();

  // Constructs a 3D array of dimensions n1,n2,n3
  Array3D(const PD n1, const PD n2, const PD n3);

  // Constructs a 3D array of dimensions n1,n2,n3, filled with a default value
  Array3D(const PD n1, const PD n2, const PD n3, T default_value);

  // COPY_FROM_INSTANCE: Constructs a copy of a 3D array
  // SHARE_WITH_INSTANCE: Constructs an auxilliary reference to a 3D array
  Array3D(InstantiationMode inst_mode, Array3D<T> *leader);

  // SHARE_WITH_FOREIGN_INSTANCE: Mirrors the given buffer as if it were a 3D
  // array.  WARNING: It is your responsibility to make sure that the buffer
  // lives longer than the instance of Array3D.
  //
  // TAKEOVER_FROM_FOREIGN_INSTANCE: Represents the given buffer
  // as if it were a 3D array. Takes responsibility of deleting it.
  // WARNING: You must not deallocate the passed buffer *EVER*.
  // Once you initialize an Array3D, it takes over the ownership
  // of the buffer. Once the Array3D is deallocated, the passed buffer
  // will be destroyed too.
  Array3D(InstantiationMode inst_mode, const PD n1, const PD n2, const PD n3,
          T *buffer);

  Array3D(const Array3D &);
  Array3D &operator=(const Array3D &);
  Array3D(Array3D &&src);
  Array3D &operator=(Array3D &&src);

  // Fill the array with a value.
  void Fill(T value) { std::fill_n(data_, num_elements_, value); }

  // If we are in SHARE_WITH_FOREIGN_INSTANCE mode,
  // the caller can choose to re-map the 3D array to some other buffer.
  // No other Array3Ds must depend on us for this to be possible.
  void RemapToNewBuffer(T *buffer);

  // Element accessors.
  T &operator()(const PD a, const PD b, const PD c);
  const T &operator()(const PD a, const PD b, const PD c) const;

  // Checks if an index is included in the bounds of the array
  bool Contains(const PD a, const PD b, const PD c) const;

  // Checks if an index +/- (da,db,dc) is included in the bounds of the array
  bool ContainsWithMargin(const PD a, const PD b, const PD c, int da, int db,
                          int dc) const;

  // Accessors for obvious 3D array information
  PD n1() const { return n1_; }
  PD n2() const { return n2_; }
  PD n3() const { return n3_; }
  PD num_elements() const { return num_elements_; }

  // Low-level accessor for stuff like memcmp, handle with care
  T *data() const { return data_; }

  // false if instantiated with SHARE_WITH_INSTANCE
  bool owns_data() const { return owns_data_; }

  // To find out who (if anyone) this instance got modeled after
  // Returns nullptr if there is no leader instance
  Array3D *leader() const { return leader_; }

  // To find out how many instances depend on this instance
  int number_of_followers() const { return number_of_followers_; }

  ~Array3D();

 private:
  // Resets to an "empty" state. The array has size of 0 x 0 x 0 and no
  // allocated data. The array is backed by an incompatible leader.
  // The only trickiness is that n2n3_lut_ and n3_lut_ are nullptr which would
  // normally be invalid but with a n1 of 0 it can't be dereferenced
  // so it's safe.
  //
  // This is a valid state and, because it has no allocated data, it
  // can be easily overwritten with a different state.
  //
  // If this instance is not new then you must call Release() before
  // Reset() to deallocate any data.
  void Reset();

  // Completely initialize, but do not fill with any value, an instance that
  // owns the data.
  void InitOwned(const PD n1, const PD n2, const PD n3);

  // Called from various constructor-places.
  // Allocates the data buffer and precalculates the
  // row_lut_ lookup table.
  void AllocateDataAndLUT(const PD n1, const PD n2, const PD n3);
  // Allocates and calculates just the LUT
  void AllocateLUT(const PD n1, const PD n2, const PD n3);

  // (Re)calculates the LUT
  void RecalculateLUT(const PD n1, const PD n2, const PD n3);

  void AddMeAsFollower();
  void RemoveMeAsFollower();

  // Deletes all memory allocated by this Array3D. After Release(), it's
  // safe to either re-allocate the array or delete this instance.
  void Release();

  PD n1_;
  PD n2_;
  PD n3_;

  PD num_elements_;

  T *data_;                  // array elements
  T **n2n3_lut_;             // n2n3_lut_[i] points into data_
  int *n3_lut_;              // n3_lut_(i,j) points into n2n3_lut_
  bool owns_data_;           // own our data_?
  int number_of_followers_;  // incremented by sharing
  bool compatible_leader_;   // true if leader object is an Array3D
  Array3D *leader_;
};

// ----------------- Implementation ------------------

template <typename T>
Array3D<T>::Array3D() {
  Reset();
}

template <typename T>
Array3D<T>::Array3D(const PD n1, const PD n2, const PD n3) {
  InitOwned(n1, n2, n3);
}

template <typename T>
Array3D<T>::Array3D(const PD n1, const PD n2, const PD n3, T default_value) {
  InitOwned(n1, n2, n3);
  Fill(default_value);
}

template <typename T>
Array3D<T>::Array3D(InstantiationMode inst_mode, Array3D<T> *leader) {
  Reset();
  compatible_leader_ = true;
  leader_ = leader;

  switch (inst_mode) {
    case COPY_FROM_INSTANCE:
      n1_ = leader->n1_;
      n2_ = leader->n2_;
      n3_ = leader->n3_;

      num_elements_ = leader->num_elements_;

      AllocateDataAndLUT(n1_, n2_, n3_);

      owns_data_ = true;

      memcpy(data_, leader->data_, num_elements_ * sizeof(T));
      return;

    case SHARE_WITH_INSTANCE:
      n1_ = leader->n1_;
      n2_ = leader->n2_;
      n3_ = leader->n3_;

      num_elements_ = leader->num_elements_;

      owns_data_ = false;

      data_ = leader->data_;
      n2n3_lut_ = leader->n2n3_lut_;
      n3_lut_ = leader->n3_lut_;

      AddMeAsFollower();
      return;

    default:
      // Yes, yes, only in debug mode, otherwise it's too
      // much code bloat for an instantiation
      DLOG(FATAL) << "Unknown instantiation mode";
      return;
  }
}

template <typename T>
Array3D<T>::Array3D(InstantiationMode inst_mode, const PD n1, const PD n2,
                    const PD n3, T *buffer) {
  Reset();
  n1_ = n1;
  n2_ = n2;
  n3_ = n3;
  num_elements_ = n1 * n2 * n3;
  data_ = buffer;

  DCHECK((inst_mode == SHARE_WITH_FOREIGN_INSTANCE) ||
         (inst_mode == TAKEOVER_FROM_FOREIGN_INSTANCE))
      << " Unsupported instantiation mode";

  assert(n1 > 0);
  assert(n2 > 0);
  assert(n3 > 0);

  if (inst_mode == TAKEOVER_FROM_FOREIGN_INSTANCE) owns_data_ = true;

  AllocateLUT(n1, n2, n3);
}

template <typename T>
Array3D<T>::Array3D(const Array3D &src) {
  Reset();
  *this = src;
}

template <typename T>
Array3D<T> &Array3D<T>::operator=(const Array3D &src) {
  Release();
  InitOwned(src.n1_, src.n2_, src.n3_);
  memcpy(data_, src.data_, num_elements_ * sizeof(T));
  return *this;
}

template <typename T>
Array3D<T>::Array3D(Array3D &&src) {
  Reset();
  *this = std::move(src);
}

template <typename T>
Array3D<T> &Array3D<T>::operator=(Array3D &&src) {
  Release();
  n1_ = src.n1_;
  n2_ = src.n2_;
  n3_ = src.n3_;
  num_elements_ = src.num_elements_;
  data_ = src.data_;
  n2n3_lut_ = src.n2n3_lut_;
  n3_lut_ = src.n3_lut_;
  owns_data_ = src.owns_data_;
  number_of_followers_ = src.number_of_followers_;
  compatible_leader_ = src.compatible_leader_;
  leader_ = src.leader_;
  src.Reset();
  return *this;
}

template <typename T>
void Array3D<T>::InitOwned(const PD n1, const PD n2, const PD n3) {
  assert(n1 >= 0);
  assert(n2 >= 0);
  assert(n3 >= 0);

  Reset();
  n1_ = n1;
  n2_ = n2;
  n3_ = n3;
  num_elements_ = n1 * n2 * n3;
  owns_data_ = true;
  number_of_followers_ = 0;
  AllocateDataAndLUT(n1, n2, n3);
}

template <typename T>
void Array3D<T>::Reset() {
  n1_ = 0;
  n2_ = 0;
  n3_ = 0;
  num_elements_ = 0;
  data_ = nullptr;
  n2n3_lut_ = nullptr;
  n3_lut_ = nullptr;
  owns_data_ = false;
  number_of_followers_ = 0;
  compatible_leader_ = false;
  leader_ = nullptr;
}

template <typename T>
void Array3D<T>::RemapToNewBuffer(T *new_buffer) {
  DCHECK(!owns_data_) << " You can't orphan this array's data!";
  DCHECK(!compatible_leader_) << " You cannot remap a buffer that belongs "
                                 "to an Array3D";
  // Don't allow arrays having dependents to suddenly change their
  // addresses as this is a source of leaks and hard-to-trace bugs
  DCHECK_EQ(number_of_followers_, 0) << "You have arrays dependent on "
                                        "this Array3D!";

  data_ = new_buffer;

  RecalculateLUT(n1_, n2_, n3_);
}

template <class T>
void Array3D<T>::Release() {
  DCHECK_EQ(number_of_followers_, 0)
      << "Arrays dependent on me are not deallocated\n";

  if (owns_data_) {
    delete[] data_;
    delete[] n2n3_lut_;
    delete[] n3_lut_;
  } else {
    if (compatible_leader_) {
      // and our leader is an Array3D
      RemoveMeAsFollower();
    } else {
      // and our leader is someone for whom we
      // had to allocate lookup tables
      delete[] n2n3_lut_;
      delete[] n3_lut_;
    }
  }
}

template <class T>
Array3D<T>::~Array3D() {
  Release();
}

template <class T>
const T &Array3D<T>::operator()(const PD a, const PD b, const PD c) const {
  assert(a >= 0);
  assert(a < n1_);
  assert(b >= 0);
  assert(b < n2_);
  assert(c >= 0);
  assert(c < n3_);
  return *(n2n3_lut_[a] + n3_lut_[b] + c);
}

template <class T>
T &Array3D<T>::operator()(const PD a, const PD b, const PD c) {
  assert(a >= 0);
  assert(a < n1_);
  assert(b >= 0);
  assert(b < n2_);
  assert(c >= 0);
  assert(c < n3_);
  return *(n2n3_lut_[a] + n3_lut_[b] + c);
}

template <class T>
bool Array3D<T>::Contains(const PD a, const PD b, const PD c) const {
  if (a < 0 || b < 0 || c < 0) return false;
  if (a >= n1_ || b >= n2_ || c >= n3_) return false;
  return true;
}

template <class T>
bool Array3D<T>::ContainsWithMargin(const PD a, const PD b, const PD c, int da,
                                    int db, int dc) const {
  if (a < da || b < db || c < dc) return false;
  if (a + da >= n1_ || b + db >= n2_ || c + dc >= n3_) return false;
  return true;
}

template <class T>
void Array3D<T>::AllocateDataAndLUT(const PD n1, const PD n2, const PD n3) {
  assert(n1 == n1_);
  assert(n2 == n2_);
  assert(n3 == n3_);
  data_ = new T[num_elements_];
  AllocateLUT(n1, n2, n3);
}

template <class T>
void Array3D<T>::AllocateLUT(const PD n1, const PD n2, const PD n3) {
  n2n3_lut_ = new T *[n1];
  n3_lut_ = new int[n2];
  RecalculateLUT(n1, n2, n3);
}

template <class T>
void Array3D<T>::RecalculateLUT(const PD n1, const PD n2, const PD n3) {
  int w = n2 * n3;
  for (int i = 0; i < n1; ++i) {
    n2n3_lut_[i] = &data_[i * w];
  }
  for (int i = 0; i < n2; ++i) {
    n3_lut_[i] = i * n3;
  }
}

template <class T>
void Array3D<T>::AddMeAsFollower() {
  assert(!owns_data_);
  assert(compatible_leader_);

  ++(leader_->number_of_followers_);
}

template <class T>
void Array3D<T>::RemoveMeAsFollower() {
  assert(!owns_data_);
  assert(compatible_leader_);

  --(leader_->number_of_followers_);
}

#endif  // UTIL_ARRAY_ARRAY3D_H_
