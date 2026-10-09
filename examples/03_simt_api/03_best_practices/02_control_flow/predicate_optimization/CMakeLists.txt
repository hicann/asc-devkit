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
set(CMAKE_ASC_ENABLE_SIMT ON)

find_package(ASC REQUIRED)
project(kernel_samples LANGUAGES ASC CXX)

set(SCENARIO_NUM "1" CACHE STRING "Case type to run (1-6)")
if(NOT SCENARIO_NUM MATCHES "^[1-6]$")
    message(FATAL_ERROR "SCENARIO_NUM must be in [1, 6], but got ${SCENARIO_NUM}")
endif()
option(CYCLE_PROFILE "Build the single-warp execute-cycle profiling variant" OFF)

add_executable(predicate_optimization
    predicate_optimization.asc
)

if(CYCLE_PROFILE)
    target_compile_definitions(predicate_optimization PRIVATE
        CYCLE_PROFILE
    )
endif()

target_compile_definitions(predicate_optimization PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

target_link_libraries(predicate_optimization PRIVATE
    m
)
