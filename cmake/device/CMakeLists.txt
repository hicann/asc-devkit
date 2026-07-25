# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16.0)
include(../fetch_cann_cmake.cmake)
project(ascendc_tiling_api)

init_cann_project(PRODUCT_SIDE "device" PREPEND_MODULE_PATH)
add_cann_target_options()

set(HI_PYTHON "python3" CACHE STRING "python executor")
set(ASCENDC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(ASCENDC_INCLUDE_DIR ${ASCENDC_DIR}/include)
set(ASCENDC_IMPL_DIR ${ASCENDC_DIR}/impl)
set(INSTALL_LIBRARY_DIR ${CMAKE_HOST_SYSTEM_PROCESSOR}-linux/lib64/device)
get_filename_component(ASC_DEVKIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

find_cann_package(aicpu_sharder REQUIRED)
find_cann_package(tilingdata_base REQUIRED OBJECTS)
find_cann_package(metadef REQUIRED)
find_cann_package(unified_dlog REQUIRED)
find_cann_package(ascend_hal REQUIRED)
find_cann_package(securec REQUIRED)
find_cann_package(runtime REQUIRED)
find_cann_package(mmpa REQUIRED)
find_cann_package(acl_rt REQUIRED)
find_cann_package(atrace REQUIRED)
find_cann_package(error_manager REQUIRED)

include(${ASCENDC_DIR}/impl/adv_api/gen_kernel_tiling.cmake)

set(DEVICE_MODE TRUE)
set(KERNEL_MODE TRUE)

add_subdirectory(../../impl/aicpu_api impl/aicpu_api)
add_subdirectory(../../impl/adv_api/tiling impl/adv_api/tiling)
add_subdirectory(../../impl/adv_api/detail/hccl/cc impl/adv_api/detail/hccl/cc)

set_cann_subprj_package()
