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

set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201, dav-3510")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    fixpipe_params_switch.asc
)

# 场景参数（通过命令行传入）
# SCENARIO_NUM: 场景编号
#   1 - NZ2ND场景（CFG_ROW_MAJOR），两种结构体dstStride单位均为element
#   2 - NZ2NZ场景（CFG_NZ），dstStride单位不同：V220为datablock(32B)，Arch3510为element
set(SCENARIO_NUM "1" CACHE STRING "Scene number (1-2)")
if(NOT SCENARIO_NUM MATCHES "^[1-2]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 1 or 2, but got ${SCENARIO_NUM}")
endif()

add_definitions(-DSCENARIO_NUM=${SCENARIO_NUM})

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
