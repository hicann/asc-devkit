# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_tools_utils_tests)
cmake_minimum_required(VERSION 3.16.0)

# ================ pack_kernel ================
add_executable(ascendc_ut_pack_kernel
    ${ASCENDC_DIR}/tests/main.cpp
    ${ASCENDC_DIR}/tools/build/elf_tool/ascendc_elf_tool.c
    ${ASCENDC_DIR}/tools/build/asc_pack_kernel/ascendc_pack_kernel.c
    asc_pack_kernel/test_pack_kernel.cpp
)

target_compile_definitions(ascendc_ut_pack_kernel PRIVATE
    UT_TEST
    ASCENDC_CPU_DEBUG=1
)

target_include_directories(ascendc_ut_pack_kernel PRIVATE
    ${ASCENDC_DIR}/tools/build/common
    ${ASCENDC_DIR}/tools/build/elf_tool
    ${ASCENDC_DIR}/tools/build/asc_pack_kernel
    ${ASCEND_CANN_PACKAGE_PATH}/include/base
)

target_compile_options(ascendc_ut_pack_kernel PRIVATE
    -Werror
)

target_link_directories(ascendc_ut_pack_kernel PRIVATE
    ${ASCEND_CANN_PACKAGE_PATH}/lib64/
)

target_link_libraries(ascendc_ut_pack_kernel PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    -Wl,--no-as-needed
    error_manager
    mmpa
    c_sec
    -Wl,--as-needed
)

run_llt_test(
    TARGET ascendc_ut_pack_kernel
    TASK_NUM 1
)

# ================ elf_tool ================
add_executable(ascendc_ut_elf_tool
    ${ASCENDC_DIR}/tests/main.cpp
    ${ASCENDC_DIR}/tools/build/elf_tool/ascendc_elf_tool.c
    elf_tool/test_elf_tool.cpp
)

target_compile_definitions(ascendc_ut_elf_tool PRIVATE
    UT_TEST
    ASCENDC_CPU_DEBUG=1
)

target_include_directories(ascendc_ut_elf_tool PRIVATE
    ${ASCENDC_DIR}/tools/build/common
    ${ASCENDC_DIR}/tools/build/elf_tool
    ${ASCEND_CANN_PACKAGE_PATH}/include/base
)
target_compile_options(ascendc_ut_elf_tool PRIVATE
    -Werror
)

target_link_directories(ascendc_ut_elf_tool PRIVATE
    ${ASCEND_CANN_PACKAGE_PATH}/lib64/
)

target_link_libraries(ascendc_ut_elf_tool PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    -Wl,--no-as-needed
    error_manager
    mmpa
    c_sec
    -Wl,--as-needed
)

run_llt_test(
    TARGET ascendc_ut_elf_tool
    TASK_NUM 1
)

# ================ asc_rt ================
add_executable(ascendc_ut_asc_runtime
    ${ASCENDC_DIR}/tests/main.cpp
    ${ASCENDC_DIR}/tools/build/elf_tool/ascendc_elf_tool.c
    ${ASCENDC_DIR}/tools/build/asc_rt/aicpu_rt.cpp
    ${ASCENDC_DIR}/tools/build/asc_rt/ascendc_runtime.cpp
    stub/acl_rt_stub.cpp
    stub/kernel_stub.cpp
    stub/mem_stub.cpp
    stub/prof_api_stub.cpp
    stub/rt_ffts_stub.cpp
    asc_rt/test_aicpu_rt.cpp
    asc_rt/test_ascendc_runtime.cpp
)

target_compile_definitions(ascendc_ut_asc_runtime PRIVATE
    UT_TEST
    ASCENDC_CPU_DEBUG=1
)

target_include_directories(ascendc_ut_asc_runtime PRIVATE
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/tools/build/common
    ${ASCENDC_DIR}/tools/build/asc_rt
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCEND_CANN_PACKAGE_PATH}/include/base
    ${ASCEND_CANN_PACKAGE_PATH}/include/acl
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/profiling
)

