cmake_minimum_required(VERSION 3.12)
project(GCIS_REPAIR C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force optimizations and all warnings
add_compile_options(-O3)

# Required by malloc_count for symbols extraction on unix platforms
find_library(DL_LIBRARY dl)

# Include paths
include_directories(
    include
    include/malloc_count-0.7.1
)

# Common shared library dependencies across your executables
set(COMMON_SOURCES
    src/gcis/gcis.cpp
    src/repair/repair.cpp
    include/bitvector.cpp
    include/malloc_count-0.7.1/malloc_count.c
)

# Target 1: Fast production executable
add_executable(hybrid_fast src/main.cpp ${COMMON_SOURCES})
if(DL_LIBRARY)
    target_link_libraries(hybrid_fast PRIVATE ${DL_LIBRARY})
endif()

# Target 2: Benchmarking / Experiment profiling executable
add_executable(hybrid_experiment src/main.cpp ${COMMON_SOURCES})
target_compile_definitions(hybrid_experiment PRIVATE EXPERIMENT)
if(DL_LIBRARY)
    target_link_libraries(hybrid_experiment PRIVATE ${DL_LIBRARY})
endif()