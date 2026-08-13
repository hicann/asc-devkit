# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
set(GENERATE_CPP_COV ${CMAKE_CURRENT_LIST_DIR}/scripts/generate_cpp_cov.sh)

find_package(Python3 COMPONENTS Interpreter)
add_cann_third_party(gtest)
add_cann_third_party(boost)
add_cann_third_party(json)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/mockcpp.cmake)
