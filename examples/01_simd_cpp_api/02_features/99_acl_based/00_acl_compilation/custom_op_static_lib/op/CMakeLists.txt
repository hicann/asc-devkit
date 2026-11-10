# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.16.0)
project(static_aclnn_custom_op)

set(ASCEND_SOC_SERIES ascend910b ascend910_93 ascend950)
find_package(ASC REQUIRED)

set(package_name customize CACHE STRING "custom op package name")

npu_op_package(${package_name}
    TYPE STATIC
    CONFIG
        INSTALL_PATH ${CMAKE_INSTALL_PREFIX}
)

file(GLOB host_ops_srcs
    add_custom_host.cpp
)
npu_op_code_gen(
    SRC ${host_ops_srcs}
    PACKAGE ${package_name}
    OUT_DIR ${ASCEND_AUTOGEN_PATH}
)
npu_op_library(cust_optiling TILING
    ${host_ops_srcs}
)

file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)

file(GLOB proto_src ${ASCEND_AUTOGEN_PATH}/op_proto.cc)
set_source_files_properties(${proto_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH
    ${host_ops_srcs}
    ${proto_src}
)

npu_op_package_add(${package_name}
    LIBRARY
        cust_optiling
        cust_opapi
        cust_op_proto
)

npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_FILE add_custom_kernel.cpp
)

npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_CURRENT_SOURCE_DIR}
    TILING_LIBRARY cust_optiling
)

npu_op_package_add(${package_name}
    LIBRARY
        ascendc_kernels
)
