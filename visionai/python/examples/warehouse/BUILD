load("@rules_python//python:defs.bzl", "py_binary")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_binary(
    name = "get_corpus",
    srcs = [
        "get_corpus.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "@absl_py//absl:app",
    ],
)

py_binary(
    name = "transform_single_asset",
    srcs = [
        "transform_single_asset.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/warehouse/transformer:asset_indexing_transformer",
        "//visionai/python/warehouse/transformer:ocr_transformer",
        "//visionai/python/warehouse/transformer:speech_transformer",
        "//visionai/python/warehouse/transformer:transformer_factory",
        "//visionai/python/warehouse/utils:vod_asset",
        "//visionai/python/warehouse/utils:vod_corpus",
        "@absl_py//absl:app",
    ],
)

py_binary(
    name = "transform_gcs_files",
    srcs = [
        "transform_gcs_files.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/streams:client",
        "//visionai/python/warehouse/transformer:asset_indexing_transformer",
        "//visionai/python/warehouse/transformer:ocr_transformer",
        "//visionai/python/warehouse/transformer:speech_transformer",
        "//visionai/python/warehouse/transformer:transformer_factory",
        "//visionai/python/warehouse/utils:vod_asset",
        "//visionai/python/warehouse/utils:vod_corpus",
        "//visionai/python/warehouse/utils:vod_index_endpoint",
        "@absl_py//absl:app",
    ],
)
