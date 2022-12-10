// Copyright 2003 Google, Inc.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// Author: Marin Saric
//

#include "visionai/util/array/array2d.h"

#include <stdio.h>

#include <cstdint>
#include <memory>

#include "base/commandlineflags.h"
#include "base/init_google.h"
#include "base/integral_types.h"
#include "glog/logging.h"
#include "base/logging_extensions.h"
#include "absl/flags/flag.h"

template <typename T>
void TestFillArray2D(int32_t h, int32_t w) {
  Array2D<T> a(h, w);
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      a(y, x) = static_cast<T>(ctr);
    }
  }

  ctr = 0;

  // Check to see 'a' got filled correctly
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      CHECK_EQ(a(y, x), static_cast<T>(ctr));
    }
  }

  // Check contains functions
  CHECK(!a.Contains(-1, 0));
  CHECK(!a.Contains(0, -1));
  CHECK(!a.Contains(h-1, w));
  CHECK(!a.Contains(h, w-1));
  bool array_has_elements = (h != 0) && (w != 0);
  CHECK_EQ(a.Contains(0, 0), array_has_elements);
  CHECK_EQ(a.Contains(h-1, w-1), array_has_elements);
  if (h > 4 && w > 2) {
    CHECK(a.ContainsWithMargin(2, 1, 2, 1));
    CHECK(!a.ContainsWithMargin(2-1, 1, 2, 1));
    CHECK(!a.ContainsWithMargin(2, 1-1, 2, 1));
    CHECK(a.ContainsWithMargin(h-2-1, w-1-1, 2, 1));
    CHECK(!a.ContainsWithMargin(h-2-1, w-1, 2, 1));
    CHECK(!a.ContainsWithMargin(h-2, w-1-1, 2, 1));
  }
}

template <typename T>
void TestFillDefaultArray2D(int32_t h, int32_t w) {
  T value = 42;
  Array2D<T> a1(h, w, value);
  Array2D<T> a2(h, w);
  a2.Fill(value);

  for (int32_t y = 0; y < h; y++) {
    for (int32_t x = 0; x < w; x++) {
      CHECK_EQ(a1(y, x), value);
      CHECK_EQ(a2(y, x), value);
    }
  }
}

template <typename T>
void TestArray2DSharing(int32_t h, int32_t w) {
  Array2D<T> a(h, w);
  Array2D<T> b(SHARE_WITH_INSTANCE, &a);
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      a(y, x) = static_cast<T>(ctr);
    }
  }

  ctr = 0;

  // Make sure 'b' contains the same numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(a(y, x), b(y, x));
    }
  }
}

template <typename T>
void TestArray2DCopying(int32_t h, int32_t w) {
  Array2D<T> a(h, w);
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      a(y, x) = static_cast<T>(ctr);
    }
  }

  Array2D<T> b(COPY_FROM_INSTANCE, &a);

  ctr = 0;

  // Make sure 'b' contains the same numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(a(y, x), b(y, x));
    }
  }

  // Change numbers in 'b'
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      --b(y, x);
    }
  }

  // Make sure they got changed correctly
  // and independently from 'a'
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(b(y, x), (a(y, x) - 1));
    }
  }
}

template <typename T>
void TestForeignShareArray2D(int32_t h, int32_t w) {
  // A one dimensional buffer whose memory is used by the 2D array 'a'
  std::unique_ptr<T[]> single_d_buffer(new T[h * w]);
  // This 2D array mirrors the contents of the single_d_buffer
  Array2D<T> a(SHARE_WITH_FOREIGN_INSTANCE, h, w, single_d_buffer.get());
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      a(y, x) = static_cast<T>(ctr);
    }
  }

  ctr = 0;

  // Check to see 'a' got filled correctly
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      CHECK_EQ(a(y, x), static_cast<T>(ctr));
    }
  }

  ctr = 0;

  // Check to see if the single_d_buffer is
  // filled with exactly the same items
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      CHECK_EQ(single_d_buffer[ctr], static_cast<T>(ctr));
    }
  }

  // Create a new buffer
  std::unique_ptr<T[]> another_buffer(new T[h * w]);

  ctr = 0;

  // Fill it with different numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      another_buffer[ctr] = static_cast<T>(ctr*2);
    }
  }

  // Remap the array to the new buffer
  a.RemapToNewBuffer(another_buffer.get());

  ctr = 0;

  // Check to see that the numbers are correct
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      CHECK_EQ(a(y, x), static_cast<T>(ctr*2));
    }
  }

  ctr = 0;

  // Check to see that the other buffer's numbers
  // are still in tact
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x, ++ctr) {
      CHECK_EQ(single_d_buffer[ctr], static_cast<T>(ctr));
    }
  }
}

