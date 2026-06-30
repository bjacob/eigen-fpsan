# Copyright (c) 2026 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#
# CMakeLists.txt
#
# Build system for the Eigen + FPSan demos.
#
cmake_minimum_required(VERSION 3.24)

project(eigen_fpsan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Configure eigen-fpsan out of tree, for example: cmake -S . -B ../eigen-fpsan-build -G Ninja")
endif()

set(FPSAN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FPSAN_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FPSAN_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(FPSAN_BUILD_PYTHON OFF CACHE BOOL "" FORCE)
set(CMAKE_HIP_COMPILER "" CACHE FILEPATH "" FORCE)
add_subdirectory(third_party/hip-fpsan EXCLUDE_FROM_ALL)

set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_BLAS OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_LAPACK OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_CMAKE_PACKAGE OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/eigen EXCLUDE_FROM_ALL)

function(add_eigen_fpsan_demo name source)
  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE Eigen3::Eigen fpsan::fpsan)
  target_include_directories(${name} PRIVATE include src)
  target_compile_options(${name} PRIVATE $<$<CXX_COMPILER_ID:Clang,GNU>:-Wall -Wextra -Wpedantic>)

  if(BUILD_TESTING)
    add_test(NAME ${name} COMMAND ${name})
  endif()
endfunction()

add_eigen_fpsan_demo(eigen_fpsan_determinant_demo src/determinant_demo.cpp)
add_eigen_fpsan_demo(eigen_fpsan_solve_demo src/solve_demo.cpp)
add_eigen_fpsan_demo(eigen_fpsan_least_squares_demo src/least_squares_demo.cpp)
