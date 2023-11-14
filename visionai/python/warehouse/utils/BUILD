load("@rules_python//python:defs.bzl", "py_library", "py_test")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "utils",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":vod_asset",
        ":vod_corpus",
        ":vod_index_endpoint",
    ],
)

py_library(
    name = "vod_asset",
    srcs = [
        "vod_asset.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/warehouse/transformer:asset_indexing_transformer",
        "//visionai/python/warehouse/transformer:transform_progress",
        "//visionai/python/warehouse/transformer:transformer_interface",
    ],
)

py_test(
    name = "vod_asset_test",
    srcs = ["vod_asset_test.py"],
    srcs_version = "PY3",
    deps = [
        ":vod_asset",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "vod_corpus",
    srcs = [
        "vod_corpus.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/warehouse/transformer:corpus_indexing_transformer",
    ],
)

py_test(
    name = "vod_corpus_test",
    srcs = ["vod_corpus_test.py"],
    srcs_version = "PY3",
    deps = [
        ":vod_corpus",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "vod_index_endpoint",
    srcs = [
        "vod_index_endpoint.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
    ],
)

py_test(
    name = "vod_index_endpoint_test",
    srcs = ["vod_index_endpoint_test.py"],
    srcs_version = "PY3",
    deps = [
        ":vod_index_endpoint",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)
