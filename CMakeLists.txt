cmake_minimum_required(VERSION 3.16)
project(DSAHeThongChoThueXe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Thu vien core: tu dong lay MOI file .cpp trong src/core (gom ca UnifiedSystem.cpp).
# KHONG dua main.cpp / CliApp.cpp / test vao day (se bi nhieu ham main).
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
add_library(core STATIC ${CORE_SOURCES})

# ===== CHUONG TRINH TONG (MC1 + MC2 + RF1 + RF2 + RF3) =====
add_executable(cli_app
    src/presentation/main.cpp
    src/presentation/CliApp.cpp)
target_link_libraries(cli_app PRIVATE core)

# ===== Demo rieng cua ban RF3 (renTalSystem + InputHandler) - giu nguyen =====
add_executable(CarRentalApp
    src/presentation/MainRF3.cpp
    src/presentation/InputHandler.cpp
    test/Tests.cpp
    benchmark/Benchmark.cpp)
target_include_directories(CarRentalApp PRIVATE src)
target_link_libraries(CarRentalApp PRIVATE core)

# Adapter de web presentation goi truc tiep cac DSA Core C++.
add_executable(RentalWebCore web/core_bridge.cpp)
target_link_libraries(RentalWebCore PRIVATE core)

add_executable(RentalMC2Demo src/presentation/mainMC2.cpp)
target_link_libraries(RentalMC2Demo PRIVATE core)

add_executable(RentalMC1Demo src/presentation/demo_mc1.cpp)
target_link_libraries(RentalMC1Demo PRIVATE core)

add_executable(BenchmarkMC1 benchmark/benchmark_mc1.cpp)
target_include_directories(BenchmarkMC1 PRIVATE src/core/model src/core/structures src/core/services)
target_link_libraries(BenchmarkMC1 PRIVATE core)

add_executable(BenchmarkMC2 benchmark/benchmark-MC2.cpp)
target_link_libraries(BenchmarkMC2 PRIVATE core)

# ===== TEST =====
enable_testing()

add_executable(TestMC2Correctness test/test_thanhvien2.cpp)
target_link_libraries(TestMC2Correctness PRIVATE core)
add_test(NAME TestMC2Correctness COMMAND TestMC2Correctness)

add_executable(TestMC1Persistence test/test_thanhvien1.cpp)
target_include_directories(TestMC1Persistence PRIVATE src/core/model src/core/structures src/core/services)
target_link_libraries(TestMC1Persistence PRIVATE core)
add_test(NAME TestMC1Persistence COMMAND TestMC1Persistence)

# Test cho he tong (kiem tra MC1/MC2/RF2/RF3 luon nhat quan)
add_executable(TestUnified test/test_unified.cpp)
target_link_libraries(TestUnified PRIVATE core)
add_test(NAME TestUnified COMMAND TestUnified)
