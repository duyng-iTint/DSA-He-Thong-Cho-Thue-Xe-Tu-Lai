cmake_minimum_required(VERSION 3.16)

project(DSA-He-Thong-Cho-Thue-Xe-Tu-Lai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    src
    src/core
    src/core/structures
    src/core/model
    src/core/services
    src/core/algorithms
)

# Gom toan bo file .cpp trong src/core thanh thu vien dung chung
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp)
add_library(core STATIC ${CORE_SOURCES})

# Chuong trinh demo MC1
add_executable(demo_mc1 src/presentation/demo_mc1.cpp)
target_link_libraries(demo_mc1 core)

# Chuong trinh test cua Thanh vien 1
add_executable(test_mc1 test/test_thanhvien1.cpp)
target_link_libraries(test_mc1 core)

# Chuong trinh benchmark MC1
add_executable(benchmark_mc1 benchmark/benchmark_mc1.cpp)
target_link_libraries(benchmark_mc1 core)