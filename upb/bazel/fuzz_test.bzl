"""fuzz_test() macro for upb"""

load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//devtools/build_cleaner/skylark:build_defs.bzl", "register_extension_info")

def _fuzz_test_impl(
        name,
        visibility,
        regression_srcs = [],
        regression_deps = None,
        impl_srcs = [],
        impl_hdrs = [],
        test_srcs = [],
        impl_deps = [],
        **kwargs):
    util_dep = []
    base_tags = kwargs.get("tags", [])
    other_kwargs = {k: v for k, v in kwargs.items() if k != "tags"}

    test_deps = ["//testing/fuzzing:fuzztest", "@googletest//:gtest", "@googletest//:gtest_main"]
    if not regression_deps:
        regression_deps = ["@googletest//:gtest", "@googletest//:gtest_main"]

    if impl_srcs or impl_hdrs:
        cc_library(
            name = name + "_impl",
            srcs = impl_srcs,
            hdrs = impl_hdrs,
            deps = impl_deps,
            testonly = 1,
            visibility = visibility,
            tags = base_tags,
        )
        util_dep = [":" + name + "_impl"]

    if regression_srcs:
        cc_test(
            name = name + "_regression_test",
            srcs = regression_srcs,
            deps = depset([str(d) for d in regression_deps + impl_deps + util_dep]).to_list(),
            tags = base_tags,
            visibility = visibility,
            **other_kwargs
        )

    pass

# fuzz_test() is a symbolic macro for upb fuzz tests.
#
# Its main purpose is to helpfully bundle three targets into one:
#   1. A fuzz test target (running with --config=fuzztest).
#   2. A regression test target (running as a normal unit test).
#   3. A utility library shared by both.
#
# This bundling allows the fuzz utility to be used by both the fuzz test and the
# regression test, makes it easy to apply custom flags to the fuzz test target,
# and enables easy exclusion of the fuzz test from OSS builds (via copybara).
#
# The macro separates sources and dependencies into three groups:
# - test_: for the main fuzz test executable.
# - regression_: for the regression tests.
# - util_: for utility functions shared by both.
#
# This separation allows build_cleaner to accurately manage dependencies for each =
# generated target.
fuzz_test = macro(
    implementation = _fuzz_test_impl,
    attrs = {
        "test_srcs": attr.label_list(default = [], configurable = False),
        "impl_srcs": attr.label_list(default = [], configurable = False),
        "impl_hdrs": attr.label_list(default = [], configurable = False),
        "impl_deps": attr.label_list(default = [], configurable = False),
        "regression_deps": attr.label_list(default = [], configurable = False),
        "regression_srcs": attr.label_list(default = [], configurable = False),
    },
)

register_extension_info(
    extension = fuzz_test,
    label_regex_map = {
        "impl_deps": "deps:{extension_name}_impl",
        "regression_deps": "deps:{extension_name}_regression_test",
    },
)
