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
    main.cc
    all_gather_nhr_testcase.cc
    all_gather_aicpu_testcase.cc
    all_to_all_mesh1d_aicpu_testcase.cc
    mc2_aicpu_task_cache_testcase.cc
)

add_executable(${HCCL_CHECKER_TARGET} ${src_list})

target_include_directories(${HCCL_CHECKER_TARGET} PRIVATE
    ${HCCL_CHECKER_COMMON_INCLUDE_LIST}
)

target_compile_options(${HCCL_CHECKER_TARGET} PRIVATE
    -std=c++14
    -Werror
    -fno-common
    -fno-strict-aliasing
    -pipe
    -O0
    -g
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${HCCL_CHECKER_TARGET} PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)

target_link_libraries(${HCCL_CHECKER_TARGET} PRIVATE
    intf_llt_pub_basic
    -Wl,--whole-archive
    ${HCCL_CHECKER_SELECTOR_TARGET}
    ${HCCL_CHECKER_EXECUTOR_TARGET}
    -Wl,--no-whole-archive
    -Wl,--start-group
    ${HCCL_CHECKER_KERNEL_TARGET}
    ${HCCL_CHECKER_SELECTOR_TARGET}
    ${HCCL_CHECKER_EXECUTOR_TARGET}
    ${HCCL_CHECKER_MODEL_PROXY_TARGET}
    ${HCCL_CHECKER_VERIFIER_TARGET}
    -Wl,--end-group
    c_sec
    pthread
    dl
)

run_llt_test(
    TARGET ${HCCL_CHECKER_TARGET}
    TASK_NUM 1
)

set(HCCL_AICPU_ALLOC_CTX_TARGET ascendc_ut_adv_api_hccl_cc_aicpu_alloc_ctx)

add_executable(${HCCL_AICPU_ALLOC_CTX_TARGET}
    main.cc
    aicpu_alloc_ctx_res_testcase.cc
)

target_include_directories(${HCCL_AICPU_ALLOC_CTX_TARGET} PRIVATE
    ${HCCL_CHECKER_COMMON_INCLUDE_LIST}
)

target_compile_options(${HCCL_AICPU_ALLOC_CTX_TARGET} PRIVATE
    -std=c++14
    -Werror
    -fno-common
    -fno-strict-aliasing
    -pipe
    -O0
    -g
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${HCCL_AICPU_ALLOC_CTX_TARGET} PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)

target_link_libraries(${HCCL_AICPU_ALLOC_CTX_TARGET} PRIVATE
    intf_llt_pub_basic
    mc2_client
    -Wl,--whole-archive
    ${HCCL_CHECKER_MODEL_PROXY_TARGET}
    -Wl,--no-whole-archive
    c_sec
    pthread
    dl
)

# The test must exercise the mc2_client built from the current source tree even
# when an older CANN installation is present in LD_LIBRARY_PATH.
# Prepend local build directories to LD_LIBRARY_PATH so that locally built
# libraries (libmc2_client.so, libmc2_compat.so) are loaded before any older
# versions from the CANN installation.
add_custom_command(
    TARGET ${HCCL_AICPU_ALLOC_CTX_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=$<TARGET_FILE_DIR:mc2_client>:$<TARGET_FILE_DIR:mc2_compat>:$ENV{LD_LIBRARY_PATH} $<TARGET_FILE:${HCCL_AICPU_ALLOC_CTX_TARGET}>
    COMMENT "Run ${HCCL_AICPU_ALLOC_CTX_TARGET} with local library path priority"
)

add_subdirectory(mc2_ccu)
