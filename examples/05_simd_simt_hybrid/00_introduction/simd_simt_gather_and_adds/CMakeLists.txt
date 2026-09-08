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

set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, cpu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")

project(kernel_samples)

set(ASC_SRC_FILE gather_and_adds.asc)

if("${CMAKE_ASC_RUN_MODE}" STREQUAL "cpu")
    find_package(tikicpulib REQUIRED)
    set_source_files_properties(${ASC_SRC_FILE} PROPERTIES LANGUAGE CXX)
else()
    find_package(ASC REQUIRED)
    enable_language(ASC CXX)
endif()

add_executable(demo
    ${ASC_SRC_FILE}
)

target_compile_options(demo PRIVATE
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},sim>:-g -O0>>
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},cpu>:-x c++>>
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},cpu>:-g -O2 -std=c++17 -D_GLIBCXX_USE_CXX11_ABI=0 -Wall -Werror>>
)

target_link_directories(demo PRIVATE
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},sim>:$ENV{ASCEND_HOME_PATH}/tools/simulator/${SOC_VERSION}/lib>>
)

target_link_libraries(demo PRIVATE
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},sim>:runtime_camodel>>
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},sim>:npu_drv>>
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},cpu>:cpudebug_acl_stub>>
    $<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_ASC_RUN_MODE},cpu>:tikicpulib::ascend950pr_9599>>
)
