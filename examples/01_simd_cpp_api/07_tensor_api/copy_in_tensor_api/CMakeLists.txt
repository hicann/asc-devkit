# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software; you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")

if(NOT CMAKE_ASC_RUN_MODE STREQUAL "npu" AND NOT CMAKE_ASC_RUN_MODE STREQUAL "sim")
    message(FATAL_ERROR "CMAKE_ASC_RUN_MODE must be 'npu' or 'sim', got: '${CMAKE_ASC_RUN_MODE}'")
endif()

if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    message(FATAL_ERROR "copy_in_tensor_api only supports CMAKE_ASC_ARCHITECTURES=dav-3510")
endif()

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

# 场景参数（通过命令行传入）
# SCENARIO_NUM: 场景编号
#   0 - AB不转置，数据类型half
#   1 - AB不转置，数据类型float
#   2 - AB转置，数据类型half
#   3 - AB转置，数据类型float
set(SCENARIO_NUM "0" CACHE STRING "Scenario number (0-3)")

if(NOT SCENARIO_NUM MATCHES "^[0-3]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 0-3, got: '${SCENARIO_NUM}'")
endif()

add_executable(demo
    copy_in_tensor_api.asc
)

target_link_libraries(demo PRIVATE
)

add_definitions(-DSCENARIO_NUM=${SCENARIO_NUM})

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
