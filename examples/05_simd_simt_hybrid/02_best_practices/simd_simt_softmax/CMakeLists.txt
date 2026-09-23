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

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")
set(SCENARIO_NUM "2" CACHE STRING "Case type to run (0-2)")

find_package(ASC REQUIRED)
project(softmax_simd_simt_hybrid LANGUAGES ASC CXX)

if(NOT SCENARIO_NUM MATCHES "^[0-2]$")
    message(FATAL_ERROR "SCENARIO_NUM must be one of 0, 1, 2.")
endif()

add_executable(softmax softmax_host.asc)

target_compile_definitions(softmax PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

target_compile_options(softmax PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--cce-res-usage>
)

target_link_libraries(softmax PRIVATE m)
