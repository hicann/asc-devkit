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
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201, dav-3510")
set(SCENARIO_NUM "1" CACHE STRING "Scenario number: 1=no split, 2=L2 Cache split")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-2201" AND
   NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    message(FATAL_ERROR "Unsupported CMAKE_ASC_ARCHITECTURES=${CMAKE_ASC_ARCHITECTURES}")
endif()

if(SCENARIO_NUM STREQUAL "1")
    set(SAMPLE_SOURCE add_l2cache_nosplit.asc)
elseif(SCENARIO_NUM STREQUAL "2")
    set(SAMPLE_SOURCE add_l2cache_split.asc)
else()
    message(FATAL_ERROR "Unsupported SCENARIO_NUM=${SCENARIO_NUM}. Use 1 for no split or 2 for L2 Cache split.")
endif()

add_executable(demo
    ${SAMPLE_SOURCE}
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)

target_compile_definitions(demo PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

target_link_libraries(demo PRIVATE
    tiling_api
    register
    platform
    m
    dl
)
