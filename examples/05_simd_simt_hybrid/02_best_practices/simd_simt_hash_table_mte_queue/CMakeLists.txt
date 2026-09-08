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

set(SCENARIO_NUM "0" CACHE STRING "Scenario: 0=SIMT-GroupInsertOrAssign, 1=MTE-TaskQueue")
if (NOT SCENARIO_NUM MATCHES "^[0-1]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 0 or 1")
endif()

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")

find_package(ASC REQUIRED)
project(hash_table_mte_queue LANGUAGES ASC CXX)

add_executable(hash_table_mte_queue
    hash_table_mte_queue.asc
)

message(STATUS "[INFO] SCENARIO_NUM is ${SCENARIO_NUM} (0=SIMT-GroupInsertOrAssign, 1=MTE-TaskQueue)")

target_compile_definitions(hash_table_mte_queue PRIVATE
    SCENARIO_NUM=${SCENARIO_NUM}
)
