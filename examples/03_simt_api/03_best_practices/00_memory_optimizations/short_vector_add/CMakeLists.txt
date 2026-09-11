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
set(CMAKE_ASC_ENABLE_SIMT ON)

find_package(ASC REQUIRED)

set(PROJECT_NAME "kernel_samples" CACHE STRING "Project name")
project(${PROJECT_NAME} LANGUAGES ASC CXX)

# 场景参数（通过命令行传入）
# SCENARIO_NUM: 场景编号
#   1 - half：half类型的向量处理，基线场景
#   2 - half2：half2类型的向量处理，提高数据计算效率（与场景1同样数据规模）
set(SCENARIO_NUM "1" CACHE STRING "Scene number (1-2)")
if(NOT SCENARIO_NUM MATCHES "^[1-2]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 1 or 2, but got ${SCENARIO_NUM}")
endif()
add_definitions(-DSCENARIO_NUM=${SCENARIO_NUM})

set(TARGET_FILENAME "demo" CACHE STRING "Execution file name")
add_executable(${TARGET_FILENAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/short_vector_add.asc
)
