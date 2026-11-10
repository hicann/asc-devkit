# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.16.0)
project(parallel_ops_package_proj)
include(ExternalProject)

set(INSTALL_TARGET "package")
set(NLOHMANN_JSON_URL
    "https://gitcode.com/cann-src-third-party/json/releases/download/v3.11.3/include.zip"
    CACHE STRING "URL or file:// path to nlohmann/json include.zip"
)

set(ADD_CUSTOM_ASCEND_SOC_SERIES ascend910 ascend310p ascend310b ascend910b ascend950)
set(LEAKY_RELU_ASCEND_SOC_SERIES ascend910b)

function(add_parallel_vendor_project target_name source_dir vendor_name soc_series)
    set(extra_cmake_args)
    if("${target_name}" STREQUAL "leaky_relu_custom")
        list(APPEND extra_cmake_args "-DNLOHMANN_JSON_URL=${NLOHMANN_JSON_URL}")
    endif()

    ExternalProject_Add(${target_name}
        SOURCE_DIR ${source_dir}
        CMAKE_CACHE_ARGS
            -DASCEND_SOC_SERIES:STRING=${soc_series}
        CMAKE_ARGS
            -Dvendor_name=${vendor_name}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/${target_name}
            ${extra_cmake_args}
        BUILD_COMMAND
            ${CMAKE_COMMAND} --build <BINARY_DIR> --target binary
        COMMAND
            ${CMAKE_COMMAND} --build <BINARY_DIR> --target ${INSTALL_TARGET}
        INSTALL_COMMAND ""
    )
endfunction()

add_parallel_vendor_project(
    add_custom
    ${CMAKE_CURRENT_SOURCE_DIR}/add_custom
    add_custom
    "${ADD_CUSTOM_ASCEND_SOC_SERIES}"
)

add_parallel_vendor_project(
    leaky_relu_custom
    ${CMAKE_CURRENT_SOURCE_DIR}/leaky_relu_custom
    leaky_relu_custom
    "${LEAKY_RELU_ASCEND_SOC_SERIES}"
)
