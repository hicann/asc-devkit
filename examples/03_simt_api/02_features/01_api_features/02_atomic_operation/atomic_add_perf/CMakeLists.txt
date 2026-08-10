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

find_package(ASC REQUIRED)

project(atomic_add_perf LANGUAGES ASC CXX)

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU ARCH, e.g. dav-3510")
set(SCENARIO_NUM "1" CACHE STRING "Scenario number (1-16)")
option(SKIP_VALIDATION "Skip result validation for replay-based profiling tools" OFF)

if(NOT SCENARIO_NUM MATCHES "^([1-9]|1[0-6])$")
    message(FATAL_ERROR "SCENARIO_NUM must be one of 1..16, but got ${SCENARIO_NUM}")
endif()

add_executable(atomic_add_perf
    atomic_add_perf.asc
)

target_compile_definitions(atomic_add_perf PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
    SKIP_VALIDATION=$<BOOL:${SKIP_VALIDATION}>
)

target_compile_options(atomic_add_perf PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>
)

target_link_libraries(atomic_add_perf PRIVATE
    m
)
