cmake_minimum_required(VERSION 3.0.0)
project(TCP VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/incs)

file(GLOB SOURCES srcs/*.cpp srcs/Packet/*.cpp)

add_library(TCP ${SOURCES})

target_compile_features(TCP PUBLIC cxx_std_11)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
