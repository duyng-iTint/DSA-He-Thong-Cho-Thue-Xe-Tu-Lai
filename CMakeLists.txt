file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp)
add_library(core STATIC ${CORE_SOURCES})

add_executable(CarRentalApp
    src/Main.cpp
    src/presentation/InputHandler.cpp
    test/Tests.cpp
    benchmark/Benchmark.cpp
    # them file MC2, RF1, RF2 neu chua nam trong src/core
)
target_link_libraries(CarRentalApp PRIVATE core)