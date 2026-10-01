cmake_minimum_required(VERSION 3.16)

project(DSA-He-Thong-Cho-Thue-Xe-Tu-Lai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(DSA-He-Thong-Cho-Thue-Xe-Tu-Lai
    ${SOURCES}
)