# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.16)

set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201, dav-3510, dav-9201")

find_package(ASC REQUIRED)
project(kernel_samples LANGUAGES ASC CXX)

execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcc.a
    OUTPUT_VARIABLE LIBGCC_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GCC_QUERY_RESULT
)
if(NOT GCC_QUERY_RESULT EQUAL 0 OR NOT EXISTS "${LIBGCC_PATH}")
    message(FATAL_ERROR "Failed to locate libgcc.a using ${CMAKE_CXX_COMPILER}")
endif()

get_filename_component(LIBGCC_PATH "${LIBGCC_PATH}" REALPATH)
string(REGEX REPLACE "/lib/gcc/.*$" "" GCC_TOOLCHAIN_ROOT "${LIBGCC_PATH}")
if("${GCC_TOOLCHAIN_ROOT}" STREQUAL "${LIBGCC_PATH}")
    message(FATAL_ERROR "Failed to derive GCC toolchain root from ${LIBGCC_PATH}")
endif()
message(STATUS "GCC toolchain root: ${GCC_TOOLCHAIN_ROOT}")

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ASCEND_SLOG_PRINT_TO_STDOUT=0
            ${Python3_EXECUTABLE} -c "import torch; print(torch.utils.cmake_prefix_path)"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE TORCH_CMAKE_PREFIX_PATH
)
find_package(Torch REQUIRED HINTS ${TORCH_CMAKE_PREFIX_PATH})

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ASCEND_SLOG_PRINT_TO_STDOUT=0
            ${Python3_EXECUTABLE} -c "import os, torch_npu; print(os.path.dirname(torch_npu.__file__))"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE TORCH_NPU_PATH
)
set(TORCH_NPU_INCLUDE_DIRS ${TORCH_NPU_PATH}/include)
set(TORCH_NPU_LIBRARIES ${TORCH_NPU_PATH}/lib)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ASCEND_SLOG_PRINT_TO_STDOUT=0
            ${Python3_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYBIND11_CMAKE_PREFIX_PATH
)
find_package(pybind11 REQUIRED HINTS ${PYBIND11_CMAKE_PREFIX_PATH})

pybind11_add_module(ascendc_ops SHARED
    add_custom.asc
)

target_include_directories(ascendc_ops PRIVATE
    ${TORCH_INCLUDE_DIRS}
    ${TORCH_NPU_INCLUDE_DIRS}
)

target_link_libraries(ascendc_ops PRIVATE
    ${TORCH_LIBRARIES}
    torch_npu
)

target_link_directories(ascendc_ops PRIVATE
    ${TORCH_NPU_LIBRARIES}
)

target_link_options(ascendc_ops PRIVATE
    --gcc-toolchain=${GCC_TOOLCHAIN_ROOT}
)

target_compile_options(ascendc_ops PRIVATE
    ${TORCH_CXX_FLAGS}
    $<$<COMPILE_LANGUAGE:ASC>:--gcc-toolchain=${GCC_TOOLCHAIN_ROOT}>
)
