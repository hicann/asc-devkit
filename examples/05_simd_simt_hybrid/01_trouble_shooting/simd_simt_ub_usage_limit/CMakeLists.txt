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

project(kernel_samples LANGUAGES ASC CXX)

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")

set(SCENARIO_NUM "3" CACHE STRING "Scenario number (0-14)")
if(NOT SCENARIO_NUM MATCHES "^([0-9]|1[0-4])$")
    message(FATAL_ERROR "SCENARIO_NUM must be one of 0-14.")
endif()

set(CASE_SOURCE "")
if(SCENARIO_NUM MATCHES "^[0-1]$")
    set(CASE_SOURCE case0_ub_out_of_bounds.asc)
elseif(SCENARIO_NUM MATCHES "^[2-5]$")
    set(CASE_SOURCE case1_ub_limit.asc)
elseif(SCENARIO_NUM STREQUAL "6")
    set(CASE_SOURCE case2_ub_layout.asc)
elseif(SCENARIO_NUM MATCHES "^([7-9]|1[0-4])$")
    set(CASE_SOURCE case3_out_of_bounds_result.asc)
endif()

option(CASE1_OVER_LIMIT "Add 4 bytes to the Case 1 UB limit to observe the failure result" OFF)

add_executable(ub_usage_limit
    ${CASE_SOURCE}
)

target_compile_options(ub_usage_limit PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
target_compile_definitions(ub_usage_limit PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

string(FIND "${CMAKE_ASC_FLAGS}" "--cce-disable-vf-stack-reserved-ubuf" CASE1_DISABLE_VF_RESERVED_POS)
string(FIND "${CMAKE_ASC_FLAGS}" "--cce-disable-asc-reserved-ubuf" CASE1_DISABLE_ASC_RESERVED_POS)
if(NOT CASE1_DISABLE_VF_RESERVED_POS EQUAL -1)
    target_compile_definitions(ub_usage_limit PRIVATE CASE1_DISABLE_VF_RESERVED=1)
endif()
if(NOT CASE1_DISABLE_ASC_RESERVED_POS EQUAL -1)
    target_compile_definitions(ub_usage_limit PRIVATE CASE1_DISABLE_ASC_RESERVED=1)
endif()
if(CASE1_OVER_LIMIT)
    target_compile_definitions(ub_usage_limit PRIVATE
        CASE1_OVER_LIMIT=1
    )
endif()
