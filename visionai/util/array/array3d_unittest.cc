// Copyright 2003 Google, Inc.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// Author: Francois-Marie Lefevere
//
// Unit test for the 3D arrays

#include "visionai/util/array/array3d.h"

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
void TestFillArray3D(int32_t n1, int32_t n2, int32_t n3) {
  Array3D<T> a(n1, n2, n3);
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
        a(x, y, z) = static_cast<T>(ctr);
      }
    }
  }

  ctr = 0;

  // Check to see 'a' got filled correctly
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
        CHECK_EQ(a(x, y, z), static_cast<T>(ctr));
      }
    }
  }

  // Check contains functions
  CHECK(a.Contains(0, 0, 0));
  CHECK(!a.Contains(-1, 0, 0));
  CHECK(!a.Contains(0, -1, 0));
  CHECK(!a.Contains(0, 0, -1));
  CHECK(a.Contains(n1-1, n2-1, n3-1));
  CHECK(!a.Contains(n1, n2-1, n3-1));
  CHECK(!a.Contains(n1-1, n2, n3-1));
  CHECK(!a.Contains(n1-1, n2-1, n3));
  if (n1 > 4 && n2 > 2) {
    CHECK(a.ContainsWithMargin(2, 1, 0, 2, 1, 0));
    CHECK(!a.ContainsWithMargin(2-1, 1, 0, 2, 1, 0));
    CHECK(!a.ContainsWithMargin(2, 1-1, 0, 2, 1, 0));
    CHECK(!a.ContainsWithMargin(2, 1, 0-1, 2, 1, 0));
    CHECK(a.ContainsWithMargin(n1-2-1, n2-1-1, n3-1, 2, 1, 0));
    CHECK(!a.ContainsWithMargin(n1-2, n2-1-1, n3-1, 2, 1, 0));
    CHECK(!a.ContainsWithMargin(n1-2-1, n2-1, n3-1, 2, 1, 0));
    CHECK(!a.ContainsWithMargin(n1-2-1, n2-1-1, n3, 2, 1, 0));
  }
}

template <typename T>
void TestFillDefaultArray3D(int32_t n1, int32_t n2, int32_t n3) {
  T value = 42;
  Array3D<T> a1(n1, n2, n3, value);
  Array3D<T> a2(n1, n2, n3);
  a2.Fill(value);

  for (int32_t x = 0; x < n1; ++x) {
    for (int32_t y = 0; y < n2; ++y) {
      for (int32_t z = 0; z < n3; ++z) {
        CHECK_EQ(a1(x, y, z), value);
        CHECK_EQ(a2(x, y, z), value);
      }
    }
  }
}

template <typename T>
void TestArray3DSharing(int32_t n1, int32_t n2, int32_t n3) {
  Array3D<T> a(n1, n2, n3);
  Array3D<T> b(SHARE_WITH_INSTANCE, &a);
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    a(x, y, z) = static_cast<T>(ctr);
      }
    }
  }

  ctr = 0;

  // Make sure 'b' contains the same numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    CHECK_EQ(a(x, y, z), b(x, y, z));
      }
    }
  }
}

template <typename T>
void TestArray3DCopying(int32_t n1, int32_t n2, int32_t n3) {
  Array3D<T> a(n1, n2, n3);
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    a(x, y, z) = static_cast<T>(ctr);
      }
    }
  }

  Array3D<T> b(COPY_FROM_INSTANCE, &a);

  ctr = 0;

  // Make sure 'b' contains the same numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    CHECK_EQ(a(x, y, z), b(x, y, z));
      }
    }
  }

  // Change numbers in 'b'
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    --b(x, y, z);
      }
    }
  }

  // Make sure they got changed correctly
  // and independently from 'a'
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    CHECK_EQ(a(x, y, z) - 1, b(x, y, z));
      }
    }
  }
}

