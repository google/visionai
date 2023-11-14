load("@rules_python//python:defs.bzl", "py_library", "py_test")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "transformer",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":asset_indexing_transformer",
        ":corpus_indexing_transformer",
        ":embedding_transformer",
        ":ocr_transformer",
        ":speech_transformer",
        ":transform_progress",
        ":transformer_factory",
        ":transformer_interface",
    ],
)

py_library(
    name = "transform_progress",
    srcs = [
        "transform_progress.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/lva:client",
    ],
)

py_test(
    name = "transform_progress_test",
    srcs = ["transform_progress_test.py"],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/lva:client",
        "//visionai/python/net:channel",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "embedding_transformer",
    srcs = [
        "embedding_transformer.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
        ":transformer_interface",
        "//visionai/python/gapic:visionai_v1",
    ],
)

py_test(
    name = "embedding_transformer_test",
    srcs = ["embedding_transformer_test.py"],
    srcs_version = "PY3",
    deps = [
        ":embedding_transformer",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "asset_indexing_transformer",
    srcs = [
        "asset_indexing_transformer.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
        ":transformer_interface",
        "//visionai/python/gapic:visionai_v1",
    ],
)

py_test(
    name = "asset_indexing_transformer_test",
    srcs = ["asset_indexing_transformer_test.py"],
    srcs_version = "PY3",
    deps = [
        ":asset_indexing_transformer",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "corpus_indexing_transformer",
    srcs = [
        "corpus_indexing_transformer.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
        ":transformer_interface",
        "//visionai/python/gapic:visionai_v1",
    ],
)

py_test(
    name = "corpus_indexing_transformer_test",
    srcs = ["corpus_indexing_transformer_test.py"],
    srcs_version = "PY3",
    deps = [
        ":corpus_indexing_transformer",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "transformer_interface",
    srcs = [
        "transformer_interface.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
    ],
)

py_library(
    name = "speech_transformer",
    srcs = [
        "speech_transformer.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
        ":transformer_interface",
        "//visionai/python/gapic:visionai_v1",
    ],
)

py_test(
    name = "speech_transformer_test",
    srcs = ["speech_transformer_test.py"],
    srcs_version = "PY3",
    deps = [
        ":speech_transformer",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "ocr_transformer",
    srcs = [
        "ocr_transformer.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":transform_progress",
        ":transformer_interface",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/lva:client",
        "//visionai/python/lva:graph",
        "//visionai/python/net:channel",
        "//visionai/python/ops:gen_ops",
    ],
)

py_test(
    name = "ocr_transformer_test",
    srcs = ["ocr_transformer_test.py"],
    srcs_version = "PY3",
    deps = [
        ":ocr_transformer",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/lva:client",
        "//visionai/python/net:channel",
        "//visionai/python/testing:test",
    ],
)

py_library(
    name = "transformer_factory",
    srcs = [
        "transformer_factory.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":embedding_transformer",
        ":ocr_transformer",
        ":speech_transformer",
        ":transformer_interface",
    ],
)

py_test(
    name = "transformer_factory_test",
    srcs = ["transformer_factory_test.py"],
    srcs_version = "PY3",
    deps = [
        ":embedding_transformer",
        ":ocr_transformer",
        ":speech_transformer",
        ":transformer_factory",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/lva:client",
        "//visionai/python/testing:test",
    ],
)
