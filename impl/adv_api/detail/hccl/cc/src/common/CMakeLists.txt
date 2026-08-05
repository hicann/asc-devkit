# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

if(PRODUCT_SIDE STREQUAL "host")
    set(src_list
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter_acl.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alg_env_config.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/param_check.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alg_type.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter_error_manager_pub.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/config_log.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/log.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sal.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/hccl_mc2.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/hccl_alloc_ctx_res.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/compat.cc
    )

    target_sources(mc2_client PRIVATE
        ${src_list}
    )
endif()

add_subdirectory(hcomm_dlsym)
