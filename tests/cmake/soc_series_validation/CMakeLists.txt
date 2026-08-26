# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16.0)
project(ascendc_soc_series_validation NONE)

set(ASC_VALID_SOC_LIST ascend910b ascend950 bs9sx2a)
include("${ASCENDC_ROOT}/cmake/asc/fwk_modules/func.cmake")
set(ASCENDC_CMAKE_SCRIPTS_PATH "${ASCENDC_ROOT}/cmake/asc/fwk_modules")
set(CUSTOM_COMPILE_OPTIONS custom_compile_options.ini)
set_property(GLOBAL PROPERTY ASCENDC_AUTO_GEN_PATH "${CMAKE_CURRENT_BINARY_DIR}")

if(TEST_MUTUALLY_EXCLUSIVE_API STREQUAL "options")
  npu_op_kernel_options(test_target TestOp
    COMPUTE_UNIT Ascend910B1
    SOC_SERIES ascend910b
    OPTIONS -O2)
elseif(TEST_MUTUALLY_EXCLUSIVE_API STREQUAL "sources")
  npu_op_kernel_sources(test_target
    OP_TYPE TestOp
    KERNEL_FILE test_op.cpp
    COMPUTE_UNIT Ascend910B1
    SOC_SERIES ascend910b)
elseif(TEST_UNSUPPORTED_API STREQUAL "options")
  npu_op_kernel_options(test_target TestOp
    SOC_SERIES ${TEST_UNSUPPORTED_SOC_SERIES}
    OPTIONS -O2)
elseif(TEST_UNSUPPORTED_API STREQUAL "sources")
  npu_op_kernel_sources(test_target
    OP_TYPE TestOp
    KERNEL_FILE test_op.cpp
    SOC_SERIES ${TEST_UNSUPPORTED_SOC_SERIES})
else()
  _ascendc_encode_soc_series(encoded_soc_series Ascend910B ascend950)
  if(NOT "${encoded_soc_series}" STREQUAL "ascend910b;ascend950")
    message(FATAL_ERROR "Unexpected encoded SOC_SERIES: ${encoded_soc_series}")
  endif()

  npu_op_kernel_options(test_target TestOp
    SOC_SERIES Ascend910B bs9sx2a
    OPTIONS -O2)
  npu_op_kernel_sources(soc_target
    OP_TYPE TestOp
    KERNEL_FILE test_op.cpp
    SOC_SERIES Ascend910B bs9sx2a)

  npu_op_kernel_options(test_target ComputeUnitOp
    COMPUTE_UNIT Ascend910B1
    OPTIONS -O3)
  npu_op_kernel_sources(compute_unit_target
    OP_TYPE ComputeUnitOp
    KERNEL_FILE compute_unit_op.cpp
    COMPUTE_UNIT Ascend910B1)

  file(READ "${CMAKE_CURRENT_BINARY_DIR}/${CUSTOM_COMPILE_OPTIONS}" compile_options)
  if(NOT compile_options MATCHES "TestOp,ascend910b1;bs9sx2a,-O2")
    message(FATAL_ERROR "SOC_SERIES options were not converted to full SoC names: ${compile_options}")
  endif()
  if(NOT compile_options MATCHES "ComputeUnitOp,Ascend910B1,-O3")
    message(FATAL_ERROR "COMPUTE_UNIT options behavior changed: ${compile_options}")
  endif()

  file(READ "${CMAKE_CURRENT_BINARY_DIR}/soc_target_custom_source_files.ini" soc_sources)
  if(NOT soc_sources MATCHES "compute_unit=ascend910b1,bs9sx2a")
    message(FATAL_ERROR "SOC_SERIES sources were not converted to full SoC names: ${soc_sources}")
  endif()
  file(READ "${CMAKE_CURRENT_BINARY_DIR}/compute_unit_target_custom_source_files.ini" compute_unit_sources)
  if(NOT compute_unit_sources MATCHES "compute_unit=Ascend910B1")
    message(FATAL_ERROR "COMPUTE_UNIT sources behavior changed: ${compute_unit_sources}")
  endif()
endif()
