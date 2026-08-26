# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_temp_all_gather_mesh_1D.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_temp_all_gather_mesh_1D_Z_axis_detour.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_temp_all_gather_nhr.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_temp_all_gather_nhr_dpu.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_temp_all_gather_omnipipe_mesh_1D.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_temp_all_gather_omnipipe_nhr.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
