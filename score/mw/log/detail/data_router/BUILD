cc_library(
    name ="data_router_recorder",
    srcs = [
        "data_router_recorder.cpp",
    ],
    hdrs = [
        "data_router_recorder.h",
    ],
    deps = [
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail/data_router/dlt",
    ]
)

cc_library(
    name = "remote_dlt_recorder_factory",
    srcs = [
        "remote_dlt_recorder_factory.cpp",
        "remote_dlt_recorder_factory.h",
    ],
    hdrs = [
        "remote_dlt_recorder_factory.h",
    ],
    deps = [
        ":data_router_recorder",
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@score_baselibs//score/mw/log/detail:log_recorder_factory",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail/data_router/dlt",
    ],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)


cc_binary(
    name = "remote_test",
    deps = [
        "@score_baselibs//score/mw/log",
    ],
    copts = [
        "-std=c++17",
        "-Wall",
        "-Werror",
    ],
    visibility = ["//visibility:private"],
    srcs = ["main.cpp"],
)

cc_test(
    name = "unit_test",
    srcs = [
        "data_router_recorder_test.cpp",
    ],
    deps =
    [
        ":data_router_recorder",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail/data_router/dlt:dlt_mock",
        "@googletest//:gtest_main",
    ]
)
