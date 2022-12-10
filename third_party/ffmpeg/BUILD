load("@bazel_tools//tools/cpp:cc_flags_supplier.bzl", "cc_flags_supplier")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "ffmpeg_src",
    srcs = glob([
        "ffbuild/**",
        "fftools/**",
        "compat/**",
        "tests/**",
    ]),
)

filegroup(
    name = "libavcodec_src",
    srcs = glob([
        "libavcodec/**",
    ]),
)

filegroup(
    name = "libavdevice_src",
    srcs = glob([
        "libavdevice/**",
    ]),
)

filegroup(
    name = "libavfilter_src",
    srcs = glob([
        "libavfilter/**",
    ]),
)

filegroup(
    name = "libavformat_src",
    srcs = glob([
        "libavformat/**",
    ]),
)

filegroup(
    name = "libavutil_src",
    srcs = glob([
        "libavutil/**",
    ]),
)

filegroup(
    name = "libswresample_src",
    srcs = glob([
        "libswresample/**",
    ]),
)

filegroup(
    name = "libswscale_src",
    srcs = glob([
        "libswscale/**",
    ]),
)

filegroup(
    name = "libpostproc_src",
    srcs = glob([
        "libpostproc/**",
    ]),
)

filegroup(
    name = "libavresample_src",
    srcs = glob([
        "libavresample/**",
    ]),
)

filegroup(
    name = "makefiles",
    srcs = glob([
        "**/*.mak",
        "**/Makefile",
    ]),
)

cc_library(
    name = "ffmpeg_headers",
    hdrs = glob(["lib*/*.h"]),
    includes = ["."],
)

cc_flags_supplier(
    name = "cc_flags",
)

cc_library(
    name = "gen_ffmpeg_lib",
    srcs = [
        ":gen_ffmpeg",
    ],
    linkstatic = 1,
    deps = [":ffmpeg_headers"],
)

genrule(
    name = "gen_ffmpeg",
    srcs = [
        "build_bazel.sh",
        "configure",
        ":ffmpeg_src",
        ":libavcodec_src",
        ":libavdevice_src",
        ":libavfilter_src",
        ":libavformat_src",
        ":libavresample_src",
        ":libavutil_src",
        ":libpostproc_src",
        ":libswresample_src",
        ":libswscale_src",
        ":makefiles",
    ],
    outs = [
        "ffmpeg",
        "libavcodec.so.58",
        "libavdevice.so.58",
        "libavfilter.so.7",
        "libavformat.so.58",
        "libavutil.so.56",
        "libswresample.so.3",
        "libswscale.so.5",
	"libavutil/avconfig.h",
    ],
    cmd = (
        " CC='$(CC)' CFLAGS='$(CC_FLAGS)'" +
        " CXX='$(CC)' CXXFLAGS='$(CC_FLAGS)'" +
        " CPPFLAGS='$(CC_FLAGS)' LDFLAGS='$(CC_FLAGS)'" +
        " AR='$(AR)' NM='$(NM)' STRIP='$(STRIP)' " +
        "$(location build_bazel.sh) $$(dirname $(location :build_bazel.sh)) " +
        "$(location ffmpeg) "
    ),
    # The CI system requires that packaged artifacts are under bazel-bin/
    # rather than bazel-genfiles/.
    output_to_bindir = True,
    tags = [
        "linux_only",
    ],
    toolchains = [
        "@bazel_tools//tools/cpp:current_cc_toolchain",
        ":cc_flags",
    ],
    tools = [
        ":build_bazel.sh",
    ],
    visibility = [
        "//image/data/manifests/devkit:__pkg__",
        "//image/data/manifests/edge:__pkg__",
        "//scripts/build:__pkg__",
    ],
)
