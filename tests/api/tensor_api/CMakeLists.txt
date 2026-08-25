# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_tensor_api_tests)
cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 17)
find_package(tikicpulib REQUIRED)

add_subdirectory(ascendc_tensor_api_header_checker)

set(ASCENDC_UT_PRODUCT_TYPE_LIST
    NPU_ARCH_3510_AIC
    NPU_ARCH_3510_AIV
)

set(ASCENDC_UT_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${ASCENDC_DIR}/
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/include/c_api
    ${ASCENDC_DIR}/include/utils
    ${ASCENDC_DIR}/impl
    ${ASCENDC_DIR}/impl/c_api
)

file(GLOB CONFIGURE_DEPENDS ASCENDC_UT_COMMON_SRC
    ../common/common.cpp
    ../common/alog_stub.cpp
    ../common/dlog_stub.cpp
    ../common/k3_pvwrap.cpp
)

file(GLOB ASCENDC_UT_BASIC_STRUCT_CASE_SRC
    basic_struct/layout/*.cpp
    basic_struct/pointer/*.cpp
    basic_struct/tensor/*.cpp
)

file(GLOB ASCENDC_UT_NPU_ARCH_3510_AIC_CASE_SRC
    npu_arch_3510/aic/*.cpp
)

file(GLOB ASCENDC_UT_NPU_ARCH_3510_AIV_CASE_SRC
    npu_arch_3510/aiv/*.cpp
)

foreach(product_type ${ASCENDC_UT_PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_tensor_api_${product_type}
        ${ASCENDC_DIR}/tests/main.cpp
        ${ASCENDC_UT_COMMON_SRC}
        ${ASCENDC_UT_BASIC_STRUCT_CASE_SRC}
        ${ASCENDC_UT_${product_type}_CASE_SRC}
    )

    target_compile_definitions(ascendc_ut_tensor_api_${product_type} PRIVATE
        UT_TEST
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        $<$<STREQUAL:${product_type},NPU_ARCH_3510_AIC>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},NPU_ARCH_3510_AIV>:__NPU_ARCH__=3510>
        __disable_kernel_type_autoinfer__=
    )

    target_include_directories(ascendc_ut_tensor_api_${product_type} PRIVATE
        ${ASCENDC_UT_HEADER}
        ${ASCEND_CANN_PACKAGE_PATH}/include/base/
        ${ASCEND_CANN_PACKAGE_PATH}/include/metadef/
    )

    target_compile_options(ascendc_ut_tensor_api_${product_type} PRIVATE
        -fno-access-control
        -Werror
    )

    target_link_libraries(ascendc_ut_tensor_api_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        $<$<STREQUAL:${product_type},NPU_ARCH_3510_AIC>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},NPU_ARCH_3510_AIV>:tikicpulib::ascend950pr_9599>
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_tensor_api_${product_type}
        TASK_NUM 1
    )
endforeach()
