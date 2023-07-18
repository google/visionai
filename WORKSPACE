workspace(name = "visionai")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "8e968b5fcea1d2d64071872b12737bbb5514524ee5f0a4f54f5920266c261acb",
    url = "https://github.com/bazelbuild/rules_go/releases/download/v0.28.0/rules_go-v0.28.0.zip",
)

http_archive(
    name = "bazel_gazelle",
    sha256 = "62ca106be173579c0a167deb23358fdfe71ffa1e4cfdddf5582af26520f1c66f",
    url = "https://github.com/bazelbuild/bazel-gazelle/releases/download/v0.23.0/bazel-gazelle-v0.23.0.tar.gz",
)

load("@io_bazel_rules_go//go:deps.bzl", "go_rules_dependencies")

go_rules_dependencies()

load("//:deps.bzl", "go_dependencies")

# gazelle:repository_macro deps.bzl%go_dependencies
go_dependencies()

load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")

gazelle_dependencies(go_sdk = "go_sdk")

# Load visionai-sdk specific dependencies.
load("//bazel:visionai_deps.bzl", "visionai_deps")

visionai_deps()

# Requirements for @com_github_googleapis_google_cloud_cpp.
load("@com_github_googleapis_google_cloud_cpp//bazel:google_cloud_cpp_deps.bzl", "google_cloud_cpp_deps")

google_cloud_cpp_deps()

# Google APIs imports. Required to build googleapis.
load("@com_github_googleapis_googleapis//:repository_rules.bzl", "switched_rules_by_language")

# Enable language specific rules.
# All non-enabled language-specific rules will default to no-op implementations.
switched_rules_by_language(
    name = "com_google_googleapis_imports",
    cc = True,
    go = True,
    grpc = True,
    python = True,
)

# Requirements for @com_github_grpc_grpc.
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

load("@com_github_jupp0r_prometheus_cpp//bazel:repositories.bzl", "prometheus_cpp_repositories")

prometheus_cpp_repositories()

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

load("@rules_python//python:pip.bzl", "pip_install")

pip_install(
    name = "pytest_deps",
    requirements = "//bazel/pytest:requirements.txt",
)

load("@pytest_deps//:requirements.bzl", "install_deps")

install_deps()

# comments
