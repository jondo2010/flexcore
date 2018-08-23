cc_test(
    name = "test_executable",
    srcs = [
        "examples.cpp",

        "core/test_connection.cpp",
        "core/test_connectables.cpp",
        "core/test_traits.cpp",

        "logging/test_logging.cpp",

        "nodes/test_buffer.cpp",
        "nodes/test_generic.cpp",
        "nodes/test_event_nodes.cpp",
        "nodes/test_state_nodes.cpp",
        "nodes/test_moving.cpp",

        "extended/graph/test_graph.cpp",
        "extended/nodes/test_base_node.cpp",
        "extended/nodes/test_infrastructure.cpp",
        "extended/nodes/test_region_worker_node.cpp",
        "extended/nodes/test_terminal_node.cpp",
        "extended/ports/test_node_aware.cpp",
        "extended/ports/test_region_buffer.cpp",

        "pure/test_events.cpp",
        "pure/test_moving.cpp",
        "pure/test_mux_ports.cpp",
        "pure/test_state_sinks.cpp",

        "range/test_range.cpp",

        #"serialisation/test_deserializer.cpp",

        #"settings/test_settings.cpp",
        #"settings/test_setting_backend.cpp",

        "scheduler/TestClock.cpp",
        "scheduler/test_cyclecontrol.cpp",
        "scheduler/test_parallel_region.cpp",
        "scheduler/test_parallelscheduler.cpp",
        "scheduler/test_serialscheduler.cpp",

        #"util/test_generic_container.cpp",

        "runner.cpp",

        "pure/sink_fixture.hpp",
        "core/movable_connectable.hpp",
        "nodes/owning_node.hpp",
    ],
    deps = [
        "//flexcore",
        "@boost//:test",
    ],
    copts = [
        "-std=c++14",
    ],
    linkstatic = False
)
