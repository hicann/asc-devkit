# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

cmake_minimum_required(VERSION 3.16)
project(specialization_matmul)
find_package(ASC REQUIRED)
set(package_name customize)
set(ASCEND_AUTOGEN_PATH ${CMAKE_CURRENT_BINARY_DIR}/autogen)
npu_op_package(${package_name}
    TYPE RUN
    CONFIG
        ENABLE_SOURCE_PACKAGE ON
        ENABLE_BINARY_PACKAGE ON
        KERNEL_SPEC_MODE Normal
        INSTALL_PATH ${CMAKE_CURRENT_BINARY_DIR}
)
add_subdirectory(op_host)
add_subdirectory(op_kernel)
