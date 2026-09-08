# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.16)

set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, cpu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")
set(TEST_MODE "1" CACHE STRING "test mode: 1, 2, or 3")
set(API_MODE "1" CACHE STRING "API mode: 1 calCount, 2 whole tensor, 3 in-place")
set(SIZE "2048" CACHE STRING "total length to compute")
set(ASC_DEVKIT_PATH "" CACHE PATH "asc-devkit source directory used before installation")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    ndtri.asc
)

target_link_libraries(demo PRIVATE
    register
    platform
    m
    dl
    graph_base
)

if(ASC_DEVKIT_PATH)
    target_include_directories(demo BEFORE PRIVATE
        ${ASC_DEVKIT_PATH}
        ${ASC_DEVKIT_PATH}/include
        ${ASC_DEVKIT_PATH}/impl
    )
endif()

target_compile_definitions(demo PRIVATE
    TEST_MODE=${TEST_MODE}
    API_MODE=${API_MODE}
    SIZE=${SIZE}
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
