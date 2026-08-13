# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set(CMAKE_PREFIX_PATH ${ASCEND_CANN_PACKAGE_PATH}/)

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
  ${CMAKE_MODULE_PATH}
)

add_cann_third_party(boost)
add_cann_third_party(json)

find_cann_package(unified_dlog REQUIRED)
find_cann_package(securec REQUIRED)
find_cann_package(mmpa REQUIRED)
find_cann_package(metadef REQUIRED)
find_cann_package(platform REQUIRED)
find_cann_package(error_manager REQUIRED)
find_cann_package(acl_rt REQUIRED)
find_cann_package(runtime REQUIRED)

if(ENABLE_TEST)
  find_cann_package(graph REQUIRED)
  find_cann_package(pvmodel REQUIRED)
endif()
