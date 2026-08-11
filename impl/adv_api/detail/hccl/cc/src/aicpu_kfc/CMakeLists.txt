# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(HCCL_HCOMM_HEADER_DIR ${HCCL_CC_INCLUDE_DIR}/hcomm)
set(HCCL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ORION_REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/legacy)
set(HCOMM_REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
include(${CMAKE_CURRENT_SOURCE_DIR}/mc2_server_minimal.cmake)

#组件间对外头文件目录
add_library(asc_hccl_headers INTERFACE)
target_include_directories(asc_hccl_headers INTERFACE
    $<BUILD_INTERFACE:${TOP_DIR}/hcomm/include>
    $<BUILD_INTERFACE:${TOP_DIR}/hcomm/include/hccl>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/hccl>
    $<INSTALL_INTERFACE:include/hccl/external>
    $<INSTALL_INTERFACE:include/hccl/external/hccl>
)

if(BUILD_OPEN_PROJECT)
    add_cann_third_party(json)
endif()

add_subdirectory(framework)
