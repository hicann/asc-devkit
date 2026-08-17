# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_tools_aclrtc_tests)
cmake_minimum_required(VERSION 3.16.0)

set(ACLRTC_TEST_HEADERS_FILES
    ${ASCENDC_DIR}/tools/include
)

file(GLOB ACLRTC_SRC_FILES
    ${ASCENDC_DIR}/tools/aclrtc/*.cpp
)

set(ACLRTC_RESOURCE_REGISTRY_SRC_FILES
    ${ASCENDC_DIR}/tools/aclrtc/specialization/resource_registry.cpp
    ${ASCENDC_DIR}/tools/build/common/file_utils.cpp
)

file(GLOB TEST_ACLRTC_SRC_FILES
    ${ASCENDC_DIR}/tests/main.cpp
    stub/*.cpp
    *.cpp
)

add_executable(ascendc_ut_aclrtc
    ${ACLRTC_SRC_FILES}
    ${ACLRTC_RESOURCE_REGISTRY_SRC_FILES}
    ${TEST_ACLRTC_SRC_FILES}
)

target_compile_definitions(ascendc_ut_aclrtc PRIVATE
    UT_TEST
    ASCENDC_CPU_DEBUG=1
)

target_include_directories(ascendc_ut_aclrtc PRIVATE
    ${ACLRTC_TEST_HEADERS_FILES}
    ${ASCENDC_DIR}/tools/aclrtc/specialization
    ${ASCENDC_DIR}/tools/build/common
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCEND_CANN_PACKAGE_PATH}/include/acl
)

target_compile_options(ascendc_ut_aclrtc PRIVATE
    -Werror
    -fno-access-control
)

target_link_directories(ascendc_ut_aclrtc PRIVATE
    ${ASCEND_DEP_LIB_DIR}
)

target_link_libraries(ascendc_ut_aclrtc PRIVATE
    $<BUILD_INTERFACE:intf_llt_pub_basic>
    $<BUILD_INTERFACE:mmpa_headers>
    $<BUILD_INTERFACE:metadef_headers>
    Boost::filesystem
    json
    -Wl,--no-as-needed
    c_sec
    dl
    -Wl,--as-needed
    GTest::gtest
)

run_llt_test(
    TARGET ascendc_ut_aclrtc
    TASK_NUM 1
)
