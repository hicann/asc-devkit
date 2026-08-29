# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
add_subdirectory(kernel)

set(src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_to_all_mesh_1D.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ccu_temp_all_to_all_v_mesh_1D.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/ccu_kernel_all_to_all_v_mesh1d.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
