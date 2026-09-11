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

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU ARCH, e.g. dav-3510")

find_package(ASC REQUIRED)
project(kernel_samples LANGUAGES ASC CXX)

set(SCENARIO_NUM "4" CACHE STRING "Case type to run (0-4)")

if(NOT SCENARIO_NUM MATCHES "^[0-4]$")
    message(FATAL_ERROR "SCENARIO_NUM must be one of 0, 1, 2, 3, 4.")
endif()

add_executable(matrix_transpose
    matrix_transpose.asc
)

target_compile_definitions(matrix_transpose PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

# ======================================================================================
# NPU 编译选项配置
#
# 说明：
#   - `--cce-res-usage`用于输出寄存器和栈等编译资源信息，便于分析资源使用。
# ======================================================================================
target_compile_options(matrix_transpose PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--cce-res-usage>
)

target_link_libraries(matrix_transpose PRIVATE
    m
)
