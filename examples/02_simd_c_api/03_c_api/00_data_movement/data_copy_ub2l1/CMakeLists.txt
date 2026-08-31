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

set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")
set(SCENARIO_NUM "1" CACHE STRING "Scenario number (1-3)")

find_package(ASC REQUIRED)

project(c_api_data_copy_ub2l1 LANGUAGES ASC CXX)

if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    message(FATAL_ERROR "This C API example only supports CMAKE_ASC_ARCHITECTURES=dav-3510.")
endif()

if(NOT CMAKE_ASC_RUN_MODE MATCHES "^(npu|sim)$")
    message(FATAL_ERROR "CMAKE_ASC_RUN_MODE must be npu or sim, but got ${CMAKE_ASC_RUN_MODE}.")
endif()

if(NOT SCENARIO_NUM MATCHES "^[1-3]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 1, 2 or 3, but got ${SCENARIO_NUM}.")
endif()

add_executable(demo data_copy_ub2l1.asc)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)

ascendc_compile_definitions(demo PRIVATE
    -DSCENARIO_NUM=${SCENARIO_NUM}
)
