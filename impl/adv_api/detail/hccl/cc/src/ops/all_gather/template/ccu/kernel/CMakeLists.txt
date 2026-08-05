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
    # Legacy direct-launch/non-Mem2Mem AllGather kernels are kept for recovery; KFC Mem2Mem is the active path.
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_all_gather_mesh1d_mem2mem.cc
    # Disabled: the legacy direct-launch Mesh1D kernel uses old hcomm::CcuKernelArg/CcuKernelAlgBase APIs.
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_all_gather_mesh1d.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_kfc_all_gather_mesh1d.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_kfc_all_gather_mesh1d_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_all_gather_nhr1d_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_all_gather_2dies_mesh1d_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_all_gather_nhr1d_multi_jetty_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_all_gather_2dies_mesh_1D.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
