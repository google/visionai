package(
    default_visibility = [
        "//cloud/ai/visionai:__subpackages__",
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "streams_control_grpc_client",
    srcs = [
        "streams_control_grpc_client.cc",
    ],
    hdrs = [
        "streams_control_grpc_client.h",
    ],
    deps = [
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net:exponential_backoff",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_grpc",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "lva_control_grpc_client",
    srcs = [
        "lva_control_grpc_client.cc",
    ],
    hdrs = [
        "lva_control_grpc_client.h",
    ],
    deps = [
        "//visionai/util/net:exponential_backoff",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_grpc",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "resource_util",
    srcs = [
        "resource_util.cc",
    ],
    hdrs = [
        "resource_util.h",
    ],
    deps = [
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/util:random_string",
        "//visionai/util:time_util",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "resource_util_test",
    srcs = ["resource_util_test.cc"],
    deps = [
        ":resource_util",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "picked_notification",
    hdrs = [
        "picked_notification.h",
    ],
    deps = ["@com_google_absl//absl/synchronization"],
)

cc_test(
    name = "picked_notification_test",
    srcs = ["picked_notification_test.cc"],
    deps = [
        ":picked_notification",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "read_write_channel",
    hdrs = [
        "read_write_channel.h",
        "read_write_channel_internal.h",
    ],
    deps = [
        ":picked_notification",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "read_write_channel_test",
    srcs = ["read_write_channel_test.cc"],
    deps = [
        ":picked_notification",
        ":read_write_channel",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "worker_v1",
    hdrs = [
        "worker_v1.h",
    ],
    deps = [
        "//visionai/streams/util:worker",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "worker_v1_test",
    srcs = ["worker_v1_test.cc"],
    deps = [
        ":worker_v1",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "control",
    srcs = [
        "control.cc",
    ],
    hdrs = [
        "control.h",
    ],
    deps = [
        ":descriptors",
        ":resource_util",
        ":streaming_control_grpc_client",
        ":streams_control_grpc_client",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "streaming_control_grpc_client",
    srcs = [
        "streaming_control_grpc_client.cc",
    ],
    hdrs = [
        "streaming_control_grpc_client.h",
    ],
    deps = [
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util:time_util",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "streaming_send_packets_grpc_client",
    srcs = ["streaming_send_packets_grpc_client.cc"],
    hdrs = ["streaming_send_packets_grpc_client.h"],
    deps = [
        ":descriptors",
        ":resource_util",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/streams/packet",
        "//visionai/util:time_util",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "mock_streaming_service",
    hdrs = [
        "mock_streaming_service.h",
    ],
    deps = [
        "//visionai/testing/grpc:mock_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "streaming_receive_events_grpc_client",
    srcs = ["streaming_receive_events_grpc_client.cc"],
    hdrs = ["streaming_receive_events_grpc_client.h"],
    deps = [
        ":constants",
        ":resource_util",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util:time_util",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "streaming_receive_packets_grpc_v1_client",
    srcs = ["streaming_receive_packets_grpc_v1_client.cc"],
    hdrs = ["streaming_receive_packets_grpc_v1_client.h"],
    deps = [
        ":constants",
        ":descriptors",
        ":resource_util",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/streams/packet",
        "//visionai/util:time_util",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "mock_streams_service",
    hdrs = ["mock_streams_service.h"],
    deps = [
        "//visionai/testing/grpc:mock_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_proto",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "descriptors",
    hdrs = [
        "descriptors.h",
    ],
    deps = ["@com_google_absl//absl/time"],
)

cc_library(
    name = "packet_sender",
    srcs = [
        "packet_sender.cc",
    ],
    hdrs = [
        "packet_sender.h",
    ],
    deps = [
        ":constants",
        ":control",
        ":descriptors",
        ":resource_util",
        ":streaming_send_packets_grpc_client",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/streams/packet",
        "//visionai/streams/util:worker",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "channel_lease_renewal_task",
    srcs = [
        "channel_lease_renewal_task.cc",
    ],
    hdrs = [
        "channel_lease_renewal_task.h",
    ],
    deps = [
        ":constants",
        ":control",
        ":descriptors",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "event_update",
    srcs = [
        "event_update.cc",
    ],
    hdrs = [
        "event_update.h",
    ],
    deps = [
        ":resource_util",
        "//visionai/util:time_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "event_update_test",
    srcs = ["event_update_test.cc"],
    deps = [
        ":event_update",
        ":resource_util",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "event_update_receiver",
    srcs = [
        "event_update_receiver.cc",
    ],
    hdrs = [
        "event_update_receiver.h",
    ],
    deps = [
        ":constants",
        ":control",
        ":event_update",
        ":read_write_channel",
        ":streaming_receive_events_grpc_client",
        ":worker_v1",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/status:status_builder",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "mock_event_update_receiver",
    hdrs = [
        "mock_event_update_receiver.h",
    ],
    deps = [
        ":event_update_receiver",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "packet_receiver",
    srcs = [
        "packet_receiver.cc",
    ],
    hdrs = [
        "packet_receiver.h",
    ],
    deps = [
        ":channel_lease_renewal_task",
        ":constants",
        ":control",
        ":descriptors",
        ":read_write_channel",
        ":streaming_receive_packets_grpc_v1_client",
        ":worker_v1",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/streams/packet",
        "//visionai/streams/util:worker",
        "//visionai/util:ring_buffer",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "mock_packet_receiver",
    hdrs = [
        "mock_packet_receiver.h",
    ],
    deps = [
        ":packet_receiver",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_packet_sender",
    hdrs = [
        "mock_packet_sender.h",
    ],
    deps = [
        ":packet_sender",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "constants",
    hdrs = [
        "constants.h",
    ],
    deps = ["@com_google_absl//absl/time"],
)
