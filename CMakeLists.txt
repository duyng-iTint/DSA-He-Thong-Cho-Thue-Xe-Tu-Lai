cmake_minimum_required(VERSION 3.16)
project(DSAHeThongChoThueXe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
add_library(core STATIC ${CORE_SOURCES})

add_executable(CarRentalApp
    src/presentation/MainRF3.cpp
    src/presentation/InputHandler.cpp
    test/Tests.cpp
    benchmark/Benchmark.cpp
)
target_include_directories(CarRentalApp PRIVATE src)
target_link_libraries(CarRentalApp PRIVATE core)

# Adapter de web presentation goi truc tiep cac DSA Core C++.
add_executable(RentalWebCore web/core_bridge.cpp)
target_link_libraries(RentalWebCore PRIVATE core)

add_executable(RentalMC2Demo src/presentation/mainMC1.cpp)
target_link_libraries(RentalMC2Demo PRIVATE core)

add_executable(RentalMC1Demo src/presentation/demo_mc1.cpp)
target_link_libraries(RentalMC1Demo PRIVATE core)

add_executable(BenchmarkMC1 benchmark/benchmark_mc1.cpp)
target_include_directories(BenchmarkMC1 PRIVATE src/core/model src/core/structures src/core/services)
target_link_libraries(BenchmarkMC1 PRIVATE core)
