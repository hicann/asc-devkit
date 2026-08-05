# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------
set(src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/topo.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/topo_host.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/topo_match_1d.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/topo_match_base.cc
)
if(NOT HCCL_CANN_COMPAT_850)
    list(APPEND src_list
        ${CMAKE_CURRENT_SOURCE_DIR}/topo_match_multilevel.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/topo_match_ubx.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/topo_match_ubx_1d.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/topo_match_pcie_mix.cc
    )
endif()

target_sources(mc2_client PRIVATE
    ${src_list}
)
