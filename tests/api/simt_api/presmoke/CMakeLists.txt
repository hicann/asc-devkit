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
set(CMAKE_ASC_ARCHITECTURES dav-3510 CACHE STRING "Compile-only SIMT architecture")

set(PRESMOKE_PROGRAMMING_MODE "simt" CACHE STRING "SIMT programming mode: simt or hybrid")
set_property(CACHE PRESMOKE_PROGRAMMING_MODE PROPERTY STRINGS simt hybrid)
if(PRESMOKE_PROGRAMMING_MODE STREQUAL "simt")
    set(CMAKE_ASC_ENABLE_SIMT ON)
elseif(NOT PRESMOKE_PROGRAMMING_MODE STREQUAL "hybrid")
    message(FATAL_ERROR "PRESMOKE_PROGRAMMING_MODE must be simt or hybrid")
endif()

find_package(ASC REQUIRED)
project(simt_math_presmoke LANGUAGES ASC CXX)

set(MATH_PRESMOKE_TARGETS integer_math_functions float_math_functions half_math_functions
    bfloat16_math_functions type_conversion_functions)
if(PRESMOKE_PROGRAMMING_MODE STREQUAL "hybrid")
    list(APPEND MATH_PRESMOKE_TARGETS fp8_conversion_functions)
endif()

foreach(target_source IN LISTS MATH_PRESMOKE_TARGETS)
    add_library(${target_source} OBJECT ${target_source}.asc)
    if(PRESMOKE_PROGRAMMING_MODE STREQUAL "simt")
        target_compile_definitions(${target_source} PRIVATE PRESMOKE_PURE_SIMT=1)
    endif()
    target_include_directories(${target_source} BEFORE PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
endforeach()
