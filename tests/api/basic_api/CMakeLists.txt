# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_basic_api_tests)
cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 17)
find_package(tikicpulib REQUIRED)

add_subdirectory(ascendc_header_checker)

add_subdirectory(ascendc_host_header_checker)

set(BASIC_UT_PRODUCT_TYPE_LIST_INIT
    ascend910
    ascend610
    ascend310p
    ascend910B1_AIC
    ascend910B1_AIV
    ascend910B1_AIV_MSTX
    ascend310B1
    ascend950pr_9599_AIC
    ascend950pr_9599_AIV_BASIC
    ascend950pr_9599_AIV_FRAMEWORK
)
set(BASIC_UT_PRODUCT_TYPE_LIST ${BASIC_UT_PRODUCT_TYPE_LIST_INIT} CACHE STRING "Supported product types")

set(ASCENDC_TEST_HEADER_FILES
    common
    ../common
    ${ASCENDC_DIR}
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/include/basic_api
    ${ASCENDC_DIR}/include/utils
    ${ASCENDC_DIR}/impl
    ${ASCENDC_DIR}/impl/basic_api
)

# common ut src files
file(GLOB ASCENDC_TEST_COMMON_SRC_FILES
    ../common/common.cpp
    ../common/alog_stub.cpp
    ../common/dlog_stub.cpp
)

