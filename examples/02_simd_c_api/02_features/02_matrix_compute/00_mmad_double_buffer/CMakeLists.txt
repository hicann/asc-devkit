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
set(SCENARIO_NUM "1" CACHE STRING "Scenario number: 0 (baseline) or 1 (L1 double buffer)")

find_package(ASC REQUIRED)

project(c_api_mmad_double_buffer LANGUAGES ASC CXX)

if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    message(FATAL_ERROR "This sample only supports CMAKE_ASC_ARCHITECTURES=dav-3510")
endif()

if(NOT SCENARIO_NUM MATCHES "^[01]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 0 or 1, but got ${SCENARIO_NUM}")
endif()

add_executable(c_api_mmad_double_buffer_example
    mmad_double_buffer.asc
)

message(STATUS "[INFO] SCENARIO_NUM is ${SCENARIO_NUM}")
target_compile_definitions(c_api_mmad_double_buffer_example PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

target_compile_options(c_api_mmad_double_buffer_example PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
