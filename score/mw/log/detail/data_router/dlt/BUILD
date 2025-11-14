cc_library(
    name = "dlt",
    srcs = ["dlt_wraper.cpp"],
    hdrs = ["dlt_wrapper.hpp", "idlt_wrapper.hpp"],
    visibility = ["//score/mw/log:__subpackages__"],
    deps = [
        "@dlt//:dlt_lib",
    ],
)

cc_library(
    name = "dlt_mock",
    srcs = ["dlt_wrapper_mock.cpp"],
    hdrs = ["dlt_wrapper_mock.hpp", "idlt_wrapper.hpp"],
    visibility = ["//score/mw/log:__subpackages__"],
    deps=
    [
        "@googletest//:gtest_main",
    ]
)