# common test cases
file(GLOB ASCENDC_TEST_COMMON_CASE_SRC_FILES
    ascendc_case_common/*.cpp
)

# ascend910 test cases
file(GLOB ASCENDC_TEST_ascend910_CASE_SRC_FILES
    ascendc_case_ascend910/*.cpp
)

# ascend610 test cases
file(GLOB ASCENDC_TEST_ascend610_CASE_SRC_FILES
    ascendc_case_ascend610/*.cpp
)

# ascend310p test cases
file(GLOB ASCENDC_TEST_ascend310p_CASE_SRC_FILES
    ascendc_case_ascend310p/*.cpp
)

# ascend910B1 aiv test cases
file(GLOB ASCENDC_TEST_ascend910B1_AIV_CASE_SRC_FILES
    ascendc_case_common/test_data_copy_slice.cpp
    ascendc_case_common/test_disabled_mode_sys_constants.cpp
    ascendc_case_common/test_sys_constants.cpp
    ascendc_case_common/test_tscm_data.cpp
    ascendc_case_ascend910b1/ascendc_case_ascend910b1_aiv/*.cpp
)

# ascend910B1 aiv mstx test cases
file(GLOB ASCENDC_TEST_ascend910B1_AIV_MSTX_CASE_SRC_FILES
    ascendc_case_ascend910b1/ascendc_case_ascend910b1_aiv_mstx/test_operator_vec_mstx.cpp
)

# ascend910B1 aic test cases
file(GLOB ASCENDC_TEST_ascend910B1_AIC_CASE_SRC_FILES
    ascendc_case_ascend910b1/ascendc_case_ascend910b1_aic/*.cpp
)

# ascend310B1 test cases
file(GLOB ASCENDC_TEST_ascend310B1_CASE_SRC_FILES
    ascendc_case_common/test_kernel_pop_stack_buffer.cpp
    ascendc_case_ascend310b1/*.cpp
)

# ascend610Lite test cases
file(GLOB ASCENDC_TEST_ascend610Lite_CASE_SRC_FILES
    ascendc_case_common/test_kernel_pop_stack_buffer.cpp
    ascendc_case_ascend610lite/*.cpp
)

# ascend950pr_9599 basic test cases
file(GLOB ASCENDC_TEST_ascend950pr_9599_AIV_BASIC_CASE_SRC_FILES
    ascendc_case_common/test_disabled_mode_sys_constants.cpp
    ascendc_case_common/test_sys_constants.cpp
    ascendc_case_common/test_kernel_pop_stack_buffer.cpp
    ascendc_case_common/test_kernel_tensor.cpp
    ascendc_case_common/test_type_check.cpp
    ascendc_case_ascend950pr_9599/test_pipe_framework.cpp
    ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aiv_basic/*.cpp
)

set(ASCENDC_TEST_ascend950pr_9599_AIV_BASIC_PRINTF_CASE_FILE
    ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aiv_basic/test_operator_printf.cpp
)
# Keep target-level ASCENDC_DUMP=0; enable it only for this source file.
set_source_files_properties(${ASCENDC_TEST_ascend950pr_9599_AIV_BASIC_PRINTF_CASE_FILE} PROPERTIES
    COMPILE_OPTIONS "-UASCENDC_DUMP;-DASCENDC_DUMP=1"
)

# ascend950pr_9599 framework test cases
file(GLOB ASCENDC_TEST_ascend950pr_9599_AIV_FRAMEWORK_CASE_SRC_FILES
    ascendc_case_common/test_kernel_pop_stack_buffer.cpp
    ascendc_case_common/test_kernel_tensor.cpp
    ascendc_case_common/test_type_check.cpp
    ascendc_case_ascend950pr_9599/test_pipe_framework.cpp
    ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aiv_framework/*.cpp
)

# ascend950pr_9599 aic test cases
file(GLOB ASCENDC_TEST_ascend950pr_9599_AIC_CASE_SRC_FILES
    ascendc_case_common/test_kernel_tensor.cpp
    ascendc_case_common/test_type_check.cpp
    ascendc_case_ascend950pr_9599/test_pipe_framework.cpp
    ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aic/*.cpp
)

# mc62cm12aa basic test cases
file(GLOB ASCENDC_TEST_mc62cm12aa_BASIC_CASE_SRC_FILES
    ascendc_case_common/test_kernel_pop_stack_buffer.cpp
    ascendc_case_common/test_kernel_tensor.cpp
    ascendc_case_common/test_type_check.cpp
    ascendc_case_mc62cm12aa/ascendc_case_mc62cm12aa_basic/*.cpp
)

# mc62cm12aa framework test cases
file(GLOB ASCENDC_TEST_mc62cm12aa_FRAMEWORK_CASE_SRC_FILES
    ascendc_case_common/test_kernel_pop_stack_buffer.cpp
    ascendc_case_common/test_kernel_tensor.cpp
    ascendc_case_common/test_type_check.cpp
    ascendc_case_mc62cm12aa/ascendc_case_mc62cm12aa_framework/*.cpp
)

# mc62cm12aa aic test cases
file(GLOB ASCENDC_TEST_mc62cm12aa_AIC_CASE_SRC_FILES
    ascendc_case_common/test_kernel_tensor.cpp
    ascendc_case_common/test_type_check.cpp
    ascendc_case_mc62cm12aa/ascendc_case_mc62cm12aa_aic/*.cpp
)

foreach(product_type ${BASIC_UT_PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_basic_api_${product_type}
        ${ASCENDC_DIR}/tests/main_global.cpp
        ${ASCENDC_TEST_COMMON_SRC_FILES}
        $<$<STREQUAL:${product_type},ascend910>:${ASCENDC_TEST_COMMON_CASE_SRC_FILES}>
        $<$<STREQUAL:${product_type},ascend910>:../common/tik_pv_wrapper.cpp>
        $<$<STREQUAL:${product_type},ascend610>:${ASCENDC_TEST_COMMON_CASE_SRC_FILES}>
        $<$<STREQUAL:${product_type},ascend610>:../common/tik_pv_wrapper.cpp>
        $<$<STREQUAL:${product_type},ascend310p>:${ASCENDC_TEST_COMMON_CASE_SRC_FILES}>
        $<$<STREQUAL:${product_type},ascend310p>:../common/tik_pv_wrapper.cpp>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend910B1_AIV_MSTX>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend310B1>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend610Lite>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_BASIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_FRAMEWORK>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},mc62cm12aa_AIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},mc62cm12aa_BASIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},mc62cm12aa_FRAMEWORK>:../common/k3_pvwrap.cpp>
        ${ASCENDC_TEST_${product_type}_CASE_SRC_FILES}
        $<$<STREQUAL:${product_type},ascend910B1_AIV_MSTX>:${ASCENDC_TEST_ascend910B1_AIV_MSTX_CASE_SRC_FILES}>
    )

    target_compile_definitions(ascendc_ut_basic_api_${product_type} PRIVATE
        UT_TEST
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        $<$<STREQUAL:${product_type},ascend910>:__NPU_ARCH__=1001;__DAV_C100__>
        $<$<STREQUAL:${product_type},ascend310p>:__NPU_ARCH__=2002;__DAV_M200__>
        $<$<STREQUAL:${product_type},ascend610>:__NPU_ARCH__=2002;__DAV_M200__>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_CUBE__;__DAV_CUBE__>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_VEC__;__DAV_VEC__>
        $<$<STREQUAL:${product_type},ascend910B1_AIV_MSTX>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_VEC__;__DAV_VEC__>
        $<$<STREQUAL:${product_type},ascend310B1>:__NPU_ARCH__=3002;__DAV_M300__>
        $<$<STREQUAL:${product_type},ascend610Lite>:__NPU_ARCH__=3102;__DAV_M310__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_CUBE__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_BASIC>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_VEC__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_FRAMEWORK>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_VEC__>
        $<$<STREQUAL:${product_type},mc62cm12aa_AIC>:__NPU_ARCH__=5102>
        $<$<STREQUAL:${product_type},mc62cm12aa_BASIC>:__NPU_ARCH__=5102>
        $<$<STREQUAL:${product_type},mc62cm12aa_FRAMEWORK>:__NPU_ARCH__=5102>
        __disable_kernel_type_autoinfer__=
        __callee__=
    )

    target_include_directories(ascendc_ut_basic_api_${product_type} PRIVATE
        ${ASCENDC_TEST_HEADER_FILES}
        ${ASCEND_CANN_PACKAGE_PATH}/include/base/
        ${ASCEND_CANN_PACKAGE_PATH}/include/metadef/
    )

    target_compile_options(ascendc_ut_basic_api_${product_type} PRIVATE
        -fno-access-control
        -Werror
        -UASCENDC_DEBUG
    )

    target_link_libraries(ascendc_ut_basic_api_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        kernel_tiling
        $<$<STREQUAL:${product_type},ascend910>:tikicpulib::ascend910>
        $<$<STREQUAL:${product_type},ascend610>:tikicpulib::ascend610>
        $<$<STREQUAL:${product_type},ascend310p>:tikicpulib::ascend310p>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:tikicpulib::ascend910B1>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:tikicpulib::ascend910B1>
        $<$<STREQUAL:${product_type},ascend910B1_AIV_MSTX>:tikicpulib::ascend910B1>
        $<$<STREQUAL:${product_type},ascend310B1>:tikicpulib::ascend310B1>
        $<$<STREQUAL:${product_type},ascend610Lite>:tikicpulib::ascend610Lite>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_BASIC>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV_FRAMEWORK>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},mc62cm12aa_AIC>:tikicpulib::mc62cm12aa>
        $<$<STREQUAL:${product_type},mc62cm12aa_BASIC>:tikicpulib::mc62cm12aa>
        $<$<STREQUAL:${product_type},mc62cm12aa_FRAMEWORK>:tikicpulib::mc62cm12aa>
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_basic_api_${product_type}
        TASK_NUM 1
    )
endforeach()
