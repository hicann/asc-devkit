# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

add_library(hccl-model-proxy STATIC "")
add_library(${HCCL_CHECKER_MODEL_PROXY_TARGET} ALIAS hccl-model-proxy)

add_subdirectory(hccl_verifier)
add_subdirectory(sim_world)
add_subdirectory(hccl_proxy)
add_subdirectory(common)
add_subdirectory(aicpu)

target_compile_options(hccl-model-proxy PRIVATE
    -std=c++14
    -Werror
    -fno-common
    -fno-strict-aliasing
    -pipe
    -O0
    -g
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(hccl-model-proxy PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)

target_include_directories(hccl-model-proxy PUBLIC
    ${HCCL_CHECKER_COMMON_INCLUDE_LIST}
)

target_link_libraries(hccl-model-proxy PUBLIC
    ${HCCL_CHECKER_KERNEL_TARGET}
    asccomm_ccu_headers
)
