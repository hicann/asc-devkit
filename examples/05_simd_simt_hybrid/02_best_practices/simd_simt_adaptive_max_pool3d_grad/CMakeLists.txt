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

set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")

# D/H/W of grad/argmax, all three the same. Change this value to reproduce the performance comparison tables in the README.
set(GRAD_DHW "8" CACHE STRING "D/H/W of grad/argmax, one of 1/2/4/8/16")

if(NOT GRAD_DHW MATCHES "^(1|2|4|8|16)$")
    message(FATAL_ERROR "GRAD_DHW must be one of 1, 2, 4, 8, 16.")
endif()

find_package(ASC REQUIRED)
project(adaptive_max_pool3d_grad_simd_simt_hybrid LANGUAGES ASC)

add_executable(adaptive_max_pool3d_grad adaptive_max_pool3d_grad_host.asc)

target_compile_definitions(adaptive_max_pool3d_grad PRIVATE GRAD_DHW=${GRAD_DHW})

math(EXPR GRAD_PER_NC_PLANE "${GRAD_DHW} * ${GRAD_DHW} * ${GRAD_DHW}")
message(STATUS "[INFO] GRAD_DHW is ${GRAD_DHW}, each NC plane scatters ${GRAD_PER_NC_PLANE} gradients")
message(STATUS "[INFO] Runtime mode is selected by ./adaptive_max_pool3d_grad <simd|scalar|hybrid|hybrid_ub> [profile]")
message(STATUS "[INFO] Block count adapts to the actual AIV core count at run time")
