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

set(SCENARIO_NUM "1" CACHE STRING "ND2NZ scenario (1 or 2)")
if (NOT SCENARIO_NUM MATCHES "^[12]$")
    message(FATAL_ERROR "SCENARIO_NUM must be 1 or 2")
endif()

set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201, dav-3510 or dav-9201")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    bank_conflict_nd2nz.asc
)

if(CMAKE_ASC_ARCHITECTURES STREQUAL "dav-2201")
    set(NPU_ARCH_ID 2201)
elseif(CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    set(NPU_ARCH_ID 3510)
elseif(CMAKE_ASC_ARCHITECTURES STREQUAL "dav-9201")
    set(NPU_ARCH_ID 9201)
else()
    message(FATAL_ERROR "Unsupported CMAKE_ASC_ARCHITECTURES=${CMAKE_ASC_ARCHITECTURES}, only dav-2201, dav-3510 and dav-9201 are supported")
endif()

target_compile_definitions(demo PRIVATE
    SCENARIO=${SCENARIO_NUM}
    NPU_ARCH=${NPU_ARCH_ID}
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
