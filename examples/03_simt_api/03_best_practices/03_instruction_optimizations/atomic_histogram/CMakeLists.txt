# ----------------------------------------------------------------------------------------------------------
# This program is free software, you can redistribute it and/or modify it.
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This file is a part of the CANN Open Software.
# Licensed under CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

find_package(ASC REQUIRED)

project(histogram LANGUAGES ASC CXX)

add_executable(histogram
    histogram.asc
)

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU ARCH, e.g. dav-3510")

option(SKIP_VALIDATION "Skip result validation for replay-based profiling tools" OFF)

# ======================================================================================
# NPU编译选项配置
#
# 说明：
#   - 需根据实际部署的NPU硬件架构选择对应的`npu-arch`参数。
# ======================================================================================
target_compile_options(histogram PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>
)

target_compile_definitions(histogram PRIVATE
    SKIP_VALIDATION=$<BOOL:${SKIP_VALIDATION}>
)

target_link_libraries(histogram PRIVATE
    m
)
