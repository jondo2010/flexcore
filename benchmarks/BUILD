cc_test(
    name = "flexcore_benchmark",
    srcs = [
        "benchmarkfunctions.cpp",
	    "range_benchmarks.cpp",
	    "port_benchmarks.cpp",

        "../tests/nodes/owning_node.hpp",
    ],
    deps = [
        "@com_google_benchmark//:benchmark",
        "//flexcore",
    ],
    copts = [
        "-std=c++14",
    ],
)