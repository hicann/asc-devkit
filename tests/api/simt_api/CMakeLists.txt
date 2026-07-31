# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_simt_api_tests)
cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 17)
find_package(tikicpulib REQUIRED)

set(SIMT_PRODUCT_TYPE_LIST_INIT ascend950pr_9599)
set(SIMT_PRODUCT_TYPE_LIST ${SIMT_PRODUCT_TYPE_LIST_INIT} CACHE STRING "Supported product types")

set(ASCENDC_TEST_HEADER_FILES
    ../common
    common_simt
    ${ASCENDC_DIR}
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/include/basic_api
    ${ASCENDC_DIR}/include/simt_api
    ${ASCENDC_DIR}/include/utils
    ${ASCENDC_DIR}/impl
    ${ASCENDC_DIR}/impl/basic_api
    ${ASCENDC_DIR}/impl/simt_api
)

# # ================ simt_api kernel ut ================
# ascend950pr_9599 simt test cases
file(GLOB ASCENDC_TEST_ascend950pr_9599_SIMT_CASE_SRC_FILES
    common_simt/*.cpp
    ascendc_case_ascend950pr_9599_simt/*.cpp
)

# mc62cm12aa simt test cases
file(GLOB ASCENDC_TEST_mc62cm12aa_SIMT_CASE_SRC_FILES
    common_simt/*.cpp
    ascendc_case_mc62cm12aa_simt/*.cpp
)

foreach(product_type ${SIMT_PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_simt_api_${product_type}
        ${ASCENDC_DIR}/tests/main.cpp
        ../common/k3_pvwrap.cpp
        ${ASCENDC_TEST_${product_type}_SIMT_CASE_SRC_FILES}
    )

    target_compile_definitions(ascendc_ut_simt_api_${product_type} PRIVATE
        UT_TEST
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        __CCE_KT_TEST__=1
        ASCENDC_CPU_DEBUG=1
        ASCENDC_DEBUG=1
        $<$<STREQUAL:${product_type},ascend950pr_9599>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_VEC__>
        $<$<STREQUAL:${product_type},mc62cm12aa>:__NPU_ARCH__=5102>
    )

    target_include_directories(ascendc_ut_simt_api_${product_type} PRIVATE
        ${ASCENDC_TEST_HEADER_FILES}
        ${ASCEND_CANN_PACKAGE_PATH}/tools/tikicpulib/lib/include
        ${ASCEND_CANN_PACKAGE_PATH}/include/base/
        ${ASCEND_CANN_PACKAGE_PATH}/include/metadef/
    )

    target_compile_options(ascendc_ut_simt_api_${product_type} PRIVATE
        -fno-access-control
    )

    target_link_libraries(ascendc_ut_simt_api_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        kernel_tiling
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_simt_api_${product_type}
        TASK_NUM 1
    )
    add_subdirectory(math)
    add_dependencies(ascendc_ut_simt_api_${product_type} math)
    add_subdirectory(math_half)
    add_dependencies(ascendc_ut_simt_api_${product_type} math_half)
    add_subdirectory(math_bfloat16)
    add_dependencies(ascendc_ut_simt_api_${product_type} math_bfloat16)
    add_subdirectory(math_cpp)
    add_dependencies(ascendc_ut_simt_api_${product_type} math_cpp)
endforeach()
