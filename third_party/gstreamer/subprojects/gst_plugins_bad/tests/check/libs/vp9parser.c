/* Gstreamer
 * Copyright (C) 2020 Yeongjin Jeong <yeongjin.jeong@navercorp.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/codecparsers/gstvp9parser.h"

/* A super frame data */
static const guint8 vp9_frame_data[] = {
  0x84, 0x00, 0x40, 0x84, 0x60, 0x1f, 0xe0, 0x11, 0xfc, 0x86, 0x23, 0x30, 0x00,
  0x02, 0x80, 0x70, 0x69, 0x60, 0x00, 0x00, 0x7e, 0x3f, 0x01, 0x60, 0xce,
  0xa0, 0x11, 0xbc, 0x02, 0x76, 0xf0, 0xd0, 0xb8, 0x2d, 0xb6, 0x1a, 0xc1,
  0xff, 0x36, 0x8d, 0xad, 0x1d, 0xde, 0x8e, 0x29, 0x47, 0xdd, 0x39, 0x65,
  0xf3, 0xf9, 0x45, 0xbe, 0xdb, 0x5b, 0xee, 0xe7, 0x36, 0x83, 0xe6, 0xaa,
  0xba, 0xf3, 0x2b, 0xe7, 0xab, 0xda, 0x07, 0xa3, 0xf5, 0x06, 0x7a, 0x19,
  0xdf, 0x37, 0x0f, 0x59, 0xae, 0x01, 0x63, 0x41, 0xf3, 0x48, 0x8a, 0x8c,
  0xb4, 0x47, 0x99, 0xe5, 0xf0, 0x48, 0x31, 0xd5, 0x2d, 0xd3, 0x01, 0x9d,
  0x87, 0xf1, 0x19, 0x63, 0xf7, 0x7e, 0xc4, 0x12, 0xa8, 0x85, 0xf3, 0x1d,
  0xd9, 0xc6, 0xcd, 0x61, 0x31, 0xec, 0x09, 0x6d, 0xfc, 0x96, 0x57, 0x26,
  0x70, 0xcc, 0xe4, 0x9e, 0x31, 0x05, 0x40, 0xa6, 0xc0, 0x2b, 0x44, 0x7b,
  0x80, 0xf7, 0x30, 0xdc, 0xa3, 0xcc, 0x88, 0xf1, 0x0b, 0x0f, 0x6c, 0xe9,
  0x85, 0xe8, 0x1c, 0xf9, 0x3f, 0xff, 0x46, 0xa3, 0x39, 0xa5, 0xab, 0x03,
  0x20, 0xad, 0x42, 0xdb, 0xb0, 0x84, 0x39, 0xff, 0xc6, 0xf7, 0xe8, 0x3e,
  0xe4, 0x0e, 0x03, 0x29, 0xc0, 0x8d, 0xbf, 0xeb, 0xfd, 0x8c, 0x6b, 0x17,
  0x49, 0x66, 0xfa, 0xab, 0xfc, 0xb3, 0x23, 0xae, 0xad, 0x92, 0xa2, 0x8b,
  0xff, 0xec, 0x22, 0x7c, 0xc4, 0x08, 0xdc, 0x85, 0xad, 0x9f, 0x63, 0x62,
  0x75, 0x05, 0x32, 0x84, 0x19, 0x61, 0xc0, 0x8f, 0x1f, 0x25, 0xd4, 0x3c,
  0x7a, 0x4d, 0x03, 0x0c, 0xfa, 0xb6, 0x04, 0xad, 0x3f, 0xa7, 0x0f, 0x5a,
  0x08, 0xda, 0xc0, 0xd4, 0x1b, 0xf3, 0x64, 0xc9, 0xaa, 0xe6, 0x97, 0x49,
  0x17, 0xe6, 0x16, 0xaa, 0x7f, 0x0a, 0x35, 0xed, 0xac, 0x87, 0x9e, 0x16,
  0xea, 0x08, 0x05, 0xa4, 0xd5, 0xdd, 0x19, 0xc5, 0x8c, 0x3c, 0x6d, 0x79,
  0xe2, 0x05, 0x0f, 0x08, 0x54, 0x6d, 0x10, 0xda, 0x07, 0x92, 0x9b, 0xb5,
  0x29, 0xec, 0x8c, 0xeb, 0xc6, 0xcb, 0xd7, 0xff, 0x66, 0x37, 0x18, 0x81,
  0x53, 0x4d, 0x1c, 0xc3, 0x13, 0xdd, 0x46, 0x0b, 0xf8, 0x0f, 0x37, 0x95,
  0xab, 0x1a, 0xfc, 0xa7, 0xfb, 0xb5, 0x15, 0xe8, 0x8f, 0xca, 0x88, 0x78,
  0x35, 0x89, 0xe4, 0xfc, 0xf4, 0x57, 0x7c, 0x4f, 0x5d, 0xdb, 0x00, 0x55,
  0x75, 0xf7, 0x77, 0x38, 0x74, 0x8b, 0x4b, 0x8c, 0x22, 0x8e, 0xc6, 0x4e,
  0xaf, 0x4a, 0x08, 0xa2, 0x70, 0xec, 0xbe, 0xd4, 0x5c, 0x04, 0x5f, 0xad,
  0xbc, 0xbf, 0xfe, 0x24, 0xc2, 0xef, 0x53, 0x8e, 0x9e, 0xef, 0xe5, 0xea,
  0x1d, 0x4c, 0xbf, 0x72, 0x3e, 0xc9, 0xcf, 0xaa, 0xaf, 0x00, 0x33, 0x04,
  0xca, 0x06, 0xe8, 0xff, 0xf8, 0x6b, 0x9b, 0x8e, 0x91, 0x0b, 0x35, 0x55,
  0x4c, 0xd0, 0x48, 0x3e, 0xc0, 0xf9, 0xf7, 0xf5, 0x31, 0x48, 0x0a, 0xfc,
  0xb0, 0xa6, 0xe2, 0x87, 0x62, 0xee, 0x0e, 0xd3, 0x63, 0x1d, 0xd7, 0x61,
  0xeb, 0x1c, 0x55, 0x56, 0xd7, 0xec, 0x38, 0x88, 0xce, 0x0a, 0x6d, 0xff,
  0x70, 0x0c, 0x26, 0x7e, 0x6d, 0x6f, 0x38, 0xbd, 0xd0, 0xf3, 0xf4, 0xd7,
  0x2b, 0xcb, 0xaf, 0xab, 0x75, 0x7a, 0xc8, 0xaf, 0x33, 0x68, 0xdc, 0x70,
  0xec, 0xdc, 0x70, 0x31, 0xf8, 0x2f, 0xfb, 0x8e, 0xde, 0x12, 0xd3, 0x47,
  0x05, 0x67, 0xe4, 0x2b, 0x5e, 0xed, 0x8d, 0x0b, 0x9f, 0x19, 0x86, 0xb1,
  0x7a, 0x3c, 0x84, 0x23, 0x45, 0x38, 0xc5, 0x90, 0xee, 0x63, 0xc0, 0x3b,
  0x90, 0x0c, 0x04, 0x04, 0x93, 0x6f, 0x9e, 0x11, 0x5a, 0x5d, 0x48, 0x7c,
  0xd8, 0x58, 0x1c, 0x23, 0x58, 0x21, 0xb5, 0x0c, 0xc2, 0x3c, 0x2e, 0x5b,
  0x60, 0x3a, 0xa5, 0x7d, 0x08, 0xc2, 0x05, 0x78, 0x07, 0xbf, 0xb7, 0xed,
  0x7f, 0x45, 0xa5, 0xd7, 0x7e, 0xbe, 0xd3, 0x73, 0x26, 0x59, 0x49, 0x82,
  0xc3, 0xf0, 0x89, 0x62, 0x7d, 0xba, 0x64, 0x08, 0x88, 0xf2, 0x30, 0x4b,
  0x06, 0x0f, 0x8d, 0x40, 0xf4, 0x02, 0x7f, 0x6f, 0xa4, 0x2b, 0x23, 0xe2,
  0x1e, 0x48, 0xf1, 0xdc, 0xa5, 0x88, 0xcf, 0xe1, 0x8c, 0x0b, 0x05, 0x58,
  0x1a, 0x2b, 0x46, 0x78, 0xea, 0x9e, 0x1e, 0xf5, 0xae, 0x66, 0x6c, 0x40,
  0xd1, 0x14, 0x77, 0x06, 0x7a, 0x01, 0x2d, 0x1c, 0x99, 0xb7, 0x95, 0x0a,
  0x23, 0x24, 0x4e, 0x51, 0xa3, 0x59, 0x19, 0xd2, 0x4f, 0xe1, 0xf2, 0x6a,
  0xe2, 0x70, 0xdc, 0x26, 0x47, 0x4e, 0xb2, 0xc8, 0x80, 0x10, 0x45, 0x09,
  0x56, 0x14, 0x93, 0xee, 0x23, 0xe3, 0xf4, 0x48, 0xb2, 0xe0, 0x31, 0x16,
  0xfc, 0xfa, 0xf1, 0xf0, 0xcd, 0xee, 0x0c, 0x42, 0xe1, 0x2f, 0xda, 0x67,
  0xd7, 0x12, 0xea, 0xcb, 0xe5, 0xd7, 0x7b, 0x1c, 0xe4, 0xc2, 0xd2, 0x14,
  0xe6, 0x14, 0xcb, 0x7f, 0xc8, 0xe7, 0x49, 0xc9, 0x2b, 0x4d, 0x2b, 0xad,
  0x8a, 0xac, 0x2c, 0xb1, 0xac, 0x3d, 0xe3, 0x9b, 0x2a, 0xd2, 0x1e, 0x16,
  0xf4, 0x57, 0xad, 0x25, 0xe1, 0xbe, 0xbf, 0x45, 0x6e, 0xf6, 0xcb, 0x5a,
  0x5b, 0x72, 0x04, 0xb5, 0x9a, 0x86, 0x80, 0x06, 0x0d, 0x38, 0x6f, 0xbe,
  0x50, 0x64, 0xbe, 0x66, 0xde, 0x11, 0xfe, 0xc3, 0x38, 0xf9, 0x76, 0x65,
  0x2e, 0x2b, 0xf2, 0x6b, 0x3b, 0x61, 0x08, 0x65, 0x6a, 0xe7, 0xdf, 0x5c,
  0x9c, 0xca, 0xe1, 0x58, 0x41, 0xb8, 0x40, 0xff, 0x27, 0xf9, 0x95, 0x37,
  0x30, 0x3f, 0x98, 0x52, 0x27, 0x8a, 0xa1, 0x99, 0x80, 0x81, 0x2f, 0x60,
  0xfd, 0xab, 0x21, 0xb3, 0x0b, 0x55, 0x5b, 0x5d, 0xff, 0xbe, 0xba, 0x98,
  0x40, 0x17, 0x50, 0x58, 0x9b, 0x61, 0x4b, 0x49, 0x70, 0x20, 0x7d, 0xd4,
  0xac, 0xf1, 0x4f, 0x73, 0x0f, 0x4a, 0x8e, 0x11, 0x83, 0x86, 0x53, 0x1c,
  0xb1, 0xa6, 0x8f, 0xd7, 0xfb, 0xaf, 0x20, 0x09, 0x9e, 0x10, 0xcb, 0xa0,
  0x45, 0x51, 0x48, 0xae, 0xea, 0x8f, 0xbb, 0x94, 0x73, 0xaf, 0x6e, 0x8f,
  0x1d, 0xd0, 0xb7, 0xd5, 0x64, 0x11, 0xaa, 0xf6, 0x2e, 0x5f, 0xcc, 0x2f,
  0x06, 0x63, 0xe4, 0x24, 0x41, 0x66, 0x1d, 0x08, 0x7a, 0xe4, 0xf0, 0xe6,
  0xdc, 0x70, 0xad, 0xb4, 0xd7, 0x65, 0x7e, 0x76, 0x6f, 0x44, 0x66, 0x6b,
  0xfc, 0xa4, 0x92, 0x6b, 0x7e, 0x31, 0x9e, 0x9c, 0xd6, 0xc7, 0x4e, 0x91,
  0xab, 0xab, 0x04, 0x9e, 0x5f, 0x7d, 0x2d, 0x1b, 0x99, 0x10, 0x13, 0xc5,
  0x5c, 0xf4, 0xb4, 0x85, 0x47, 0x66, 0x54, 0xc8, 0x6c, 0x4b, 0x1e, 0x99,
  0x2a, 0x87, 0x3e, 0x19, 0xf2, 0x97, 0xf8, 0xfa, 0xbb, 0x10, 0xbc, 0x28,
  0x1f, 0x68, 0x06, 0x3d, 0xf7, 0xe6, 0xfb, 0x4b, 0x8f, 0x2a, 0xa8, 0x50,
  0xe0, 0xb4, 0x12, 0x6b, 0x5c, 0x62, 0x96, 0x14, 0xd6, 0xb3, 0xd5, 0x6d,
  0xc1, 0xc8, 0x4f, 0x49, 0xd5, 0x4e, 0x18, 0x33, 0x52, 0x6d, 0x23, 0xb6,
  0x3c, 0xc9, 0x21, 0xd9, 0xd9, 0xf0, 0x0b, 0x1f, 0xc8, 0x08, 0x1b, 0x15,
  0xf6, 0xff, 0xf1, 0x95, 0x7f, 0xa9, 0xf1, 0x01, 0xa6, 0xe0, 0x85, 0x8a,
  0x45, 0x04, 0xd9, 0x07, 0x6e, 0x16, 0x44, 0x83, 0x94, 0x1b, 0xe4, 0x25,
  0x91, 0xe1, 0x20, 0xdd, 0xf2, 0x77, 0xb5, 0xc5, 0x4a, 0xbf, 0x6d, 0x6d,
  0x40, 0xd1, 0x40, 0x99, 0x29, 0xb6, 0x8b, 0x9b, 0x10, 0xd0, 0xf8, 0x4c,
  0x92, 0x95, 0x83, 0x0b, 0x8e, 0x22, 0x06, 0xf6, 0xbf, 0x93, 0x39, 0x89,
  0xf7, 0x7c, 0xf2, 0x6c, 0x03, 0x16, 0x0d, 0xd3, 0x80, 0x7f, 0x48, 0x18,
  0xbc, 0x49, 0x32, 0x70, 0x43, 0x3d, 0xdc, 0xe6, 0x96, 0x86, 0xef, 0x39,
  0x50, 0x36, 0xfe, 0xd0, 0xe0, 0xaa, 0x83, 0xaf, 0xac, 0x0c, 0x70, 0x4c,
  0x91, 0x93, 0x5f, 0xfc, 0xfb, 0xc2, 0xce, 0x69, 0xec, 0xcf, 0x06, 0x44,
  0xc8, 0xc9, 0x73, 0x7d, 0x07, 0x4d, 0xa8, 0x76, 0xe0, 0xf3, 0x5b, 0xa1,
  0xd8, 0xf5, 0x04, 0x00, 0xaf, 0x53, 0x15, 0x35, 0x75, 0xce, 0xcd, 0x26,
  0x6e, 0xd7, 0xc4, 0x23, 0x13, 0xf3, 0xe7, 0x47, 0x9b, 0x76, 0x70, 0x32,
  0x74, 0xaa, 0x9c, 0x9a, 0x71, 0xe7, 0xe3, 0x2a, 0xbb, 0xe5, 0xfb, 0x7e,
  0x34, 0x34, 0xb6, 0xae, 0xc2, 0xd5, 0x55, 0x6c, 0xf4, 0x6b, 0x4f, 0x7a,
  0x04, 0xa9, 0x8e, 0x63, 0xbf, 0x8a, 0x92, 0x9b, 0xf1, 0xf8, 0xc9, 0xc5,
  0x57, 0x5b, 0x2d, 0x08, 0x2f, 0xcf, 0x2d, 0xdd, 0x9f, 0x61, 0x12, 0x9f,
  0x0b, 0xd9, 0x58, 0xc6, 0x83, 0xd3, 0x5f, 0x9c, 0x99, 0x6f, 0xe2, 0xc8,
  0x36, 0x99, 0xaa, 0xe5, 0xda, 0x33, 0xee, 0x9a, 0xc4, 0xa5, 0x00, 0x78,
  0x91, 0x8a, 0xf7, 0x17, 0x9a, 0xf1, 0x9d, 0x89, 0x48, 0x84, 0xcb, 0x71,
  0x8c, 0xfe, 0x63, 0xbc, 0x9d, 0xbe, 0x2c, 0xf2, 0xd0, 0xa3, 0x8d, 0xe0,
  0xdf, 0xd1, 0x00, 0xe4, 0x85, 0xab, 0x61, 0x97, 0x45, 0x63, 0x57, 0xeb,
  0xff, 0x17, 0xf9, 0xc5, 0xa8, 0x22, 0x96, 0x99, 0xea, 0xa0, 0x7e, 0x4f,
  0xd8, 0x8c, 0xac, 0xd1, 0x4d, 0x79, 0xc6, 0x45, 0xa6, 0xb9, 0x69, 0x29,
  0x4a, 0xd3, 0xa5, 0x8d, 0xd5, 0xed, 0x3f, 0x78, 0xe8, 0x9b, 0x4e, 0xff,
  0xda, 0xb1, 0x9f, 0x13, 0x53, 0x11, 0x00, 0xb5, 0x00, 0x36, 0x00, 0xc0,
  0xe2, 0x45, 0x29, 0x4c, 0xe9, 0xd6, 0x7f, 0xd6, 0xc6, 0x2e, 0x12, 0x6f,
  0xea, 0xeb, 0xcc, 0xae, 0x89, 0x05, 0xbe, 0x6b, 0x6e, 0x6a, 0x8a, 0x73,
  0xd5, 0xba, 0x28, 0x66, 0x3e, 0x35, 0xdc, 0x1f, 0x77, 0x30, 0x74, 0x52,
  0x80, 0x51, 0x6e, 0xaa, 0x69, 0x2c, 0x4a, 0x5c, 0x69, 0x3d, 0x40, 0x7b,
  0x34, 0x1e, 0xe0, 0xd7, 0xf2, 0xf8, 0x8e, 0x27, 0xa1, 0xb2, 0x49, 0x82,
  0x46, 0x4b, 0x61, 0x13, 0xf3, 0xab, 0x4a, 0xf7, 0x59, 0xbb, 0x03, 0xe9,
  0xcf, 0x4d, 0x6b, 0x19, 0xa4, 0x6f, 0xba, 0x23, 0xf2, 0x5e, 0xb9, 0x8b,
  0x75, 0xf4, 0x56, 0x48, 0x42, 0x64, 0xed, 0x78, 0xc7, 0xb3, 0x41, 0x60,
  0x86, 0x00, 0x41, 0x0c, 0xc0, 0x3f, 0xc0, 0x23, 0xf8, 0x14, 0x4e, 0x00,
  0x00, 0x05, 0x70, 0x59, 0x00, 0x80, 0x00, 0x56, 0xfe, 0x01, 0x95, 0x4b,
  0x59, 0xf5, 0xee, 0xf0, 0x12, 0xf1, 0xd9, 0x43, 0xcb, 0xa2, 0xcb, 0x49,
  0x63, 0xa2, 0x30, 0xea, 0xd6, 0xc6, 0x38, 0x34, 0x29, 0x26, 0x4d, 0xfa,
  0x69, 0xf5, 0x31, 0xe1, 0x35, 0x24, 0x42, 0x18, 0x38, 0x46, 0x4c, 0xad,
  0x1e, 0x2f, 0x99, 0x2f, 0x30, 0x5c, 0x6f, 0x8a, 0xdc, 0xf6, 0x72, 0xb4,
  0x23, 0xde, 0xf6, 0x7c, 0x8a, 0x1a, 0x4d, 0xa8, 0x0c, 0xa3, 0x49, 0x10,
  0xf6, 0x94, 0xcf, 0x21, 0x21, 0x4d, 0xcf, 0x0a, 0x9b, 0xd0, 0x54, 0xd5,
  0xa3, 0xdf, 0x2f, 0x26, 0x17, 0xfd, 0x79, 0xe8, 0x2d, 0x98, 0xcf, 0xb9,
  0x57, 0x24, 0xa3, 0x0e, 0x7f, 0xaf, 0x3f, 0x15, 0xb9, 0x33, 0xdd, 0xfd,
  0x8a, 0x7e, 0xb9, 0x1b, 0xda, 0xaa, 0x0c, 0xfe, 0xf9, 0xbf, 0xaf, 0x01,
  0x7a, 0x0e, 0x4b, 0xc3, 0x02, 0x7a, 0x54, 0xb4, 0x5d, 0x70, 0x90, 0xa0,
  0x83, 0xf0, 0x46, 0x2f, 0x38, 0x94, 0x1c, 0xb6, 0xb8, 0x7f, 0xfe, 0x3f,
  0xcd, 0xa5, 0xb6, 0xb7, 0x1b, 0x0f, 0x83, 0x4c, 0x83, 0xce, 0x44, 0xb0,
  0xb4, 0x06, 0xaa, 0x62, 0x1e, 0xd9, 0xf4, 0x4c, 0x9e, 0x8a, 0x80, 0x5e,
  0xe3, 0xd3, 0xf2, 0xb3, 0x12, 0xe7, 0xa3, 0xb9, 0xb4, 0xb0, 0xab, 0xdf,
  0x24, 0xd2, 0x4f, 0x4c, 0xc9, 0x1f, 0x16, 0x08, 0x84, 0x83, 0x02, 0x62,
  0x83, 0xb1, 0x92, 0x2a, 0x15, 0x5e, 0xda, 0xbe, 0x3b, 0x00, 0xc9, 0x35,
  0x05, 0xd6, 0x00, 0xc9
};

GST_START_TEST (test_vp9_parse_superframe)
{
  GstVp9Parser *parser;
  GstVp9SuperframeInfo superframe_info;

  parser = gst_vp9_parser_new ();

  memset (&superframe_info, 0, sizeof (superframe_info));

  assert_equals_int (gst_vp9_parser_parse_superframe_info (parser,
          &superframe_info, vp9_frame_data, sizeof (vp9_frame_data)),
      GST_VP9_PARSER_OK);

  assert_equals_int (superframe_info.bytes_per_framesize, 2);
  assert_equals_int (superframe_info.frames_in_superframe, 2);
  assert_equals_int (superframe_info.superframe_index_size, 6);

  assert_equals_int (superframe_info.frame_sizes[0], 1333);
  assert_equals_int (superframe_info.frame_sizes[1], 214);

  gst_vp9_parser_free (parser);
}

GST_END_TEST;

static Suite *
vp9parsers_suite (void)
{
  Suite *s = suite_create ("VP9 Parser library");

  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_vp9_parse_superframe);

  return s;
}

GST_CHECK_MAIN (vp9parsers);
