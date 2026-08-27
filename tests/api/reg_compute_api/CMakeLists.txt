# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_reg_compute_tests)
cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 17)
find_package(tikicpulib REQUIRED)

add_subdirectory(ascendc_reg_header_checker)

set(MICRO_PRODUCT_TYPE_LIST_INIT ascend950pr_9599)
set(MICRO_PRODUCT_TYPE_LIST ${MICRO_PRODUCT_TYPE_LIST_INIT} CACHE STRING "Supported product types")

set(ASCENDC_TEST_HEADER_FILES
    ../common
    ${ASCENDC_DIR}
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/include/basic_api
    ${ASCENDC_DIR}/include/basic_api/reg_compute
    ${ASCENDC_DIR}/include/utils
    ${ASCENDC_DIR}/impl
    ${ASCENDC_DIR}/impl/basic_api
    ${ASCENDC_DIR}/impl/basic_api/reg_compute
)

# # ================ reg_compute_api kernel ut ================
# ascend950pr_9599 micro test cases
file(GLOB ASCENDC_TEST_ascend950pr_9599_MICRO_CASE_SRC_FILES
    ascendc_case_ascend950pr_9599_reg_compute/*.cpp
)

# mc62cm12aa micro test cases
file(GLOB ASCENDC_TEST_mc62cm12aa_MICRO_CASE_SRC_FILES
    ascendc_case_mc62cm12aa_reg_compute/*.cpp
)

foreach(product_type ${MICRO_PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_reg_compute_${product_type}
        ${ASCENDC_DIR}/tests/main_global.cpp
        ../common/k3_pvwrap.cpp
        ${ASCENDC_TEST_${product_type}_MICRO_CASE_SRC_FILES}
    )

    target_compile_definitions(ascendc_ut_reg_compute_${product_type} PRIVATE
        UT_TEST
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        $<$<STREQUAL:${product_type},ascend950pr_9599>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_VEC__>
        $<$<STREQUAL:${product_type},mc62cm12aa>:__NPU_ARCH__=5102>
        __callee__=
    )

    target_include_directories(ascendc_ut_reg_compute_${product_type} PRIVATE
        ${ASCENDC_TEST_HEADER_FILES}
        ${ASCEND_CANN_PACKAGE_PATH}/include/base/
        ${ASCEND_CANN_PACKAGE_PATH}/include/metadef/
    )

    target_compile_options(ascendc_ut_reg_compute_${product_type} PRIVATE
        -fno-access-control
        -UASCENDC_DEBUG
    )

    target_link_libraries(ascendc_ut_reg_compute_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        $<$<STREQUAL:${product_type},ascend950pr_9599>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},mc62cm12aa>:tikicpulib::mc62cm12aa>
        kernel_tiling
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_reg_compute_${product_type}
        TASK_NUM 1
    )
endforeach()
