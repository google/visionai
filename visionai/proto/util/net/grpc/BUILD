load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

# gazelle:ignore

proto_library(
    name = "connection_options_proto",
    srcs = ["connection_options.proto"],
    deps = ["@com_google_protobuf//:duration_proto"],
)

cc_proto_library(
    name = "connection_options_cc_proto",
    deps = [":connection_options_proto"],
)
