# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_devkit_ut)
cmake_minimum_required(VERSION 3.16.0)

# ================ context & ascendc_platform ================
set(PRODUCT_TYPE_LIST
    ascend910
    ascend610
    ascend310p
    ascend910B1_AIC
    ascend910B1_AIV
    ascend310B1
    ascend610Lite
    ascend950pr_9599_AIV_BASIC
    ascend950pr_9599_AIV_FRAMEWORK
    ascend950pr_9599_AIV_MICRO
    ascend950pr_9599_AIV_SIMT
    ascend950pr_9599_AIC
    mc62cm12aa_AIC
    mc62cm12aa_BASIC
    mc62cm12aa_FRAMEWORK
    mc62cm12aa_MICRO
    mc62cm12aa_SIMT
)

file(GLOB ASCENDC_TILING_TEST_SRC_FILES
    test_tiling.cpp
)

file(GLOB ASCENDC_TILING_SRC_FILES
    ${ASCENDC_DIR}/impl/utils/platform/*.cpp
    ${ASCENDC_DIR}/impl/utils/context/*.cpp
    ${ASCENDC_DIR}/impl/utils/stub/*.cpp
    ${ASCENDC_DIR}/impl/utils/tiling/*.cpp
    ${ASCENDC_DIR}/impl/utils/tiling/platform/*.cpp
    ${ASCENDC_DIR}/tests/tools/utils/stub/acl_rt_stub.cpp
)

foreach(product_type ${PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_tiling_utils_${product_type}
        ${ASCENDC_DIR}/tests/main.cpp
        ${ASCENDC_TILING_TEST_SRC_FILES}
        ${ASCENDC_TILING_SRC_FILES}
        ../../common/common.cpp
        ../../common/platform_stub.cpp
        ../../common/dlog_stub.cpp
    )

    target_compile_definitions(ascendc_ut_tiling_utils_${product_type} PRIVATE
        $<$<STREQUAL:${product_type},ascend910>:__NPU_ARCH__=1001>
        $<$<STREQUAL:${product_type},ascend610>:__NPU_ARCH__=2002>
        $<$<STREQUAL:${product_type},ascend310p>:__NPU_ARCH__=2002>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:__NPU_ARCH__=2201>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:__NPU_ARCH__=2201>
        $<$<STREQUAL:${product_type},ascend310B1>:__NPU_ARCH__=3002>
        $<$<STREQUAL:${product_type},ascend610Lite>:__NPU_ARCH__=3102>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_BASIC>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_FRAMEWORK>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_MICRO>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_SIMT>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:__NPU_ARCH__=3510>
    )

    target_include_directories(ascendc_ut_tiling_utils_${product_type} PRIVATE
        ${ASCENDC_DIR}/include
        ${ASCENDC_DIR}/include/utils/context
        ${ASCENDC_DIR}/include/utils/tiling/platform
        ${ASCENDC_DIR}/include/utils/stub
        ${ASCENDC_DIR}/include/utils/stub/common
        ${ASCEND_CANN_PACKAGE_PATH}/include
        ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc
        ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base
    )

    target_compile_options(ascendc_ut_tiling_utils_${product_type} PRIVATE
        -Werror
    )

    target_link_libraries(ascendc_ut_tiling_utils_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        kernel_tiling
        register
        graph
        graph_base
        opp_registry
        platform
        metadef
        unified_dlog
        dl
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        platform
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_tiling_utils_${product_type}
        TASK_NUM 1
    )
endforeach()

# ================ tpl tiling ================
set(ASCENDC_TPL_TILING_MODE debug release)
foreach(impl_mode ${ASCENDC_TPL_TILING_MODE})
    add_executable(ascendc_ut_tpl_tiling_${impl_mode}
        ${ASCENDC_DIR}/tests/main.cpp
        ${ASCENDC_DIR}/impl/utils/tiling/template_argument.cpp
        test_tpl_tiling_key_${impl_mode}.cpp
    )
    if(${impl_mode} STREQUAL "debug")
        target_compile_definitions(ascendc_ut_tpl_tiling_${impl_mode} PRIVATE
            ASCENDC_DEBUG=1
        )
    endif()

    target_include_directories(ascendc_ut_tpl_tiling_${impl_mode} PRIVATE
         ${ASCENDC_DIR}/include
    )

    target_compile_options(ascendc_ut_tpl_tiling_${impl_mode} PRIVATE
        -Werror
    )

    target_link_libraries(ascendc_ut_tpl_tiling_${impl_mode} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
    )

    run_llt_test(
        TARGET ascendc_ut_tpl_tiling_${impl_mode}
        TASK_NUM 1
    )
endforeach()
