# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.16)
project(tf_custom_ops LANGUAGES CXX)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import tensorflow as tf; print(' '.join(tf.sysconfig.get_compile_flags()))"
    OUTPUT_VARIABLE TF_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE TF_CFLAGS_STATUS
)
if(NOT TF_CFLAGS_STATUS EQUAL 0 OR TF_CFLAGS STREQUAL "")
    message(FATAL_ERROR "Failed to query TensorFlow compile flags via ${Python3_EXECUTABLE}.")
endif()

execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import tensorflow as tf; print(' '.join(tf.sysconfig.get_link_flags()))"
    OUTPUT_VARIABLE TF_LFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE TF_LFLAGS_STATUS
)
if(NOT TF_LFLAGS_STATUS EQUAL 0 OR TF_LFLAGS STREQUAL "")
    message(FATAL_ERROR "Failed to query TensorFlow link flags via ${Python3_EXECUTABLE}.")
endif()

separate_arguments(TF_CFLAGS NATIVE_COMMAND "${TF_CFLAGS}")
separate_arguments(TF_LFLAGS NATIVE_COMMAND "${TF_LFLAGS}")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "*.cc")
add_library(custom_ops SHARED ${SOURCE_FILES})

set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/outputs")
file(MAKE_DIRECTORY "${OUTPUT_DIR}")

set_target_properties(custom_ops PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    OUTPUT_NAME "custom_ops"
)

# ======================================================================================
# NPU 编译选项配置
#
# 说明：
#   - 需根据实际部署的 NPU 硬件架构选择对应的 `npu-arch` 参数。
# ======================================================================================
target_compile_options(custom_ops PRIVATE
    ${TF_CFLAGS}
    -std=c++14
)

target_link_libraries(custom_ops PRIVATE
    ${TF_LFLAGS}
)
