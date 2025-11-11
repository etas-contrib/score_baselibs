
cc_library(
    name = "remote_dlt_recorder_factory",
    srcs = [
        "remote_dlt_recorder_factory.cpp",
        "remote_dlt_recorder_factory.h",
        "data_router_recorder.cpp",
        "data_router_recorder.h",
    ],
    hdrs = [
        "data_router_recorder.h",
        "remote_dlt_recorder_factory.h",
    ],
    deps = [
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@score_baselibs//score/mw/log/detail:dlt_content_formatting",
        "@score_baselibs//score/mw/log/detail:statistics_reporter",
        # "@score_baselibs//score/os/utils:high_resolution_steady_clock",
        "@score_baselibs//score/static_reflection_with_serialization/serialization",
        "@score_baselibs//score/mw/log/detail:log_recorder_factory",
        "@score_baselibs//score/language/futurecpp",
        "@dlt//:dlt_lib"
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
