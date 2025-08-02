cmake_minimum_required(VERSION 3.16)
project(Cerberus VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -Wextra")

# Source files
set(CORE_SOURCES
    src/core/bitboard.cpp
    src/core/position.cpp
    src/core/move.cpp
    src/core/magic.cpp
    src/core/move_generator.cpp
    src/core/types.cpp
    src/core/zobrist.cpp
)

set(SEARCH_SOURCES
    src/search/search.cpp
    src/search/transposition_table.cpp
    src/search/move_ordering.cpp
    src/search/mcts.cpp
    src/search/multithreading.cpp
    src/search/thread.cpp
)

set(EVALUATION_SOURCES
    src/evaluation/evaluation.cpp
    src/evaluation/dynamic_eval.cpp
    src/evaluation/material.cpp
    src/evaluation/positional.cpp
    src/evaluation/pawn_structure.cpp
    src/evaluation/nnue.cpp
    src/evaluation/nnue/network.cpp
    src/evaluation/nnue/nnue_accumulator.cpp
    src/evaluation/nnue/nnue_misc.cpp
    src/evaluation/nnue/features/half_ka_v2_hm.cpp
)

set(UCI_SOURCES
    src/uci/uci_interface.cpp
    src/uci/command_parser.cpp
)

# Main executable
add_executable(cerberus
    src/main.cpp
    ${CORE_SOURCES}
    ${SEARCH_SOURCES}
    ${EVALUATION_SOURCES}
    ${UCI_SOURCES}
)

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(cerberus Threads::Threads)

# Include directories
target_include_directories(cerberus PRIVATE include)
