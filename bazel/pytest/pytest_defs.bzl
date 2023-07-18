"""Bazel rules wrapping py_test.
"""

load("@rules_python//python:defs.bzl", "py_test")
load("@pytest_deps//:requirements.bzl", "requirement")

def pytest_test(name, srcs, deps = [], args = [], **kwargs):
    """
        Call pytest
    """
    py_test(
        name = name,
        srcs = [
            "//bazel/pytest:pytest_wrapper.py",
        ] + srcs,
        main = "//bazel/pytest:pytest_wrapper.py",
        args = [
            "--capture=no",
        ] + args + ["$(location :%s)" % x for x in srcs],
        deps = deps + [
            requirement("pytest"),
        ],
        **kwargs
    )