// Test a reallocated array has the right size and we can read and
// write to the first and last element without dying.
template <typename T>
void TestRealloc(int32_t h, int32_t w) {
  Array2D<T> array;
  CHECK_EQ(array.height(), 0);
  CHECK_EQ(array.width(), 0);
  array.Realloc(h, w);
  CHECK_EQ(array.height(), h);
  CHECK_EQ(array.width(), w);
  if (h > 0 && w > 0) {
    array(0, 0) = 5;
    CHECK_EQ(array(0, 0), 5);
    // Remember array(0, 0) and array(h - 1, w - 1) might be the same
    // array element. Test separately.
    array(h - 1, w - 1) = 6;
    CHECK_EQ(array(h - 1, w - 1), 6);
  }
}

// Tests an Array2D object can be copied.
template <typename T>
void TestCopyConstructorAndAssignment(int32_t h, int32_t w) {
  // Tests copy constructor.
  const T kTestData = static_cast<T>(10);
  Array2D<T> a(h, w, kTestData);
  Array2D<T> b(a);
  CHECK_EQ(b.height(), h);
  CHECK_EQ(a.width(), w);
  // Make sure 'b' contains the same numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(a(y, x), b(y, x));
    }
  }

  // Tests copy assignment operator.
  Array2D<T> c(1, 1, 0);
  c = a;
  CHECK_EQ(c.height(), h);
  CHECK_EQ(c.width(), w);
  // Make sure 'c' contains the same numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(a(y, x), c(y, x));
    }
  }
}

// Tests an Array2D object can be moved.
template <typename T>
void TestMoveConstructorAndAssignment(int32_t h, int32_t w) {
  // Tests move constructor.
  const T kTestData = static_cast<T>(10);
  Array2D<T> a(h, w, kTestData);
  T *data_ptr = a.data();
  Array2D<T> b(std::move(a));
  CHECK_EQ(b.height(), h);
  CHECK_EQ(b.width(), w);
  CHECK_EQ(b.data(), data_ptr);
  // Make sure 'b' contains the same numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(b(y, x), kTestData);
    }
  }

  // Tests move assignment operator.
  Array2D<T> c(1, 1, 0);
  c = std::move(b);
  CHECK_EQ(c.height(), h);
  CHECK_EQ(c.width(), w);
  CHECK_EQ(c.data(), data_ptr);
  // Make sure 'c' contains the same numbers
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      CHECK_EQ(c(y, x), kTestData);
    }
  }
}

// Perform a test battery on Array2D
// of height 'h' and width 'w' .
template <typename T>
void FullTestArray2D(int32_t h, int32_t w) {
  TestFillArray2D<T>(h, w);
  TestFillDefaultArray2D<T>(h, w);
  TestArray2DSharing<T>(h, w);
  TestArray2DCopying<T>(h, w);
  TestForeignShareArray2D<T>(h, w);
  TestRealloc<T>(h, w);
  TestCopyConstructorAndAssignment<T>(h, w);
  TestMoveConstructorAndAssignment<T>(h, w);
}

static void TestArray2D() {
  FullTestArray2D<int>(3, 4);
  FullTestArray2D<int>(5, 2);
  FullTestArray2D<int>(1, 1);
  FullTestArray2D<int>(0, 0);
  FullTestArray2D<float>(2, 9);
  FullTestArray2D<float>(7, 4);
  FullTestArray2D<double>(5, 8);
}

int main(int argc, char **argv) {
  absl::SetFlag(&FLAGS_logtostderr, true);
  InitGoogle(argv[0], &argc, &argv, true);
  TestArray2D();
  printf("PASS\n");
  return 0;
}
