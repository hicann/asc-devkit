# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
get_filename_component(ASCENDC_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
find_program(ASCENDC_TEST_PYTHON NAMES python3 python)
if(NOT ASCENDC_TEST_PYTHON)
  message(FATAL_ERROR "Python interpreter not found")
endif()
string(RANDOM LENGTH 12 test_dir_suffix)
set(test_build_root "/tmp/ascendc_soc_series_${test_dir_suffix}")
set(test_source_dir "${CMAKE_CURRENT_LIST_DIR}/soc_series_validation")

execute_process(
  COMMAND "${CMAKE_COMMAND}" -S "${test_source_dir}"
          -B "${test_build_root}/supported"
          -DASCENDC_ROOT=${ASCENDC_ROOT}
          -DASCEND_PYTHON_EXECUTABLE=${ASCENDC_TEST_PYTHON}
  RESULT_VARIABLE supported_result
  OUTPUT_VARIABLE supported_output
  ERROR_VARIABLE supported_error)
if(NOT supported_result EQUAL 0)
  file(REMOVE_RECURSE "${test_build_root}")
  message(FATAL_ERROR
    "Expected supported short SoC series to pass: ${supported_output}${supported_error}")
endif()

foreach(full_soc_name Ascend910B1 Ascend950PR_9599)
  foreach(kernel_api options sources)
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -S "${test_source_dir}"
              -B "${test_build_root}/unsupported_${kernel_api}_${full_soc_name}"
              -DASCENDC_ROOT=${ASCENDC_ROOT}
              -DASCEND_PYTHON_EXECUTABLE=${ASCENDC_TEST_PYTHON}
              -DTEST_UNSUPPORTED_API=${kernel_api}
              -DTEST_UNSUPPORTED_SOC_SERIES=${full_soc_name}
      RESULT_VARIABLE unsupported_result
      OUTPUT_VARIABLE unsupported_output
      ERROR_VARIABLE unsupported_error)
    if(unsupported_result EQUAL 0)
      file(REMOVE_RECURSE "${test_build_root}")
      message(FATAL_ERROR
        "Expected npu_op_kernel_${kernel_api} to reject ${full_soc_name}")
    endif()
    if(NOT unsupported_error MATCHES "SOC_SERIES ${full_soc_name} does not support")
      file(REMOVE_RECURSE "${test_build_root}")
      message(FATAL_ERROR "Unexpected validation error: ${unsupported_error}")
    endif()
  endforeach()
endforeach()

foreach(kernel_api options sources)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${test_source_dir}"
            -B "${test_build_root}/mutually_exclusive_${kernel_api}"
            -DASCENDC_ROOT=${ASCENDC_ROOT}
            -DASCEND_PYTHON_EXECUTABLE=${ASCENDC_TEST_PYTHON}
            -DTEST_MUTUALLY_EXCLUSIVE_API=${kernel_api}
    RESULT_VARIABLE mutually_exclusive_result
    OUTPUT_VARIABLE mutually_exclusive_output
    ERROR_VARIABLE mutually_exclusive_error)
  if(mutually_exclusive_result EQUAL 0)
    file(REMOVE_RECURSE "${test_build_root}")
    message(FATAL_ERROR
      "Expected npu_op_kernel_${kernel_api} to reject SOC_SERIES with COMPUTE_UNIT")
  endif()
  if(NOT mutually_exclusive_error MATCHES "SOC_SERIES and COMPUTE_UNIT cannot be used at the same time")
    file(REMOVE_RECURSE "${test_build_root}")
    message(FATAL_ERROR "Unexpected mutual-exclusion error: ${mutually_exclusive_error}")
  endif()
endforeach()

file(REMOVE_RECURSE "${test_build_root}")
