# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
cmake_minimum_required(VERSION 3.16)
project(mc2_comm_info_state_test LANGUAGES CXX)
find_package(Threads REQUIRED)
enable_testing()
add_executable(mc2_comm_info_state_test mc2_comm_info_state_test.cc)
target_compile_features(mc2_comm_info_state_test PRIVATE cxx_std_14)
target_include_directories(mc2_comm_info_state_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..)
target_link_libraries(mc2_comm_info_state_test PRIVATE Threads::Threads)
add_test(NAME mc2_comm_info_state_test COMMAND mc2_comm_info_state_test)

# Optional Host-only compatibility probe. It does not launch any NPU tasks.
set(CANN_PREFIX "" CACHE PATH "Installed CANN toolkit for the real profapi callback probe")
if(CANN_PREFIX)
    add_executable(prof_callback_probe prof_callback_probe.cc)
    target_compile_features(prof_callback_probe PRIVATE cxx_std_14)
    target_include_directories(prof_callback_probe PRIVATE ${CANN_PREFIX}/pkg_inc/profiling)
    target_link_libraries(prof_callback_probe PRIVATE ${CANN_PREFIX}/lib64/libprofapi.so Threads::Threads)
endif()
