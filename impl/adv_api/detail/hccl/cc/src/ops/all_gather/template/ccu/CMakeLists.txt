# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
add_subdirectory(kernel)

set(src_list
    # Legacy direct-launch/non-Mem2Mem AllGather templates are kept for recovery; KFC Mem2Mem is the active path.
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_gather_mesh_1D_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_gather_mesh_1D.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_kfc_all_gather_mesh_1D_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_gather_nhr_1D_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_gather_2dies_mesh_1d_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_gather_2dies_mesh_1D.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_gather_nhr_1D_multi_jetty_mem2mem.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