target_compile_options(ascendc_ut_asc_runtime PRIVATE
    -Werror
)

target_link_directories(ascendc_ut_asc_runtime PRIVATE
    ${ASCEND_CANN_PACKAGE_PATH}/lib64/
)

target_link_libraries(ascendc_ut_asc_runtime PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    -Wl,--no-as-needed
    error_manager
    mmpa
    c_sec
    -Wl,--as-needed
)

run_llt_test(
    TARGET ascendc_ut_asc_runtime
    TASK_NUM 1
)

# ================ asc_op_compile_base ================
set(ASC_COMPILE_BASE_TEST_HEADERS_FILES
    ${ASCENDC_DIR}/tools/build/common/
    ${ASCENDC_DIR}/tools/build/asc_op_compile_base/c_api/
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base/
)

file(GLOB ASC_COMPILE_BASE_SRC_FILES
    ${ASCENDC_DIR}/tools/build/asc_op_compile_base/c_api/*.cpp
    ${ASCENDC_DIR}/tools/build/asc_op_compile_base/c_api/*.cc
)

file(GLOB TEST_ASC_COMPILE_BASE_SRC_FILES
    ${ASCENDC_DIR}/tests/main.cpp
    asc_op_compile_base/test_c_api.cpp
)

add_executable(ascendc_ut_asc_compile_base
    ${ASC_COMPILE_BASE_SRC_FILES}
    ${TEST_ASC_COMPILE_BASE_SRC_FILES}
)

target_compile_definitions(ascendc_ut_asc_compile_base PRIVATE
    UT_TEST
    ASCENDC_CPU_DEBUG=1
)

target_include_directories(ascendc_ut_asc_compile_base PRIVATE
    ${ASC_COMPILE_BASE_TEST_HEADERS_FILES}
)

target_compile_options(ascendc_ut_asc_compile_base PRIVATE
    -Werror
)

target_link_libraries(ascendc_ut_asc_compile_base PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    platform
    -Wl,--no-as-needed
    error_manager
    mmpa
    c_sec
    -Wl,--as-needed
)

run_llt_test(
    TARGET ascendc_ut_asc_compile_base
    TASK_NUM 1
)

# ================ asc_compile_exporter ================
add_executable(ascendc_ut_asc_compile_exporter
    ${ASCENDC_DIR}/tests/main.cpp
    asc_compile_exporter/test_asc_compile_exporter_unit.cpp
    asc_compile_exporter/test_asc_compile_exporter.cpp
    asc_compile_exporter/test_bundle_source_generator.cpp
    asc_compile_exporter/test_collected_manifest_repository.cpp
    asc_compile_exporter/test_file_utils.cpp
    asc_compile_exporter/test_manifest_bundle_types.cpp
    common/test_process_executor.cpp
    asc_compile_exporter/test_resource_manifest_validator.cpp
    common/test_directory_cleanup_guard.cpp
    asc_compile_exporter/test_ascendc_tool_log.cpp
    ${ASCENDC_DIR}/tools/build/asc_compile_exporter/bundle_source_generator.cpp
    ${ASCENDC_DIR}/tools/build/asc_compile_exporter/collected_manifest_repository.cpp
    ${ASCENDC_DIR}/tools/build/asc_compile_exporter/manifest_bundle_compiler.cpp
    ${ASCENDC_DIR}/tools/build/asc_compile_exporter/resource_manifest_validator.cpp
    ${ASCENDC_DIR}/tools/build/common/file_utils.cpp
    ${ASCENDC_DIR}/tools/build/common/process_executor.cpp
    ${ASCENDC_DIR}/tools/build/common/directory_cleanup_guard.cpp
)

function(configure_asc_compile_exporter_test target_name)
    target_compile_definitions(${target_name} PRIVATE
        UT_TEST
    )

    target_include_directories(${target_name} PRIVATE
        ${ASCENDC_DIR}/tools/build/asc_compile_exporter
        ${ASCENDC_DIR}/tools/build/common
        ${ASC_COMPILE_EXPORTER_GENERATED_DIR}
    )

    target_compile_options(${target_name} PRIVATE
        -Werror
    )

    target_link_options(${target_name} PRIVATE
        -Wl,--wrap=rename
    )

    target_link_libraries(${target_name} PRIVATE
        Boost::filesystem
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        json
        mmpa_headers
        metadef_headers
        dl
    )
endfunction()

configure_asc_compile_exporter_test(ascendc_ut_asc_compile_exporter)

run_llt_test(
    TARGET ascendc_ut_asc_compile_exporter
    TASK_NUM 1
)


# ###################################opbuild ut#################################
add_library(ops_utest SHARED ${ASCENDC_DIR}/tests/tools/utils/opbuild/test_ops.cpp)

target_include_directories(ops_utest PRIVATE
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCENDC_TEST_HEADER_FILES}
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base/
    ${ASCEND_CANN_PACKAGE_PATH}/include/base/
    ${ASCENDC_DIR}/tools/build/common/
    ${ASCEND_CANN_PACKAGE_PATH}/tools/msopgen/template/custom_operator_sample/DSL/Onnx/metadef/inc
    mmpa_headers
    slog_headers
)

target_link_directories(ops_utest PRIVATE
    ${ASCEND_DEP_LIB_DIR}
)

target_link_libraries(ops_utest PRIVATE
    -Wl,--no-as-needed
    c_sec graph graph_base register
    -Wl,--as-needed
)

add_custom_target(utest_env
    COMMAND echo "export OPS_DSO_FILE_PATH=$<TARGET_FILE:ops_utest>" > ${ASCENDC_DIR}/build/tests/tools/utils/opbuild_env.sh
    COMMAND echo "export OPS_SRC_FILE_PATH=${ASCENDC_DIR}/tests/tools/utils/opbuild" >> ${ASCENDC_DIR}/build/tests/tools/utils/opbuild_env.sh
)

add_executable(ascendc_ut_asc_tikcpp_utest_opbuild
    ${ASCENDC_DIR}/tests/main.cpp
    ${ASCENDC_DIR}/tests/tools/utils/opbuild/test_opbuild.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_build.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_build_params.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_generator.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_generator_factory.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_cfg_generator.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_cpu_cfg_generator.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_proto_generator.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_aclnn_generator.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_custom_registry_generator.cpp
    ${ASCENDC_DIR}/tools/build/opbuild/op_aclnn_fallback_generator.cpp
)

add_dependencies(ascendc_ut_asc_tikcpp_utest_opbuild ops_utest utest_env)

target_compile_definitions(ascendc_ut_asc_tikcpp_utest_opbuild PRIVATE
    UT_TEST
)

target_compile_options(ascendc_ut_asc_tikcpp_utest_opbuild PRIVATE
    -g
    -Werror
    -Wno-unused-function
)

target_include_directories(ascendc_ut_asc_tikcpp_utest_opbuild PRIVATE
    ${ASCENDC_DIR}/tools/build/opbuild
    ${ASCENDC_DIR}/tools/build/common/
    ${ASCENDC_TEST_HEADER_FILES}
    ${ASCEND_CANN_PACKAGE_PATH}/include/base/
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base/
    ${ASCEND_CANN_PACKAGE_PATH}/include
    mmpa_headers
    slog_headers
    ${ASCEND_CANN_PACKAGE_PATH}/tools/msopgen/template/custom_operator_sample/DSL/Onnx/metadef/inc
)

target_link_directories(ascendc_ut_asc_tikcpp_utest_opbuild PRIVATE
    ${ASCEND_DEP_LIB_DIR}
)

target_link_libraries(ascendc_ut_asc_tikcpp_utest_opbuild PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    -Wl,--no-as-needed
    c_sec graph graph_base register dl
    -Wl,--as-needed
)

run_llt_test(
    TARGET ascendc_ut_asc_tikcpp_utest_opbuild
    ENV_FILE ${ASCENDC_DIR}/build/tests/tools/utils/opbuild_env.sh
    TASK_NUM 1
)
