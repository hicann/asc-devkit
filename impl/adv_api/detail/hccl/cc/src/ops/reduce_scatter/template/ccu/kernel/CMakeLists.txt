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
    ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem.cc
    # Legacy direct-launch CCU kernels are retained but use APIs unavailable in the KFC-only mc2_client.
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_reduce_scatter_mesh1d_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_reduce_scatter_mesh1d.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_reduce_scatter_nhr1d_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_reduce_scatter_mesh1d_2die_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_reduce_scatter_nhr1d_multi_jetty_mem2mem.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/ccu_kernel_reduce_scatter_mesh2die.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
