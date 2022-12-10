/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

// Reused definitions needed by Array2d, Array3d.
//
// Currently 'enum InstantiationMode'
//
#ifndef UTIL_ARRAY_ARRAY_COMMON_H_
#define UTIL_ARRAY_ARRAY_COMMON_H_

// We define enumerations describing the method by which a new instance
// of a class is constructed, such as COPY_FROM_INSTANCE or SHARE_WITH_INSTANCE
// to facilitate the readability and maintability of both the class
// implementation and the code interfacing with such a class.
//
// This topic was discussed on google.eng.c-style under the thread
// "Copying from and/or sharing the content..." dated 17/07/2003
// and this solution was deemed good by the committee.
//
// There are many resources of a class that could be copied or shared,
// this is not meant to be a placeholder for every possible combination
// of every possible resource; this file is appropriate for most of classes
// that usually have one resource that one would be interested in
// sharing/copying and that one resource corresponds to the resource
// specified in the regular (non-copy/share) constructor.
enum InstantiationMode {
  // Instantiates a new class instance as a
  // copy of another instance of the same class
  COPY_FROM_INSTANCE = 1,
  // Instantiates a new class instance that
  // mirrors another instance of the same class
  // in contents
  SHARE_WITH_INSTANCE,
  // Instantiates a new class instance that
  // mirrors an instance of some different class/type
  // in contents. For example, a 2D array mirroring a
  // 1D array of the same size
  SHARE_WITH_FOREIGN_INSTANCE,
  // Behaves just like SHARE_WITH_FOREIGN_INSTANCE,
  // except that we claim ownership of the data
  // passed to us. Thus, we are responsible for
  // disposing of it.
  // This is very useful when we would like to use a
  // foreign memory allocation process, for example,
  // to initialize types our class is inept at
  // initializing.
  TAKEOVER_FROM_FOREIGN_INSTANCE
};

// One example:
//
// Let's say we have:
//
// class Canvas {
//    private:
//       PixelData *image_;
//       ImageStats image_statistics_;
//
//       Brush current_brush_;
// };
//
// ...
// Canvas my_canvas(...);
//
// Let's say we would like to instantiate another Canvas operating
// on the same data (image_), but which would have a different state,
// i.e. a different brush:
//
// Canvas other_canvas(SHARE_WITH_INSTANCE, &my_canvas);
//
// If, on the other hand, we would want a completely different canvas,
// sharing nothing with my_canvas, we would do:
//
// Canvas independent_canvas(COPY_FROM_INSTANCE, &my_canvas);
//
// The independent canvas would start with an image which is the same
// as that of my_canvas, but operations and modifications on independent_canvas
// would have nothing to do with my_canvas.
//
// The advantage of this method over doing:
// Canvas my_canvas(...);
//
// Canvas other_canvas;
// other_canvas.copyFrom(&my_canvas);
//
// are that sometimes it doesn't make sense to have a default constructor,
// since there is not enough information to construct a class, thus having
// a default constructor aggravates maintaining the methods in the class,
// debugging the class and the class caller and it delegates the
// responsibility of constructors to non-constructor methods. Furthermore,
// it hurts code readability and invites bugs caused by the classes being
// in a semi-initialized state.
//
// To blanket declare an array of non-default-constructor classes,
// you would do:
//
// // Initializes 54 copies of my_canvas
// Canvas *copies = new (Canvas[54])(COPY_FROM_INSTANCE, &my_canvas);
//
// If this doesn't fit your purpose, you most likely want to declare
// an array of Canvas pointers, and instantiate each with 'new' .
//
// SHARE_WITH_FOREIGN_INSTANCE is used when it is too expensive to copy
// a foreign type in and you'd rather share the data.
// For example:
//
// Array2D<int> my_array(SHARE_WITH_FOREIGN_INSTANCE, 20, 30, buffer);
//
// would enable you to access a buffer as if it were a 20x30 array.
#endif  // UTIL_ARRAY_ARRAY_COMMON_H_
