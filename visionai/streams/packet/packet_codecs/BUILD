package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "packet_codecs",
    hdrs = [
        "packet_codecs.h",
    ],
    deps = [
        ":codec_selector",
    ],
)

cc_library(
    name = "codec_selector",
    hdrs = ["codec_selector.h"],
    deps = [
        ":gstreamer_buffer_packet_codec",
        ":int_packet_codec",
        ":protobuf_packet_codec",
        ":raw_image_packet_codec",
        ":signal_packet_codec",
        ":string_packet_codec",
        ":type_traits",
        "//visionai/types:gstreamer_buffer",
    ],
)

cc_library(
    name = "packet_codec_base",
    hdrs = [
        "packet_codec_base.h",
    ],
    deps = [
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "protobuf_packet_codec",
    hdrs = [
        "protobuf_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "gstreamer_buffer_packet_codec",
    hdrs = [
        "gstreamer_buffer_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "int_packet_codec",
    hdrs = [
        "int_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "raw_image_packet_codec",
    hdrs = [
        "raw_image_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/types:raw_image",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "signal_packet_codec",
    hdrs = [
        "signal_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/types:signal",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "string_packet_codec",
    hdrs = [
        "string_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "no_copy_packet_codec",
    hdrs = [
        "no_copy_packet_codec.h",
    ],
    deps = [
        ":packet_codec_base",
        "//visionai/types:no_copy",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "type_traits",
    hdrs = ["type_traits.h"],
)
