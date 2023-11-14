load("@rules_python//python:defs.bzl", "py_library")
load("//bazel/pytest:pytest_defs.bzl", "pytest_test")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

_GAPIC_COMMON_DEPS = [
    "@com_github_grpc_grpc//src/python/grpcio/grpc:grpcio",
    "@com_google_protobuf//:protobuf_python",
]

py_library(
    name = "visionai_v1",
    srcs = glob(
        ["visionai/visionai_v1/**/*.py"],
    ),
    deps = _GAPIC_COMMON_DEPS,
)

pytest_test(
    name = "visionai_tests",
    size = "small",
    timeout = "long",
    srcs = glob(["visionai/tests/unit/gapic/visionai_v1/*.py"]),
    python_version = "PY3",
    tags = [
        "nozapfhahn",
    ],
    deps = [":visionai_v1"],
)
