load("@rules_cc//cc:defs.bzl", "cc_library")

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "cpp-utils",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["inc/*.h"]),
    includes = ["inc"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_spdlog//:spdlog",
        "@com_google_absl//absl/debugging:failure_signal_handler",
        "@com_google_absl//absl/debugging:stacktrace",
        "@com_google_absl//absl/debugging:symbolize",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)
