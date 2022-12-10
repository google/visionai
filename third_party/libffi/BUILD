# Copyright 2012 Google Inc. All Rights Reserved.
#
# Description:
#   libffi, a Portable Foreign Function Interface Library

licenses(["notice"])

exports_files(["LICENSE"])

# Source code common to all platforms.
COMMON_SRCS = [
    "src/closures.c",
    "src/debug.c",
    "src/java_raw_api.c",
    "src/prep_cif.c",
    "src/raw_api.c",
    "src/types.c",
]

# Platform-specific sourcers and headers.
K8_SRCS = COMMON_SRCS + [
    "src/x86/asmnames.h",
    "src/x86/ffi.c",
    "src/x86/ffi64.c",
    "src/x86/ffiw64.c",
    "src/x86/freebsd.S",
    "src/x86/internal.h",
    "src/x86/internal64.h",
    "src/x86/sysv.S",
    "src/x86/unix64.S",
    "src/x86/win64.S",
]

K8_HDRS = [
    "k8-linux/fficonfig.h",
    "k8-linux/include/ffi.h",
    "k8-linux/include/ffi_cfi.h",
    "k8-linux/include/ffi_common.h",
    "k8-linux/include/ffitarget.h",
    "src/dlmalloc.c",
]

K8_INCLUDES = [
    "k8-linux",
    "k8-linux/include",
]

cc_library(
    name = "libffi",
    srcs = K8_SRCS,
    copts = [
        # libffi-3.3-rc0 uses variable length arrays for closures on all
        # platforms.
        "-Wno-vla",
        # libffi does not check the result of ftruncate.
        "-Wno-unused-result",
        # libffi casts from void* (64 bits) to long int (32 bits) on Windows.
        "-Wno-void-pointer-to-int-cast",
    ],
    includes = K8_INCLUDES,
    textual_hdrs = K8_HDRS,
    visibility = ["//visibility:public"],
)
