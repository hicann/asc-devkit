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

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

# 场景参数（通过命令行传入）
# SCENARIO_NUM: 场景编号
#   0 - 普通模式：L0C直接搬出到GM
#   1 - 使能unitflag：L0C搬出到GM时使能unitflag
#   2 - 搬运UB使能通道拆分M：L0C -> UB(通道拆分M) -> GM
#   3 - 搬运UB使能通道拆分N：L0C -> UB(通道拆分N) -> GM
set(SCENARIO_NUM "0" CACHE STRING "Scenario number (0-3)")

add_executable(demo
    copy_coord_shape_tensor_api.asc
)

set(ASCEND_HOME $ENV{ASCEND_HOME_PATH})

set(ASC_INCLUDE_DIR "${ASCEND_HOME}/asc/")

include_directories(
    "${ASC_INCLUDE_DIR}"
)

target_link_libraries(demo PRIVATE
)

add_definitions(-DSCENARIO_NUM=${SCENARIO_NUM})

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
