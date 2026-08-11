# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
add_library(intf_pub_base INTERFACE)

target_compile_definitions(intf_pub_base INTERFACE
    CFG_BUILD_DEBUG
)

target_compile_options(intf_pub_base INTERFACE
    -D_GLIBCXX_USE_CXX11_ABI=0
    -g
    --coverage
    -w
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
    $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address -fsanitize=leak -fsanitize-recover=address,all -fno-stack-protector -fno-omit-frame-pointer -g>
    $<$<BOOL:${ENABLE_GCOV}>:-fprofile-arcs -ftest-coverage>
    -fPIC
    -pipe
)

target_link_options(intf_pub_base INTERFACE
    -fprofile-arcs -ftest-coverage
    $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address -fsanitize=leak -fsanitize-recover=address>
    $<$<BOOL:${ENABLE_GCOV}>:-fprofile-arcs -ftest-coverage>
)

target_link_libraries(intf_pub_base INTERFACE
    $<$<BOOL:${ENABLE_GCOV}>:-lgcov>
)


add_library(intf_pub INTERFACE)

target_link_libraries(intf_pub INTERFACE
    $<BUILD_INTERFACE:intf_pub_base>
    $<$<BOOL:${ENABLE_TEST}>:gtest>
    -Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib
)