template <typename T>
void TestForeignShareArray3D(int32_t n1, int32_t n2, int32_t n3) {
  // A one dimensional buffer whose memory is used by the 3D array 'a'
  std::unique_ptr<T[]> single_d_buffer(new T[n1 * n2 * n3]);
  // This 3D array mirrors the contents of the single_d_buffer
  Array3D<T> a(SHARE_WITH_FOREIGN_INSTANCE, n1, n2, n3, single_d_buffer.get());
  int ctr;

  ctr = 0;

  // Fill 'a' with numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    a(x, y, z) = static_cast<T>(ctr);
      }
    }
  }

  ctr = 0;

  // Check to see 'a' got filled correctly
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
        CHECK_EQ(a(x, y, z), static_cast<T>(ctr));
      }
    }
  }

  ctr = 0;

  // Check to see if the single_d_buffer is
  // filled with exactly the same items
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
        CHECK_EQ(single_d_buffer[ctr], static_cast<T>(ctr));
      }
    }
  }

  // Create a new buffer
  std::unique_ptr<T[]> another_buffer(new T[n1 * n2 * n3]);

  ctr = 0;

  // Fill it with different numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
    another_buffer[ctr] = static_cast<T>(ctr*2);
      }
    }
  }

  // Remap the array to the new buffer
  a.RemapToNewBuffer(another_buffer.get());

  ctr = 0;

  // Check to see that the numbers are correct
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
        CHECK_EQ(a(x, y, z), static_cast<T>(ctr*2));
      }
    }
  }

  ctr = 0;

  // Check to see that the other buffer's numbers
  // are still intact
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z, ++ctr) {
        CHECK_EQ(single_d_buffer[ctr], static_cast<T>(ctr));
      }
    }
  }
}

// Tests an Array3D object can be copied.
template <typename T>
void TestCopyConstructorAndAssignment(int32_t n1, int32_t n2, int32_t n3) {
  // Tests copy constructor.
  const T kTestData = static_cast<T>(100);
  Array3D<T> a(n1, n2, n3, kTestData);
  Array3D<T> b(a);
  CHECK_EQ(b.n1(), n1);
  CHECK_EQ(b.n2(), n2);
  CHECK_EQ(b.n3(), n3);
  // Make sure 'b' contains the same numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z) {
        CHECK_EQ(a(x, y, z), b(x, y, z));
      }
    }
  }

  // Tests copy assignment operator.
  Array3D<T> c(1, 1, 1, 0);
  c = a;
  CHECK_EQ(c.n1(), n1);
  CHECK_EQ(c.n2(), n2);
  CHECK_EQ(c.n3(), n3);
  // Make sure 'c' contains the same numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z) {
        CHECK_EQ(a(x, y, z), c(x, y, z));
      }
    }
  }
}

// Tests an Array3D object can be moved.
template <typename T>
void TestMoveConstructorAndAssignment(int32_t n1, int32_t n2, int32_t n3) {
  // Tests move constructor.
  const T kTestData = static_cast<T>(100);
  Array3D<T> a(n1, n2, n3, kTestData);
  T *data_ptr = a.data();
  Array3D<T> b(std::move(a));
  CHECK_EQ(b.n1(), n1);
  CHECK_EQ(b.n2(), n2);
  CHECK_EQ(b.n3(), n3);
  CHECK_EQ(b.data(), data_ptr);
  // Make sure 'b' contains the same numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z) {
        CHECK_EQ(b(x, y, z), kTestData);
      }
    }
  }

  // Tests move assignment operator.
  Array3D<T> c(1, 1, 1, 0);
  c = std::move(b);
  CHECK_EQ(c.n1(), n1);
  CHECK_EQ(c.n2(), n2);
  CHECK_EQ(c.n3(), n3);
  CHECK_EQ(c.data(), data_ptr);
  // Make sure 'c' contains the same numbers
  for (int x = 0; x < n1; ++x) {
    for (int y = 0; y < n2; ++y) {
      for (int z = 0; z < n3; ++z) {
        CHECK_EQ(c(x, y, z), kTestData);
      }
    }
  }
}

// Perform a test battery on Array3D
template <typename T>
void FullTestArray3D(int32_t n1, int32_t n2, int32_t n3) {
  TestFillArray3D<T>(n1, n2, n3);
  TestFillDefaultArray3D<T>(n1, n2, n3);
  TestArray3DSharing<T>(n1, n2, n3);
  TestArray3DCopying<T>(n1, n2, n3);
  TestForeignShareArray3D<T>(n1, n2, n3);
  TestCopyConstructorAndAssignment<T>(n1, n2, n3);
  TestMoveConstructorAndAssignment<T>(n1, n2, n3);
}

static void TestArray3D() {
  FullTestArray3D<int>(3, 4, 4);
  FullTestArray3D<int>(5, 2, 3);
  FullTestArray3D<int>(1, 1, 7);
  FullTestArray3D<float>(2, 9, 1);
  FullTestArray3D<float>(7, 4, 5);
  FullTestArray3D<double>(5, 8, 2);
}

int main(int argc, char **argv) {
  absl::SetFlag(&FLAGS_logtostderr, true);
  InitGoogle(argv[0], &argc, &argv, true);
  TestArray3D();
  printf("PASS\n");
  return 0;
}
