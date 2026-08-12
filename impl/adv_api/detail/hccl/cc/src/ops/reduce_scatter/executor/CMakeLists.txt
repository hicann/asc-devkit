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
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_v2_reduce_scatter_sole_executor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_reduce_scatter_parallel_executor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_v2_reduce_scatter_sequence_executor_aicpu.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_v2_reduce_scatter_sequence_executor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ins_reduce_scatter_concurrent_executor.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
