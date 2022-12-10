# Description:
#   Perl-compatible regular expressions library.
#
#-*- mode: python; -*-
package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

exports_files(["LICENSE"])

PCRE8_SRCS = glob(
    ["pcre_*.c"],
    exclude = [
        "pcre_printint.c",
        "pcre_jit_test.c",
    ],
)

PCRE16_SRCS = glob(
    ["pcre16_*.c"],
    exclude = [
        "pcre16_printint.c",
    ],
)

PCRE32_SRCS = glob(
    ["pcre32_*.c"],
    exclude = [
        "pcre32_printint.c",
    ],
)

PCRE_HDRS = [
    "include/pcre.h",
    "include/pcreposix.h",
    "pcre.h",
    "pcreposix.h",
]

PCRE_INTERNAL_HDRS = [
    "config.h",
    "pcre_internal.h",
    "ucp.h",
]

PCRE_COMMON_DEFS = [
    "-DHAVE_CONFIG_H",
    "-DLINK_SIZE=2",
    "-DMATCH_LIMIT=10000000",
    "-DMATCH_LIMIT_RECURSION=1000",
    "-DNO_RECURSE",
    "-DPARENS_NEST_LIMIT=50",
    "-DSUPPORT_UTF",
    "-DSUPPORT_UCP",
    "-DHAVE_DIRENT_H=1",
]

PCRE8_DEFS = PCRE_COMMON_DEFS + [
    "-DSUPPORT_PCRE8=1",
    # configure defines these only for the 8-bit version. A comment in
    # configure states that they only matter for pcregrep, which is 8-bit only,
    # but we mirror the config.h defines exactly.
    "-DHAVE_LONG_LONG=1",
    "-DHAVE_STRING=1",
    "-DHAVE_STRTOQ=1",
    "-DHAVE_UNSIGNED_LONG_LONG=1",
]

PCRE16_DEFS = PCRE_COMMON_DEFS + [
    "-DSUPPORT_PCRE16=1",
]

PCRE32_DEFS = PCRE_COMMON_DEFS + [
    "-DSUPPORT_PCRE32=1",
]

cc_library(
    name = "internal_hdrs",
    hdrs = PCRE_INTERNAL_HDRS + PCRE_HDRS,
    copts = PCRE8_DEFS,
    visibility = ["//visibility:private"],
)

cc_library(
    name = "pcre",
    srcs = PCRE8_SRCS,
    hdrs = PCRE_HDRS,
    copts = PCRE8_DEFS,
    includes = ["include"],
    deps = [":internal_hdrs"],
)

cc_library(
    name = "pcre16_hdrs",
    hdrs = PCRE_INTERNAL_HDRS + PCRE_HDRS + PCRE8_SRCS,
    copts = PCRE16_DEFS,
    visibility = ["//visibility:private"],
)

cc_library(
    name = "pcre16",
    srcs = PCRE16_SRCS,
    hdrs = PCRE_HDRS,
    copts = PCRE16_DEFS,
    includes = ["include"],
    deps = [":pcre16_hdrs"],
)

cc_library(
    name = "pcre32_hdrs",
    hdrs = PCRE_INTERNAL_HDRS + PCRE_HDRS + PCRE8_SRCS,
    copts = PCRE32_DEFS,
    visibility = ["//visibility:private"],
)

cc_library(
    name = "pcre32",
    srcs = PCRE32_SRCS,
    hdrs = PCRE_HDRS,
    copts = PCRE32_DEFS,
    includes = ["include"],
    deps = [":pcre32_hdrs"],
)

cc_library(
    name = "pcreposix",
    srcs = ["pcreposix.c"],
    hdrs = PCRE_HDRS,
    copts = PCRE8_DEFS,
    deps = [
        ":internal_hdrs",
        ":pcre",
    ],
)

cc_library(
    name = "pcrecpp",
    srcs = ["pcrecpp.cc"],
    hdrs = [
        "pcre_stringpiece.h",
        "pcrecpp.h",
        "pcrecpp_internal.h",
        "pcrecpparg.h",
    ],
    copts = PCRE_COMMON_DEFS,
    deps = [
        ":internal_hdrs",
        ":pcre",
    ],
)

# Testing of PCRE library

# pcretest supports simultaneously testing all three versions (8, 16, and 32).
PCRETEST_DEFS = PCRE_COMMON_DEFS + [
    "-DSUPPORT_PCRE8=1",
    "-DSUPPORT_PCRE16=1",
    "-DSUPPORT_PCRE32=1",
]

cc_library(
    name = "pcretest_hdrs",
    hdrs = PCRE_INTERNAL_HDRS + PCRE_HDRS + [
        "pcre_printint.c",
        "pcre_tables.c",
        "pcre_ucd.c",
    ],
    copts = PCRETEST_DEFS,
    visibility = ["//visibility:private"],
)

cc_binary(
    name = "pcretest",
    srcs = [
        "pcre16_printint.c",
        "pcre32_printint.c",
        "pcre_internal.h",
        "pcre_printint.c",
        "pcreposix.c",
        "pcretest.c",
    ],
    copts = PCRETEST_DEFS,
    visibility = ["//visibility:private"],
    deps = [
        ":pcre",
        ":pcre16",
        ":pcre32",
        ":pcretest_hdrs",
    ],
)

cc_binary(
    name = "pcregrep",
    srcs = ["pcregrep.c"],
    copts = PCRE8_DEFS,
    visibility = ["//visibility:private"],
    deps = [
        ":internal_hdrs",
        ":pcre",
    ],
)

filegroup(
    name = "testdata",
    srcs = [
        "RunTest",
        "RunGrepTest",
    ] + glob(["testdata/*"]),
    visibility = ["//visibility:private"],
)

sh_test(
    name = "pcre_tests",
    size = "small",
    srcs = ["RunTest.sh"],
    args = [
        # We have lower (non-default) recursion limit and nesting parenthesis
        # limit. Test 2, 11 and 14 fail with them.
        "~2",
        "~11",
        "~14",
        # We turn off UTF8 validity check, which also happens to disable UTF16
        # and UTF32 validity checks. Tests 15, 18, 24 and 26 check that certain
        # expressions are invalid UTF*, and therefore must be disabled.
        "~15",
        "~18",
        "~24",
        "~26",
    ],
    data = [
        ":pcregrep",
        ":pcretest",
        ":testdata",
    ],
    visibility = ["//visibility:private"],
)
