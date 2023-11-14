load("@rules_python//python:defs.bzl", "py_library")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "warehouse",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/warehouse/transformer",
        "//visionai/python/warehouse/utils",
    ],
)
