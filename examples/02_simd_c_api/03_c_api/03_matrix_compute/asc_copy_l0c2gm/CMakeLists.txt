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

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    asc_copy_l0c2gm.asc
)

target_link_libraries(demo PRIVATE)

# 场景参数（通过命令行传入）
# SCENARIO_NUM: 场景编号
#   1 - 输出格式NZ，输出数据类型float
#   2 - 输出格式ND，输出数据类型float
#   3 - 输出格式DN，输出数据类型float
#   4 - 输出格式ND，输出数据类型int8_t，使能Scalar量化
#   5 - 输出格式ND，输出数据类型int8_t，使能Vector量化
#   6 - 输出格式ND，输出数据类型float，使能relu
#   7 - 输出格式NZ，输出数据类型float，使能channel split
set(SCENARIO_NUM "1" CACHE STRING "Scene number (1-7)")
if(NOT SCENARIO_NUM MATCHES "^[1-7]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 1, 2, 3, 4, 5, 6 or 7, but got ${SCENARIO_NUM}")
endif()

add_definitions(-DSCENARIO_NUM=${SCENARIO_NUM})

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
