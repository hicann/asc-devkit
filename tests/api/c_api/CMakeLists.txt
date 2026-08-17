# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_c_api_tests)
cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 17)
find_package(tikicpulib REQUIRED)

add_subdirectory(ascendc_host_header_checker)

set(ASCENDC_UT_PRODUCT_TYPE_LIST
    ascend910B1_AIC
    ascend910B1_AIV
    ascend950pr_9599_AIC
    ascend950pr_9599_AIV
)

set(ASCENDC_UT_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${ASCENDC_DIR}
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/include/c_api
    ${ASCENDC_DIR}/impl/c_api
    ${ASCENDC_DIR}/
)

file(GLOB ASCENDC_UT_COMMON_SRC
    ../common/common.cpp
    ../common/alog_stub.cpp
    ../common/dlog_stub.cpp
)

file(GLOB ASCENDC_UT_ascend910B1_AIC_CASE_SRC
    npu_arch_2201/cube_compute/*.cpp
    npu_arch_2201/cube_datamove/*.cpp
)

file(GLOB ASCENDC_UT_ascend910B1_AIV_CASE_SRC
    npu_arch_2201/simd_atomic/*.cpp
    npu_arch_2201/cache_ctrl/*.cpp
    npu_arch_2201/sync/*.cpp
    npu_arch_2201/sys_var/*.cpp
    npu_arch_2201/vector_compute/*.cpp
    npu_arch_2201/vector_datamove/*.cpp
    npu_arch_2201/scalar_compute/*.cpp
    npu_arch_2201/misc/*.cpp
    npu_arch_2201/cache_ctrl/*.cpp
)

file(GLOB ASCENDC_UT_ascend950pr_9599_AIC_CASE_SRC
    npu_arch_3510/cube_compute/*.cpp
    npu_arch_3510/cube_datamove/*.cpp
    npu_arch_3510/sys_var/test_asc_get_pipe_idle_slot_count.cpp
)

file(GLOB ASCENDC_UT_ascend950pr_9599_AIV_CASE_SRC
    npu_arch_3510/atomic/*.cpp
    npu_arch_3510/cache_ctrl/*.cpp
    npu_arch_3510/misc/*.cpp
    npu_arch_3510/scalar_compute/*.cpp
    npu_arch_3510/sync/*.cpp
    npu_arch_3510/sys_var/*.cpp
    npu_arch_3510/vector_compute/*.cpp
    npu_arch_3510/vector_datamove/*.cpp
)

foreach(product_type ${ASCENDC_UT_PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_c_api_${product_type}
        ${ASCENDC_DIR}/tests/main.cpp
        ${ASCENDC_UT_COMMON_SRC}
        ../common/k3_pvwrap.cpp
        ${ASCENDC_UT_${product_type}_CASE_SRC}
    )

    target_compile_definitions(ascendc_ut_c_api_${product_type} PRIVATE
        UT_TEST
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_CUBE__;__DAV_CUBE__>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_VEC__;__DAV_VEC__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_CUBE__;__DAV_CUBE__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_VEC__;__DAV_VEC__>
        __disable_kernel_type_autoinfer__=
    )

    target_include_directories(ascendc_ut_c_api_${product_type} PRIVATE
        ${ASCENDC_UT_HEADER}
        ${ASCEND_CANN_PACKAGE_PATH}/include/base/
        ${ASCEND_CANN_PACKAGE_PATH}/include/metadef/
    )

    target_compile_options(ascendc_ut_c_api_${product_type} PRIVATE
        -fno-access-control
        -Werror
        -UASCENDC_DEBUG
    )

    target_link_libraries(ascendc_ut_c_api_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:tikicpulib::ascend910B1>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:tikicpulib::ascend910B1>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV>:tikicpulib::ascend950pr_9599>
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_c_api_${product_type}
        TASK_NUM 1
    )
endforeach()
