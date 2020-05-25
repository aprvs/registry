package(
    default_visibility = ["//visibility:private"],
)

licenses(["notice"])

cc_library(
    name = "registry",
    srcs = [
        "registry.cc",
    ],
    hdrs = [
        "registry.h",
    ],
    deps = [
        "//common:error_or",
        "//common:type_traits",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "registry_test",
    srcs = [
        "registry_test.cc",
    ],
    deps = [
        ":registry",
        "@com_googletest//:gtest_main",
    ],
)
