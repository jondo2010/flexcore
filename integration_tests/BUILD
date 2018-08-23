cc_binary(
    name = "dummy_executable",
    srcs = ["main.cpp"],
    #includes = ["../src"],
    copts = [
        "-std=c++14",
    ],
    deps = [
        "//flexcore",
        "@boost//:scope_exit",
    ]
)
