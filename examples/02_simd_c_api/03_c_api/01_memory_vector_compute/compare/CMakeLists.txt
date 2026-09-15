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
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201")
set(SCENARIO_NUM "1" CACHE STRING "Scenario: 1-3")

if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-2201")
    message(FATAL_ERROR "This Memory vector compute C API example only supports CMAKE_ASC_ARCHITECTURES=dav-2201.")
endif()
if(NOT SCENARIO_NUM MATCHES "^[1-3]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 1, 2, or 3, but got ${SCENARIO_NUM}.")
endif()

find_package(ASC REQUIRED)
project(memory_vector_compare LANGUAGES ASC CXX)
add_executable(demo compare.asc)
target_link_libraries(demo PRIVATE tiling_api register platform m)
target_compile_definitions(demo PRIVATE SCENARIO_NUM=${SCENARIO_NUM})
target_compile_options(demo PRIVATE $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>)
