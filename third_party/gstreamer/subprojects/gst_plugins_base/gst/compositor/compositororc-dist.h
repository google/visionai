
/* autogenerated from compositororc.orc */

#ifndef _COMPOSITORORC_H_
#define _COMPOSITORORC_H_

#include "third_party/glib/glib/glib.h"

#ifdef __cplusplus
extern "C" {
#endif



#ifndef _ORC_INTEGER_TYPEDEFS_
#define _ORC_INTEGER_TYPEDEFS_
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
typedef int8_t orc_int8;
typedef int16_t orc_int16;
typedef int32_t orc_int32;
typedef int64_t orc_int64;
typedef uint8_t orc_uint8;
typedef uint16_t orc_uint16;
typedef uint32_t orc_uint32;
typedef uint64_t orc_uint64;
#define ORC_UINT64_C(x) UINT64_C(x)
#elif defined(_MSC_VER)
typedef signed __int8 orc_int8;
typedef signed __int16 orc_int16;
typedef signed __int32 orc_int32;
typedef signed __int64 orc_int64;
typedef unsigned __int8 orc_uint8;
typedef unsigned __int16 orc_uint16;
typedef unsigned __int32 orc_uint32;
typedef unsigned __int64 orc_uint64;
#define ORC_UINT64_C(x) (x##Ui64)
#define inline __inline
#else
#include <limits.h>
typedef signed char orc_int8;
typedef short orc_int16;
typedef int orc_int32;
typedef unsigned char orc_uint8;
typedef unsigned short orc_uint16;
typedef unsigned int orc_uint32;
#if INT_MAX == LONG_MAX
typedef long long orc_int64;
typedef unsigned long long orc_uint64;
#define ORC_UINT64_C(x) (x##ULL)
#else
typedef long orc_int64;
typedef unsigned long orc_uint64;
#define ORC_UINT64_C(x) (x##UL)
#endif
#endif
typedef union { orc_int16 i; orc_int8 x2[2]; } orc_union16;
typedef union { orc_int32 i; float f; orc_int16 x2[2]; orc_int8 x4[4]; } orc_union32;
typedef union { orc_int64 i; double f; orc_int32 x2[2]; float x2f[2]; orc_int16 x4[4]; } orc_union64;
#endif
#ifndef ORC_RESTRICT
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define ORC_RESTRICT restrict
#elif defined(__GNUC__) && __GNUC__ >= 4
#define ORC_RESTRICT __restrict__
#else
#define ORC_RESTRICT
#endif
#endif

#ifndef ORC_INTERNAL
#if defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define ORC_INTERNAL __attribute__((visibility("hidden")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#define ORC_INTERNAL __hidden
#elif defined (__GNUC__)
#define ORC_INTERNAL __attribute__((visibility("hidden")))
#else
#define ORC_INTERNAL
#endif
#endif

void compositor_orc_splat_u32 (guint32 * ORC_RESTRICT d1, int p1, int n);
void compositor_orc_memcpy_u32 (guint32 * ORC_RESTRICT d1, const guint32 * ORC_RESTRICT s1, int n);
void compositor_orc_blend_u8 (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_blend_argb (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_source_argb (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_blend_bgra (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_source_bgra (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_overlay_argb (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_overlay_argb_addition (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_overlay_bgra (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);
void compositor_orc_overlay_bgra_addition (guint8 * ORC_RESTRICT d1, int d1_stride, const guint8 * ORC_RESTRICT s1, int s1_stride, int p1, int n, int m);

#ifdef __cplusplus
}
#endif

#endif

