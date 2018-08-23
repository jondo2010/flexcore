cc_library(
    name = "adobe",
    srcs = [],
    hdrs = [
        "3rdparty/adobe/config.hpp",
        "3rdparty/adobe/forest.hpp",
        "3rdparty/adobe/reverse.hpp",
        "3rdparty/adobe/set_next.hpp",
    ],
    includes = ["3rdparty"],
)

cc_library(
    name = "flexcore",
    srcs = [
        "infrastructure.cpp",
        "extended/graph/graph.cpp",
        "utils/logging/logger.cpp",
        "utils/demangle.cpp",
        "extended/base_node.cpp",
        "extended/visualization/visualization.cpp",
        "scheduler/clock.cpp",
        "scheduler/cyclecontrol.cpp",
        "scheduler/parallelregion.cpp",
        "scheduler/parallelscheduler.cpp",
        "scheduler/serialschedulers.cpp",
    ],
    hdrs = [
        "infrastructure.hpp",
        "ports.hpp",
        "range/actions.hpp",

    ] + glob([
        "utils/**/*.hpp",
        "scheduler/**/*.hpp",
        "extended/**/*.hpp",
        "core/**/*.hpp",
        "pure/**/*.hpp",
    ]),
    includes = ["."],
    #include_prefix = "flexcore",
    #includes = ["flexcore"],
    deps =[
        ":adobe",
        "@boost//:core",
        "@boost//:uuid",
        "@boost//:graph",
        "@boost//:circular_buffer",
        "@boost//:log",
    ],
    copts = [
        "-std=c++14",
    ],
    linkopts = [
        #"-Wl,--no-undefined"
    ],
    #linkstatic = False,
    #alwayslink = True,
    visibility = ["//visibility:public"]
)