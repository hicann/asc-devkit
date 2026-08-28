# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set(ASCENDC_UT_RUNTIME_VALIDATION_COMMON_SRC
    ${ASCENDC_DIR}/tests/main.cpp
    ${ASCENDC_DIR}/tests/api/common/common.cpp
    ${ASCENDC_DIR}/tests/api/common/alog_stub.cpp
    ${ASCENDC_DIR}/tests/api/common/dlog_stub.cpp
    ${ASCENDC_DIR}/tests/api/common/k3_pvwrap.cpp
)

set(ASCENDC_UT_RUNTIME_VALIDATION_NPU_ARCH_3510_AIC_CASE_SRC
    test_tensor_api_cube_runtime_validation.cpp
    test_tensor_api_cube_in_additional.cpp
    test_tensor_api_cube_in_operand_layout.cpp
    test_tensor_api_cube_out_additional.cpp
)

set(ASCENDC_UT_RUNTIME_VALIDATION_NPU_ARCH_3510_AIV_CASE_SRC
    test_tensor_api_runtime_validation.cpp
    test_tensor_api_data_structure_additional.cpp
    test_tensor_api_data_structure_matrix.cpp
    test_tensor_api_data_structure_strict.cpp
    test_tensor_api_vector_copy_abnormal.cpp
)

set(ASCENDC_UT_RUNTIME_VALIDATION_PRODUCT_TYPE_LIST
    NPU_ARCH_3510_AIC
    NPU_ARCH_3510_AIV
)

foreach(product_type ${ASCENDC_UT_RUNTIME_VALIDATION_PRODUCT_TYPE_LIST})
    set(target_name ascendc_ut_tensor_api_runtime_validation_${product_type})
    add_executable(${target_name}
        ${ASCENDC_UT_RUNTIME_VALIDATION_COMMON_SRC}
        ${ASCENDC_UT_RUNTIME_VALIDATION_${product_type}_CASE_SRC}
    )

    target_compile_definitions(${target_name} PRIVATE
        UT_TEST
        CANN_ASC_USE_EXPERIMENTAL
        ASCENDC_DEBUG=1
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        __NPU_ARCH__=3510
        __disable_kernel_type_autoinfer__=
    )

    target_include_directories(${target_name} BEFORE PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stub
        ${CMAKE_CURRENT_LIST_DIR}
        ${ASCENDC_UT_HEADER}
        ${ASCEND_CANN_PACKAGE_PATH}/include/base/
        ${ASCEND_CANN_PACKAGE_PATH}/include/metadef/
    )

    target_compile_options(${target_name} PRIVATE
        -fno-access-control
        -Werror
    )

    target_link_libraries(${target_name} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        tikicpulib::ascend950pr_9599
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ${target_name}
        TASK_NUM 1
    )
endforeach()
