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

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510, dav-9201")

if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510" AND NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-9201")
    message(FATAL_ERROR "matmul_mxfp4_high_performance only supports CMAKE_ASC_ARCHITECTURES=dav-3510 or dav-9201")
endif()

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

set(SCENARIO_NUM 2 CACHE STRING "Scenario number to compile (1-2)")

add_executable(demo
    matmul_mx.asc
)

if(CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    target_compile_definitions(demo PRIVATE NPU_ARCH_DAV_3510)
elseif(CMAKE_ASC_ARCHITECTURES STREQUAL "dav-9201")
    target_compile_definitions(demo PRIVATE NPU_ARCH_DAV_9201)
endif()

target_link_libraries(demo PRIVATE
    tiling_api
    register
    platform
    ${CMAKE_DL_LIBS}
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
