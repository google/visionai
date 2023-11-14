# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

"""
Vision AI Bazel Dependencies.
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def visionai_deps(is_gob_source_only = False):
    """Load dependencies for the visionai."""
    maybe(
        http_archive,
        name = "rules_foreign_cc",
        sha256 = "c2cdcf55ffaf49366725639e45dedd449b8c3fe22b54e31625eb80ce3a240f1e",
        strip_prefix = "rules_foreign_cc-0.1.0",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.1.0.zip",
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        strip_prefix = "grpc-1.55.1",
        urls = ["https://github.com/grpc/grpc/archive/v1.55.1.zip"],
    )

    maybe(
        http_archive,
        name = "com_github_google_glog",
        strip_prefix = "glog-0.6.0",
        urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
        sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    )

    maybe(
        http_archive,
        name = "com_github_gflags_gflags",
        strip_prefix = "gflags-2.2.2",
        sha256 = "19713a36c9f32b33df59d1c79b4958434cb005b5b47dc5400a7a4b078111d9b5",
        url = "https://github.com/gflags/gflags/archive/v2.2.2.zip",
    )

    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        url = "https://github.com/google/benchmark/archive/v1.5.1.tar.gz",
        strip_prefix = "benchmark-1.5.1",
        sha256 = "23082937d1663a53b90cb5b61df4bcc312f6dee7018da78ba00dd6bd669dfef2",
    )

    maybe(
        http_archive,
        name = "com_github_google_googletest",
        strip_prefix = "googletest-release-1.11.0",
        url = "https://github.com/google/googletest/archive/release-1.11.0.tar.gz",
        sha256 = "b4870bf121ff7795ba20d20bcdd8627b8e088f2d1dab299a031c1034eddc93d5",
    )

    maybe(
        http_archive,
        name = "com_google_protobuf",
        sha256 = "209385d3c08252e320196b628584c8007f849f9ec8a26c2796a886345ee58bb6",
        strip_prefix = "protobuf-2dca62f7296e5b49d729f7384f975cecb38382a0",
        urls = [
            "https://storage.googleapis.com/grpc-bazel-mirror/github.com/protocolbuffers/protobuf/archive/2dca62f7296e5b49d729f7384f975cecb38382a0.tar.gz",
            "https://github.com/protocolbuffers/protobuf/archive/2dca62f7296e5b49d729f7384f975cecb38382a0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "5366d7e7fa7ba0d915014d387b66d0d002c03236448e1ba9ef98122c13b35c36",
        strip_prefix = "abseil-cpp-20230125.3",
        urls = [
            "https://storage.googleapis.com/grpc-bazel-mirror/github.com/abseil/abseil-cpp/archive/20230125.3.tar.gz",
            "https://github.com/abseil/abseil-cpp/archive/20230125.3.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "absl_py",
        sha256 = "0fb3a4916a157eb48124ef309231cecdfdd96ff54adf1660b39c0d4a9790a2c0",
        strip_prefix = "abseil-py-1.4.0",
        urls = [
            "https://github.com/abseil/abseil-py/archive/refs/tags/v1.4.0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_tencent_rapidjson",
        build_file = "//third_party:rapidjson.BUILD",
        strip_prefix = "rapidjson-b7734d97c0c011632367f5e3510916828da1346c",
        urls = [
            "https://github.com/Tencent/rapidjson/archive/b7734d97c0c011632367f5e3510916828da1346c.zip",
        ],
        sha256 = "44ba38febb4a433e19e1fd139a4b12e3b3d4da0c4d31f0f7d7ca9221dac276ef",
    )

    maybe(
        http_archive,
        name = "com_census_instrumentation_cpp",
        strip_prefix = "opencensus-cpp-7268fc9e2722245cb20322866a93c6fb6c5c5c80",
        urls = [
            "https://github.com/census-instrumentation/opencensus-cpp/archive/7268fc9e2722245cb20322866a93c6fb6c5c5c80.zip",
        ],
        sha256 = "601a8987d443448f7389e3cb2e998eca32ecbc3d1d3e8c3d19b71d4d1da83af7",
    )

    maybe(
        http_archive,
        name = "com_github_googleapis_googleapis",
        sha256 = "53cdd6a10d4c57dd9f14cf3cca96d9204f62e05dc39a8028148fad1aea620b48",
        strip_prefix = "googleapis-10b972be1028d510e96a8b92e0cd2c79ccf54495",
        urls = ["https://github.com/googleapis/googleapis/archive/10b972be1028d510e96a8b92e0cd2c79ccf54495.tar.gz"],
    )

    maybe(
        http_archive,
        name = "com_github_googleapis_google_cloud_cpp",
        sha256 = "168c38219feb5a2c6b81bec5960cd067f6cda3daa83cd9761fa04f27d2b78f17",
        strip_prefix = "google-cloud-cpp-2.1.0",
        url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.1.0.tar.gz",
    )

    maybe(
        http_archive,
        name = "zlib",
        urls = [
            "http://zlib.net/zlib-1.2.12.tar.gz",
            "https://zlib.net/fossils/zlib-1.2.12.tar.gz",
        ],
        sha256 = "91844808532e5ce316b3c010929493c0244f3d37593afd6de04f71821d5136d9",
        strip_prefix = "zlib-1.2.12",
        build_file = Label("//bazel:zlib.BUILD"),
    )

    maybe(
        http_archive,
        name = "bzip2",
        urls = [
            "https://src.fedoraproject.org/repo/pkgs/bzip2/bzip2-1.0.6.tar.gz/00b516f4704d4a7cb50a1d97e6e8e15b/bzip2-1.0.6.tar.gz",
            "http://anduin.linuxfromscratch.org/LFS/bzip2-1.0.6.tar.gz",
            "https://fossies.org/linux/misc/bzip2-1.0.6.tar.gz",
            "http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz",
        ],
        sha256 = "a2848f34fcd5d6cf47def00461fcb528a0484d8edef8208d6d2e2909dc61d9cd",
        strip_prefix = "bzip2-1.0.6",
        build_file = Label("//bazel:bzip2.BUILD"),
    )

    maybe(
        native.new_local_repository,
        name = "linux_ffmpeg",
        build_file = "//bazel:ffmpeg.BUILD",
        path = "/usr",
    )
    maybe(
        native.new_local_repository,
        name = "linux_libjpeg",
        build_file = "//bazel:libjpeg.BUILD",
        path = "/usr",
    )
    maybe(
        native.new_local_repository,
        name = "linux_x264",
        build_file = "//bazel:x264.BUILD",
        path = "/usr",
    )
    maybe(
        native.new_local_repository,
        name = "pcre",
        build_file = "//third_party/pcre:BUILD",
        path = "third_party/pcre",
    )
    maybe(
        native.new_local_repository,
        name = "libffi",
        build_file = "//third_party/libffi:BUILD",
        path = "third_party/libffi",
    )
    maybe(
        native.new_local_repository,
        name = "glib",
        build_file = "//third_party/glib:BUILD",
        path = "third_party/glib",
    )

    maybe(
        http_archive,
        name = "com_google_mediapipe",
        strip_prefix = "mediapipe-0.8.11",
        repo_mapping = {
            "@com_github_glog_glog": "@com_github_google_glog",
        },
        urls = [
            "https://github.com/google/mediapipe/archive/v0.8.11.tar.gz",
        ],
        patches = [
            "@//bazel:com_google_mediapipe.diff",
        ],
        patch_args = ["-p1"],
        sha256 = "5b331a46b459900d0789967f9e26e4a64d1466bc1e74dd0712eb3077358c5473",
    )

    _TENSORFLOW_GIT_COMMIT = "af1d5bc4fbb66d9e6cc1cf89503014a99233583b"
    _TENSORFLOW_SHA256 = "f85a5443264fc58a12d136ca6a30774b5bc25ceaf7d114d97f252351b3c3a2cb"
    maybe(
        http_archive,
        name = "org_tensorflow",
        urls = [
            "https://github.com/tensorflow/tensorflow/archive/%s.tar.gz" % _TENSORFLOW_GIT_COMMIT,
        ],
        patches = [
            "@//bazel:org_tensorflow_compatibility_fixes.diff",
            # Diff is generated with a script, don't update it manually.
            "@//bazel:org_tensorflow_custom_ops.diff",
        ],
        patch_args = [
            "-p1",
        ],
        strip_prefix = "tensorflow-%s" % _TENSORFLOW_GIT_COMMIT,
        sha256 = _TENSORFLOW_SHA256,
    )

    maybe(
        http_archive,
        name = "com_googlesource_code_re2",
        sha256 = "9f3b65f2e0c78253fcfdfce1754172b0f97ffdb643ee5fd67f0185acf91a3f28",
        strip_prefix = "re2-2022-06-01",
        url = "https://github.com/google/re2/archive/refs/tags/2022-06-01.zip",
    )

    # This requires opencv2 to be installed on your system.
    maybe(
        native.new_local_repository,
        name = "linux_opencv",
        build_file = "//bazel:opencv.BUILD",
        path = "/usr/local/opencv",
    )

    maybe(
        http_archive,
        name = "opencv4",
        build_file = "@//bazel:BUILD.opencv4",
        strip_prefix = "opencv-4.x",
        urls = ["https://github.com/opencv/opencv/archive/refs/heads/4.x.zip"],
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc_proto",
        sha256 = "365c0c8d9ae90aa11a8bd7fa870d8962a13600cfc851405acd5327907f9f4c56",
        strip_prefix = "grpc-proto-d653c6d98105b2af937511aa6e46610c7e677e6e",
        url = "https://github.com/grpc/grpc-proto/archive/d653c6d98105b2af937511aa6e46610c7e677e6e.zip",
        patches = [
            # The patch is needed since grpc bazel rules enforce package to be in the source folder.
            "@//bazel:com_github_grpc_grpc_proto.diff",
        ],
        patch_args = [
            "-p1",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_curl",
        build_file = "//:third_party/curl.BUILD",
        strip_prefix = "curl-f141b0bbf78c818e0fd6ea6782ec718e4a9055c0",
        urls = [
            "https://github.com/curl/curl/archive/f141b0bbf78c818e0fd6ea6782ec718e4a9055c0.zip",
        ],
    )

    # OpenCensus depends on jupp0r/prometheus-cpp
    maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        strip_prefix = "prometheus-cpp-76470b3ec024c8214e1f4253fb1f4c0b28d3df94",
        urls = ["https://github.com/jupp0r/prometheus-cpp/archive/76470b3ec024c8214e1f4253fb1f4c0b28d3df94.zip"],
    )

# Explicitly add some Tensorflow deps.
def tensorflow_deps():
    """
    Additional dependencies for Tensorflow (not covered in tf_workspace).
    """
    http_archive(
        name = "io_bazel_rules_closure",
        sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
        strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
            "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",  # 2019-06-13
        ],
    )
