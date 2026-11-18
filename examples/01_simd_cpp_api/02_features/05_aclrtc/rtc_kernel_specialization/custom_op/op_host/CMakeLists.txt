# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

npu_op_code_gen(SRC ${CMAKE_CURRENT_SOURCE_DIR}/matmul_custom_host.cpp PACKAGE ${package_name} OUT_DIR ${ASCEND_AUTOGEN_PATH} JOIN_OP_DEF True)
npu_op_library(cust_optiling TILING matmul_custom_host.cpp)
file(GLOB api_sources ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${api_sources} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN ${api_sources})
file(GLOB proto_sources ${ASCEND_AUTOGEN_PATH}/op_proto.cc ${ASCEND_AUTOGEN_PATH}/group_op_proto/*.cc)
set_source_files_properties(${proto_sources} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH matmul_custom_host.cpp ${proto_sources})
npu_op_package_add(${package_name} LIBRARY cust_optiling cust_opapi cust_op_proto)
