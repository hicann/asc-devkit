# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
add_subdirectory(executor)
add_subdirectory(selector)
add_subdirectory(template)
add_subdirectory(topo)

set(src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/op_common.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ccu_log.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/dlhcomm_function.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/cann_host_bridge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/omnipipe_data_slice_calc.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
