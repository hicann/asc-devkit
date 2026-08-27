# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software; you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")
option(CANN_ASC_USE_EXPERIMENTAL "Enable experimental CANN ASC APIs" OFF)

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    histogram_compute_tensor_api.asc
)

set(ASCEND_HOME $ENV{ASCEND_HOME_PATH})

set(ASC_INCLUDE_DIR "${ASCEND_HOME}/asc/")

include_directories(
    "${ASC_INCLUDE_DIR}"
)

target_link_libraries(demo PRIVATE
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
    $<$<AND:$<COMPILE_LANGUAGE:ASC>,$<BOOL:${CANN_ASC_USE_EXPERIMENTAL}>>:-DCANN_ASC_USE_EXPERIMENTAL>
)
