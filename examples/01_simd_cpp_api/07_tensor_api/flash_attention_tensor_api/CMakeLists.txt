# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)
set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")

if(NOT CMAKE_ASC_RUN_MODE STREQUAL "npu" AND NOT CMAKE_ASC_RUN_MODE STREQUAL "sim")
    message(FATAL_ERROR "CMAKE_ASC_RUN_MODE must be 'npu' or 'sim', got: '${CMAKE_ASC_RUN_MODE}'")
endif()
if(NOT CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    message(FATAL_ERROR "flash_attention_tensor_api only supports CMAKE_ASC_ARCHITECTURES=dav-3510")
endif()

find_package(ASC REQUIRED)
project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    flash_attention_tensor_api.asc
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)

option(FA_DEBUG_PV "Build with the PV update bypassed for board debugging" OFF)
if(FA_DEBUG_PV)
    target_compile_definitions(demo PRIVATE FA_DEBUG_PV)
endif()

option(FA_DEBUG_AIC_ONLY "Build with only the QK matmul and CV synchronization" OFF)
if(FA_DEBUG_AIC_ONLY)
    target_compile_definitions(demo PRIVATE FA_DEBUG_AIC_ONLY)
endif()
