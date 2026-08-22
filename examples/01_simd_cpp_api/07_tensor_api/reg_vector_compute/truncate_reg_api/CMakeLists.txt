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

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

if(NOT DEFINED SCENARIO_NUM)
    message(FATAL_ERROR "SCENARIO_NUM is not set. Please specify -DSCENARIO_NUM=1/2/3")
endif()

if(NOT SCENARIO_NUM EQUAL 1 AND NOT SCENARIO_NUM EQUAL 2 AND NOT SCENARIO_NUM EQUAL 3)
    message(FATAL_ERROR "SCENARIO_NUM must be 1, 2 or 3, got: ${SCENARIO_NUM}")
endif()

add_executable(demo
    truncate_reg_api.asc
)

set(ASCEND_HOME "$ENV{ASCEND_HOME_PATH}")
set(ASC_INCLUDE_DIR "${ASCEND_HOME}/asc/")
include_directories(
    "${ASC_INCLUDE_DIR}"
)

target_compile_definitions(demo PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)

message(STATUS "[INFO] SCENARIO_NUM is ${SCENARIO_NUM}")
