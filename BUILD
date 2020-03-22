package(
    default_visibility = ["//visibility:private"],
)

licenses(["notice"])

cc_library(
    name = "registry",
    srcs = [
        "registry_element.cc",
        "registry.cc",
    ],
    hdrs = [
        "type_traits.h",
        "registry_element.h",
        "registry.h",
    ],
    deps = [
        "//common:error_or",
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
        "@googletest//:gtest_main",
    ],
)
