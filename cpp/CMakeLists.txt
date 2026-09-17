cmake_minimum_required(VERSION 3.20)

project(HFTAlphaEngine
    VERSION 1.0
    DESCRIPTION "High Frequency Market Microstructure C++ Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization
if(MSVC)
    add_compile_options(/O2)
else()
    add_compile_options(-O3 -march=native)
endif()

# Core library
add_library(hft_engine
    src/order_book.cpp
    src/microstructure.cpp
    src/event_processor.cpp
    src/data/lobster_parser.cpp
    src/order_registry.cpp
    src/order_aware_book.cpp
    src/ofi_engine.cpp
)

target_include_directories(hft_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Main executable
add_executable(hft_demo
    src/main.cpp
)

target_link_libraries(hft_demo
    PRIVATE hft_engine
)

# Tests
enable_testing()

add_executable(order_book_test
    tests/test_order_book.cpp
)

target_link_libraries(order_book_test
    PRIVATE hft_engine
)

add_test(
    NAME OrderBookTest
    COMMAND order_book_test
)
# Event Processor Benchmark
add_executable(event_processor_benchmark
    benchmarks/benchmark_event_processor.cpp
)

target_link_libraries(event_processor_benchmark
    PRIVATE hft_engine
)

# Latency Distribution Benchmark
add_executable(latency_benchmark
    benchmarks/latency_benchmark.cpp
)

target_link_libraries(latency_benchmark
    PRIVATE hft_engine
)

add_executable(throughput_benchmark
    benchmarks/throughput_benchmark.cpp
)

target_link_libraries(throughput_benchmark
    PRIVATE hft_engine
)

add_library(fast_hft_engine
    src/fast_order_book.cpp
)

target_include_directories(fast_hft_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(fast_order_book_benchmark
    benchmarks/fast_order_book_benchmark.cpp
)

target_link_libraries(fast_order_book_benchmark
    PRIVATE fast_hft_engine
)

add_executable(fast_order_book_test
    tests/test_fast_order_book.cpp
)

target_link_libraries(fast_order_book_test
    PRIVATE fast_hft_engine
)

add_test(
    NAME FastOrderBookTest
    COMMAND fast_order_book_test
)

#######
add_executable(lobster_replay_benchmark
    benchmarks/lobster_replay_benchmark.cpp
)

target_link_libraries(lobster_replay_benchmark
    PRIVATE
        hft_engine
        fast_hft_engine
)
######
add_executable(lobster_validation
    benchmarks/lobster_validation.cpp
)

target_link_libraries(lobster_validation
    PRIVATE
        hft_engine
)

########
add_executable(lobster_alpha_benchmark
    benchmarks/lobster_alpha_benchmark.cpp
)

target_link_libraries(lobster_alpha_benchmark
    PRIVATE
        fast_hft_engine
        hft_engine
)


########
add_executable(aapl_feature_generator
    benchmarks/aapl_feature_generator.cpp
)

target_link_libraries(aapl_feature_generator
    PRIVATE
        fast_hft_engine
        hft_engine
)




# Google Benchmark (optional)
find_package(benchmark QUIET)

if(benchmark_FOUND)

    add_executable(order_book_benchmark
        benchmarks/benchmark_order_book.cpp
    )

    target_link_libraries(order_book_benchmark
        PRIVATE
            hft_engine
            benchmark::benchmark
            benchmark::benchmark_main
    )

endif()

# Benchmark
find_package(benchmark QUIET)

if(benchmark_FOUND)

    add_executable(order_book_benchmark
        benchmarks/benchmark_order_book.cpp
    )

    target_link_libraries(order_book_benchmark
        PRIVATE
            hft_engine
            benchmark::benchmark
            benchmark::benchmark_main
    )

endif()