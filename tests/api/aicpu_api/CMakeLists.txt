# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_aicpu_api_tests)
cmake_minimum_required(VERSION 3.16.0)

add_executable(ascendc_ut_aicpu_api
    ${ASCENDC_DIR}/tests/main.cpp
    ${ASCENDC_DIR}/impl/aicpu_api/aicpu_dump.cpp
    test_aicpu_dump.cpp
    test_aicpu_dump_fifo.cpp
)

target_compile_definitions(ascendc_ut_aicpu_api PRIVATE
    UT_TEST
    ASCENDC_DEBUG=1
    ASCENDC_CPU_DEBUG=1
)

target_include_directories(ascendc_ut_aicpu_api PRIVATE
    ${ASCENDC_DIR}/include
)

target_compile_options(ascendc_ut_aicpu_api PRIVATE
    -Werror
)

target_link_libraries(ascendc_ut_aicpu_api PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    -Wl,--no-as-needed
    error_manager
    mmpa
    c_sec
    -Wl,--as-needed
)

target_link_options(ascendc_ut_aicpu_api PRIVATE "-Wl,--wrap=malloc")

run_llt_test(
    TARGET ascendc_ut_aicpu_api
    TASK_NUM 1
)
