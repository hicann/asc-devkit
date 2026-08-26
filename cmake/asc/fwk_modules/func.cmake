# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
include(ExternalProject)

function(_ascendc_encode_soc_series output_var)
  set(encoded_soc_series)
  foreach(soc_series ${ARGN})
    string(TOLOWER "${soc_series}" soc_series_lower)
    list(FIND ASC_VALID_SOC_LIST "${soc_series_lower}" soc_series_index)
    if(soc_series_index EQUAL -1)
      message(FATAL_ERROR
        "SOC_SERIES ${soc_series} does not support, the support list is ${ASC_VALID_SOC_LIST}")
    endif()
    list(APPEND encoded_soc_series "${soc_series_lower}")
  endforeach()
  set(${output_var} "${encoded_soc_series}" PARENT_SCOPE)
endfunction()

function(get_system_info SYSTEM_INFO)
  if (UNIX)
    execute_process(COMMAND grep -i ^id= /etc/os-release OUTPUT_VARIABLE TEMP)
    string(REGEX REPLACE "\n|id=|ID=|\"" "" SYSTEM_NAME ${TEMP})
    set(${SYSTEM_INFO} ${SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR} PARENT_SCOPE)
  elseif (WIN32)
    message(STATUS "System is Windows. Only for pre-build.")
  else()
    message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not support.")
  endif()
endfunction()

function(add_ops_info_target)
  cmake_parse_arguments(OPINFO "" "TARGET;OPS_INFO;OUTPUT;INSTALL_DIR" "" ${ARGN})
  get_filename_component(opinfo_file_path "${OPINFO_OUTPUT}" DIRECTORY)
  add_custom_command(OUTPUT ${OPINFO_OUTPUT}
      COMMAND mkdir -p ${opinfo_file_path}
      COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/parse_ini_to_json.py
              ${OPINFO_OPS_INFO} ${OPINFO_OUTPUT}
  )
  add_custom_target(${OPINFO_TARGET} ALL
      DEPENDS ${OPINFO_OUTPUT}
  )
endfunction()

function(npu_op_kernel_options target_name OP_TYPE)
  cmake_parse_arguments(OP_COMPILE "" "" "COMPUTE_UNIT;SOC_SERIES;OPTIONS" ${ARGN})
  if((DEFINED OP_COMPILE_COMPUTE_UNIT OR "COMPUTE_UNIT" IN_LIST OP_COMPILE_KEYWORDS_MISSING_VALUES)
      AND (DEFINED OP_COMPILE_SOC_SERIES OR "SOC_SERIES" IN_LIST OP_COMPILE_KEYWORDS_MISSING_VALUES))
    message(FATAL_ERROR "SOC_SERIES and COMPUTE_UNIT cannot be used at the same time in npu_op_kernel_options")
  endif()
  if("SOC_SERIES" IN_LIST OP_COMPILE_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR "Must provide SOC_SERIES values for npu_op_kernel_options")
  endif()
  get_property(auto_gen_path GLOBAL PROPERTY ASCENDC_AUTO_GEN_PATH)
  if("${auto_gen_path}" STREQUAL "")
    message(FATAL_ERROR "auto_gen_path has not be set which should set in npu_op_code_gen api.")
  endif()
  if(NOT DEFINED OP_COMPILE_OPTIONS)
    message(FATAL_ERROR "Must provide options for npu_op_kernel_options")
  endif()
  set(compute_unit_args ${OP_COMPILE_COMPUTE_UNIT})
  if(DEFINED OP_COMPILE_SOC_SERIES)
    _ascendc_encode_soc_series(encoded_soc_series ${OP_COMPILE_SOC_SERIES})
    string(REPLACE ";" "," encoded_soc_series_arg "${encoded_soc_series}")
    set(compute_unit_args "--soc-series=${encoded_soc_series_arg}")
  endif()
  execute_process(COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_gen_options.py
                          ${auto_gen_path}/${CUSTOM_COMPILE_OPTIONS} ${OP_TYPE} ${compute_unit_args}
                          "${OP_COMPILE_OPTIONS}"
                  RESULT_VARIABLE EXEC_RESULT
                  OUTPUT_VARIABLE EXEC_INFO
                  ERROR_VARIABLE  EXEC_ERROR)
  if (${EXEC_RESULT})
      message("add ops compile options info: ${EXEC_INFO}")
      message("add ops compile options error: ${EXEC_ERROR}")
      message(FATAL_ERROR "add ops compile options failed!")
  endif()
endfunction()

function(adapt_install_path)
  cmake_parse_arguments(ADAPT "" "INPUT_PATH;INPUT_TARGET;OUTPUT_PATH" "" ${ARGN})
  get_property(enable_cpack GLOBAL PROPERTY _ASC_PKG_${ADAPT_INPUT_TARGET}_ENABLE_CPACK)
  if(enable_cpack)
    set(${ADAPT_OUTPUT_PATH} "${ADAPT_INPUT_PATH}" PARENT_SCOPE)
  else()
    get_property(pkg_install_path GLOBAL PROPERTY _ASC_PKG_${ADAPT_INPUT_TARGET}_INSTALL_PATH)
    set(${ADAPT_OUTPUT_PATH} "${pkg_install_path}/${ADAPT_INPUT_PATH}" PARENT_SCOPE)
  endif()
endfunction()

function(add_npu_support_target)
  cmake_parse_arguments(NPUSUP "" "TARGET;OPS_INFO_DIR;OUT_DIR;INSTALL_DIR;PACKAGE_NAME" "" ${ARGN})
  get_filename_component(npu_sup_file_path "${NPUSUP_OUT_DIR}" DIRECTORY)
  add_custom_command(OUTPUT ${NPUSUP_OUT_DIR}/npu_supported_ops.json
    COMMAND mkdir -p ${NPUSUP_OUT_DIR}
    COMMAND ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/gen_ops_filter.sh
            ${NPUSUP_OPS_INFO_DIR}
            ${NPUSUP_OUT_DIR}
  )
  add_custom_target(${NPUSUP_TARGET} ALL
    DEPENDS ${NPUSUP_OUT_DIR}/npu_supported_ops.json
  )

  get_property(output_pkg_type GLOBAL PROPERTY _ASC_PKG_${NPUSUP_PACKAGE_NAME}_TYPE)
  if("${output_pkg_type}" STREQUAL "RUN")
    adapt_install_path(
      INPUT_PATH ${NPUSUP_INSTALL_DIR}
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(FILES ${NPUSUP_OUT_DIR}/npu_supported_ops.json
      DESTINATION ${install_path}
    )
  endif()
endfunction()

function(add_static_library STATIC_LIB_NAME)
  cmake_parse_arguments(STATIC_LIB "" "TYPE;PACKAGE_NAME" "TARGETS" ${ARGN})
  set(WORKING_DIR ${CMAKE_BINARY_DIR})
  set(OUTPUT_LIB lib${STATIC_LIB_NAME}.a)
  set(FILE_LIST_TMP_DIR ${WORKING_DIR}/.static_lib_file_list)

  foreach(TARGET IN LISTS STATIC_LIB_TARGETS)
    list(APPEND TARGET_LIST ${TARGET})
    if("${STATIC_LIB_TYPE}" STREQUAL "HOST")
      set(CUR_FILE ${FILE_LIST_TMP_DIR}/${TARGET}.txt)
      file(GENERATE OUTPUT ${CUR_FILE}
        CONTENT "$<JOIN:$<TARGET_OBJECTS:${TARGET}>,\n>"
      )
    endif()
  endforeach()

  set(COLLECT_TMP_DIR ${WORKING_DIR}/.static_lib_tmp)
  # set(KERNEL_LIBS "${CMAKE_BINARY_DIR}/library/libopregistry.a ${CMAKE_BINARY_DIR}/op_kernel/binary/library/libkernels.a")
  if (NOT TARGET ${STATIC_LIB_NAME})
    add_custom_target(${STATIC_LIB_PACKAGE_NAME}_asc_gen_static_obj_file
      COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_gen_static_library.py
      --object-files-dir=${FILE_LIST_TMP_DIR}
      --kernel-libs-dir=${CMAKE_BINARY_DIR}
      --tmp-obj-dir=${COLLECT_TMP_DIR}
      --output-file=${CMAKE_BINARY_DIR}/${OUTPUT_LIB}
      --remove-tmp-files=1
      --package-name=${STATIC_LIB_PACKAGE_NAME}
    )
    add_dependencies(${STATIC_LIB_PACKAGE_NAME}_asc_gen_static_obj_file ${TARGET_LIST})
    add_custom_target(${STATIC_LIB_NAME} ALL DEPENDS ${STATIC_LIB_PACKAGE_NAME}_asc_gen_static_obj_file)
  else()
    add_dependencies(${STATIC_LIB_PACKAGE_NAME}_asc_gen_static_obj_file ${TARGET_LIST})
  endif()
endfunction()

function(npu_op_device_tiling_library target_name target_type)
  message(STATUS "Ascendc device library generating")
  string(TOUPPER ${target_type} _upper_target_type)
  set(support_types SHARED STATIC)
  if(NOT _upper_target_type IN_LIST support_types)
      message(FATAL_ERROR "target_type ${_upper_target_type} does not support, the support list is ${support_types}")
  endif()
  set(SOURCES)
  foreach(_source ${ARGN})
    get_filename_component(absolute_source "${_source}" ABSOLUTE)
    list(APPEND SOURCES ${absolute_source})
  endforeach()
  string(REPLACE ";" " " EP_SOURCES "${SOURCES}")

  execute_process(
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tiling_sink
      COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/tiling_sink/CMakeLists.txt
  )
  execute_process(
      COMMAND ${CMAKE_COMMAND} -E echo "cmake_minimum_required(VERSION 3.16.0)\nproject(cust_tiling_sink)\ninclude(${ASCENDC_CMAKE_SCRIPTS_PATH}/device_task.cmake)\n"
      OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/tiling_sink/CMakeLists.txt
      RESULT_VARIABLE result
  )

  ExternalProject_Add(tiling_sink_task
      SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/tiling_sink
      CONFIGURE_COMMAND ${CMAKE_COMMAND}
      -DASCEND_CANN_PACKAGE_PATH=${ASCEND_CANN_PACKAGE_PATH}
      -DTARGET=${target_name}
      -DOPTION=${_upper_target_type}
      -DSRC=${EP_SOURCES}
      <SOURCE_DIR>
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${ASCENDC_INSTALL_PREFIX}
      INSTALL_COMMAND ""
      BUILD_ALWAYS TRUE
  )
  ExternalProject_Get_Property(tiling_sink_task BINARY_DIR)
  set(TILINGSINK_LIB "")
  if ("${target_type}" STREQUAL "SHARED")
      set(TILINGSINK_LIB "${BINARY_DIR}/libcust_opmaster.so")
  else()
      set(TILINGSINK_LIB "${BINARY_DIR}/libcust_opmaster.a")
  endif()
  set_property(GLOBAL PROPERTY ASCENDC_DEVICE_SINK_TARGET_OUTPUT ${TILINGSINK_LIB})
  get_property(tmp_device_sink_target GLOBAL PROPERTY ASCENDC_DEVICE_SINK_TARGET)
  list(APPEND tmp_device_sink_target ${target_name})
  set_property(GLOBAL PROPERTY ASCENDC_DEVICE_SINK_TARGET ${tmp_device_sink_target})
  set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_TYPE "DEVICE_SINK")
endfunction()

function(simple_kernel_compile)
  cmake_parse_arguments(BINCMP "" "OPS_INFO;OUT_DIR;TILING_LIB;OP_TYPE;SRC;COMPUTE_UNIT;JSON_FILE;DYNAMIC_PATH;TARGET_NAME;PACKAGE_NAME" "OPTIONS;CONFIGS" ${ARGN})
  if (NOT DEFINED BINCMP_OUT_DIR)
    set(BINCMP_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/binary)
  endif()
  set(BINCMP_TILING_LIB_TMP ${BINCMP_TILING_LIB})
  set(BINCMP_TILING_LIB $<TARGET_FILE:${BINCMP_PACKAGE_NAME}_ascendc_${BINCMP_TILING_LIB}>)
  # add Environment Variable Configurations of ccache
  set(_ASCENDC_ENV_VAR)
  if(${CMAKE_CXX_COMPILER_LAUNCHER} MATCHES "ccache$")
    list(APPEND _ASCENDC_ENV_VAR export ASCENDC_CCACHE_EXECUTABLE=${CMAKE_CXX_COMPILER_LAUNCHER} &&)
  endif()

  get_property(auto_gen_path GLOBAL PROPERTY _ASC_PKG_${BINCMP_PACKAGE_NAME}_CODE_GEN_DIRS)
  if (NOT DEFINED BINCMP_OPS_INFO)
    set(BINCMP_OPS_INFO ${auto_gen_path}/aic-${BINCMP_COMPUTE_UNIT}-ops-info.ini)
  endif()
  get_property(bincmp_enable_binary_package GLOBAL PROPERTY _ASC_PKG_${BINCMP_PACKAGE_NAME}_ENABLE_BINARY_PACKAGE)

  get_property(error_msg_file GLOBAL PROPERTY _ASC_PKG_ERROR_FILE)
  set(BINCMP_ERROR_LOG ${error_msg_file})
  set(BINCMP_COMPILE_LOG "${CMAKE_CURRENT_BINARY_DIR}/${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT}_compile.log")
  set(BINCMP_COMPILE_FAIL_FLAG "${CMAKE_CURRENT_BINARY_DIR}/${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT}_fail.log")
  set(COMPILE_LOG "${CMAKE_CURRENT_BINARY_DIR}/compile.log")

  if (NOT ${ENABLE_CROSS_COMPILE})
    if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
      set(COMPILE_CMD
        "${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_compile_kernel.py \
            --op-type=${BINCMP_OP_TYPE} \
            --src-file=${BINCMP_SRC} \
            --compute-unit=${BINCMP_COMPUTE_UNIT} \
            --compile-options='${BINCMP_OPTIONS}' \
            --debug-config='${BINCMP_CONFIGS}' \
            --config-ini=${BINCMP_OPS_INFO} \
            --tiling-lib=${BINCMP_TILING_LIB} \
            --output-path=${BINCMP_OUT_DIR} \
            --dynamic-dir=${BINCMP_DYNAMIC_PATH} \
            --enable-binary='${bincmp_enable_binary_package}' \
            --json-file=${BINCMP_JSON_FILE} \
            --target-name=${BINCMP_TARGET_NAME} \
            --auto-gen-path=${auto_gen_path} \
            --build-tool=$(MAKE) > ${BINCMP_COMPILE_LOG} 2>&1; echo $? > ${BINCMP_COMPILE_FAIL_FLAG}"
      )

      # Collect failure information and append it to BINCMP_ERROR_LOG
      set(CHECK_ERROR_CMD
          "if ! grep -q -x -i '0' ${BINCMP_COMPILE_FAIL_FLAG}; then \
              ERROR_MSG='fail op-type: ${BINCMP_OP_TYPE}, fail compute-unit: ${BINCMP_COMPUTE_UNIT}.'; \
              if [ ! -f ${BINCMP_ERROR_LOG} ] || ! grep -qF \"\${ERROR_MSG}\" ${BINCMP_ERROR_LOG}; then \
                  echo '' >> ${BINCMP_ERROR_LOG}; \
                  echo \"\${ERROR_MSG}\" >> ${BINCMP_ERROR_LOG}; \
              fi; \
              cat ${BINCMP_COMPILE_LOG} >> ${COMPILE_LOG}; \
          fi; \
          rm -f ${BINCMP_COMPILE_LOG}; \
          rm -f ${BINCMP_COMPILE_FAIL_FLAG}"
      )

      add_custom_target(${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT}
          COMMAND ${CMAKE_COMMAND} -E env ${_ASCENDC_ENV_VAR} bash -c "${COMPILE_CMD}"
          COMMAND ${CMAKE_COMMAND} -E env bash -c "${CHECK_ERROR_CMD}"
          VERBATIM
      )
    else()
      add_custom_target(${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT}
                      COMMAND ${_ASCENDC_ENV_VAR} ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_compile_kernel.py
                      --op-type=${BINCMP_OP_TYPE}
                      --src-file=${BINCMP_SRC}
                      --compute-unit=${BINCMP_COMPUTE_UNIT}
                      --compile-options=\"${BINCMP_OPTIONS}\"
                      --debug-config=\"${BINCMP_CONFIGS}\"
                      --config-ini=${BINCMP_OPS_INFO}
                      --tiling-lib=${BINCMP_TILING_LIB}
                      --output-path=${BINCMP_OUT_DIR}
                      --dynamic-dir=${BINCMP_DYNAMIC_PATH}
                      --enable-binary=\"${bincmp_enable_binary_package}\"
                      --json-file=${BINCMP_JSON_FILE}
                      --target-name=${BINCMP_TARGET_NAME}
                      --auto-gen-path=${auto_gen_path}
                      --build-tool=$(MAKE))
    endif()
    add_dependencies(${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT} ${BINCMP_PACKAGE_NAME}_ascendc_${BINCMP_TILING_LIB_TMP})
  else()
    if (${bincmp_enable_binary_package} AND NOT DEFINED HOST_NATIVE_TILING_LIB)
      message(FATAL_ERROR "Native host libs was not set for cross compile!")
    endif()
    if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
      set(COMPILE_CMD
        "${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_compile_kernel.py \
            --op-type=${BINCMP_OP_TYPE} \
            --src-file=${BINCMP_SRC} \
            --compute-unit=${BINCMP_COMPUTE_UNIT} \
            --compile-options='${BINCMP_OPTIONS}' \
            --debug-config='${BINCMP_CONFIGS}' \
            --config-ini=${BINCMP_OPS_INFO} \
            --tiling-lib=${HOST_NATIVE_TILING_LIB} \
            --output-path=${BINCMP_OUT_DIR} \
            --dynamic-dir=${BINCMP_DYNAMIC_PATH} \
            --enable-binary='${bincmp_enable_binary_package}' \
            --json-file=${BINCMP_JSON_FILE} \
            --target-name=${BINCMP_TARGET_NAME} \
            --auto-gen-path=${auto_gen_path} \
            --build-tool=$(MAKE) > ${BINCMP_COMPILE_LOG} 2>&1; echo $? > ${BINCMP_COMPILE_FAIL_FLAG}"
      )

      # Collect failure information and append it to BINCMP_ERROR_LOG
      set(CHECK_ERROR_CMD
          "if ! grep -q -x -i '0' ${BINCMP_COMPILE_FAIL_FLAG}; then \
              ERROR_MSG='fail op-type: ${BINCMP_OP_TYPE}, fail compute-unit: ${BINCMP_COMPUTE_UNIT}.'; \
              if [ ! -f ${BINCMP_ERROR_LOG} ] || ! grep -qF \"\${ERROR_MSG}\" ${BINCMP_ERROR_LOG}; then \
                  echo '' >> ${BINCMP_ERROR_LOG}; \
                  echo \"\${ERROR_MSG}\" >> ${BINCMP_ERROR_LOG}; \
              fi; \
              cat ${BINCMP_COMPILE_LOG} >> ${COMPILE_LOG}; \
          fi; \
          rm -f ${BINCMP_COMPILE_LOG}; \
          rm -f ${BINCMP_COMPILE_FAIL_FLAG}"
      )

      add_custom_target(${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT}
          COMMAND ${CMAKE_COMMAND} -E env ${_ASCENDC_ENV_VAR} bash -c "${COMPILE_CMD}"
          COMMAND ${CMAKE_COMMAND} -E env bash -c "${CHECK_ERROR_CMD}"
          VERBATIM
      )
    else()
      add_custom_target(${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT}
                      COMMAND ${_ASCENDC_ENV_VAR} ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_compile_kernel.py
                      --op-type=${BINCMP_OP_TYPE}
                      --src-file=${BINCMP_SRC}
                      --compute-unit=${BINCMP_COMPUTE_UNIT}
                      --compile-options=\"${BINCMP_OPTIONS}\"
                      --debug-config=\"${BINCMP_CONFIGS}\"
                      --config-ini=${BINCMP_OPS_INFO}
                      --tiling-lib=${HOST_NATIVE_TILING_LIB}
                      --output-path=${BINCMP_OUT_DIR}
                      --dynamic-dir=${BINCMP_DYNAMIC_PATH}
                      --enable-binary=\"${bincmp_enable_binary_package}\"
                      --json-file=${BINCMP_JSON_FILE}
                      --target-name=${BINCMP_TARGET_NAME}
                      --auto-gen-path=${auto_gen_path}
                      --build-tool=$(MAKE))
    endif()
  endif()
  add_dependencies(${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT} ${BINCMP_TARGET_NAME}_ops_info_gen_${BINCMP_COMPUTE_UNIT})

  get_property(kernel_targets GLOBAL PROPERTY ${BINCMP_TARGET_NAME}_ASCENDC_KERNEL_TARGETS_${BINCMP_COMPUTE_UNIT})
  if(kernel_targets)
      list(APPEND kernel_targets ${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT})
  else()
      set(kernel_targets ${BINCMP_OP_TYPE}_${BINCMP_COMPUTE_UNIT})
  endif()
  set_property(GLOBAL PROPERTY ${BINCMP_TARGET_NAME}_ASCENDC_KERNEL_TARGETS_${BINCMP_COMPUTE_UNIT} "${kernel_targets}")
endfunction()

function(npu_op_kernel_sources target_name)
  cmake_parse_arguments(KERNEL_SRC "" "KERNEL_FILE;KERNEL_DIR;OP_TYPE" "COMPUTE_UNIT;SOC_SERIES" ${ARGN})
  if((DEFINED KERNEL_SRC_COMPUTE_UNIT OR "COMPUTE_UNIT" IN_LIST KERNEL_SRC_KEYWORDS_MISSING_VALUES)
      AND (DEFINED KERNEL_SRC_SOC_SERIES OR "SOC_SERIES" IN_LIST KERNEL_SRC_KEYWORDS_MISSING_VALUES))
    message(FATAL_ERROR "SOC_SERIES and COMPUTE_UNIT cannot be used at the same time in npu_op_kernel_sources")
  endif()
  if("SOC_SERIES" IN_LIST KERNEL_SRC_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR "Must provide SOC_SERIES values for npu_op_kernel_sources")
  endif()
  if(((DEFINED KERNEL_SRC_KERNEL_FILE) AND (NOT DEFINED KERNEL_SRC_OP_TYPE)) OR ((NOT DEFINED KERNEL_SRC_KERNEL_FILE) AND (DEFINED KERNEL_SRC_OP_TYPE)))
    message(FATAL_ERROR "op_type and kernel_file must exist at the same time")
  endif()
  get_property(auto_gen_path GLOBAL PROPERTY ASCENDC_AUTO_GEN_PATH)
  if("${auto_gen_path}" STREQUAL "")
    message(FATAL_ERROR "auto_gen_path has not be set which should set in npu_op_code_gen api.")
  endif()
  set(process_source_files_command "${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_source_files_ini.py" "--auto-gen-path=${auto_gen_path}" "--target-name=${target_name}")
  if(DEFINED KERNEL_SRC_OP_TYPE)
    list(APPEND process_source_files_command "--op-type=${KERNEL_SRC_OP_TYPE}")
  else()
    list(APPEND process_source_files_command "--op-type=ALL")
  endif()
  if(DEFINED KERNEL_SRC_KERNEL_FILE)
    list(APPEND process_source_files_command "--op-kernel-file=${KERNEL_SRC_KERNEL_FILE}")
  endif()
  if(DEFINED KERNEL_SRC_KERNEL_DIR)
    list(APPEND process_source_files_command "--op-kernel-dir=${KERNEL_SRC_KERNEL_DIR}")
  endif()
  if(DEFINED KERNEL_SRC_COMPUTE_UNIT)
    string(REPLACE ";" " " COMPUTE_UNITS "${KERNEL_SRC_COMPUTE_UNIT}")
    list(APPEND process_source_files_command "--compute-unit=${COMPUTE_UNITS}")
  elseif(DEFINED KERNEL_SRC_SOC_SERIES)
    _ascendc_encode_soc_series(encoded_soc_series ${KERNEL_SRC_SOC_SERIES})
    string(REPLACE ";" " " COMPUTE_UNITS "${encoded_soc_series}")
    list(APPEND process_source_files_command "--soc-series=${COMPUTE_UNITS}")
  endif()
  execute_process(COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${process_source_files_command}
    RESULT_VARIABLE EXEC_RESULT
    OUTPUT_VARIABLE EXEC_INFO
    ERROR_VARIABLE  EXEC_ERROR)
  if(${EXEC_RESULT})
    message("process kernel source files info: ${EXEC_INFO}")
    message("process kernel source files error: ${EXEC_ERROR}")
    message(FATAL_ERROR "process kernel source files failed!")
  endif()
endfunction()

function(ascend_copy_kernel_source_file)
  cmake_parse_arguments(COPY_KERNEL "" "TARGET;PACKAGE_TARGET;KERNEL_BASE_DIR;DST_DIR" "" ${ARGN})
  get_property(auto_gen_path GLOBAL PROPERTY ASCENDC_AUTO_GEN_PATH)
  if("${auto_gen_path}" STREQUAL "")
    message(FATAL_ERROR "auto_gen_path has not be set which should set in npu_op_code_gen api.")
  endif()
  add_custom_command(OUTPUT ${ASCEND_AUTOGEN_PATH}/copy_kernel_src.timestamp
    COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_copy_kernel_src.py --auto-gen-path=${auto_gen_path} --target-name=${COPY_KERNEL_PACKAGE_TARGET}
    --kernel-base-dir=${COPY_KERNEL_KERNEL_BASE_DIR} --copy-dst-dir=${COPY_KERNEL_DST_DIR}
    COMMAND echo "copy kernel src" > ${ASCEND_AUTOGEN_PATH}/copy_kernel_src.timestamp
  )
  add_custom_target(${COPY_KERNEL_TARGET}
    DEPENDS ${ASCEND_AUTOGEN_PATH}/copy_kernel_src.timestamp
  )
endfunction()

function(npu_op_kernel_library target_name)
  cmake_parse_arguments(KERNEL "" "SRC_BASE;TILING_LIBRARY" "" ${ARGN})
  set(DYNAMIC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${target_name}/binary/dynamic/)
  if(NOT DEFINED KERNEL_SRC_BASE)
    message(FATAL_ERROR "must provide SRC_BASE for npu_op_kernel_library")
  endif()
  if(NOT DEFINED KERNEL_TILING_LIBRARY)
    message(FATAL_ERROR "must provide TILING_LIBRARY for npu_op_kernel_library")
  endif()
  ascend_copy_kernel_source_file(TARGET ${target_name}_copy_kernel_srcs
    PACKAGE_TARGET ${target_name}
    KERNEL_BASE_DIR ${KERNEL_SRC_BASE}/
    DST_DIR ${DYNAMIC_PATH}
  )

  get_property(tmp_kernel_target GLOBAL PROPERTY ASCENDC_KERNEL_TARGET)
  list(APPEND tmp_kernel_target ${target_name})
  set_property(GLOBAL PROPERTY ASCENDC_KERNEL_TARGET ${tmp_kernel_target})
  set_property(GLOBAL PROPERTY _ASC_TGT_KERNEL_${target_name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${target_name})
  set_property(GLOBAL PROPERTY _ASC_TGT_KERNEL_${target_name}_TYPE "KERNEL")
  set_property(GLOBAL PROPERTY _ASC_TGT_KERNEL_${target_name}_SRC_BASE ${KERNEL_SRC_BASE})
  set_property(GLOBAL PROPERTY _ASC_TGT_KERNEL_${target_name}_TILING_LIBRARY ${KERNEL_TILING_LIBRARY})
endfunction()

function(npu_op_package target_package_name)
  cmake_parse_arguments(PACKAGE "" "TYPE" "CONFIG" ${ARGN})
  if(NOT DEFINED PACKAGE_TYPE)
    message(FATAL_ERROR "must provide package type for npu_op_package")
  endif()
  string(TOUPPER ${PACKAGE_TYPE} _upper_target_type)
  set(support_types SHARED STATIC RUN)
  if(NOT _upper_target_type IN_LIST support_types)
      message(FATAL_ERROR "target_type ${target_type} does not support, the support list is ${support_types}")
  endif()

  if(DEFINED PACKAGE_CONFIG)
    message(STATUS "set package config: ${PACKAGE_CONFIG}")
  else()
    message(STATUS "package config is empty")
  endif()

  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_SOURCE_PACKAGE ${ENABLE_SOURCE_PACKAGE})
  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_BINARY_PACKAGE ${ENABLE_BINARY_PACKAGE})
  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_INSTALL_PATH ${ASCENDC_INSTALL_PREFIX})
  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_DEFAULT_PACKAGE_NAME_RULE True)
  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_VENDOR_NAME ${target_package_name})

  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_CPACK True)
  if(DEFINED PACKAGE_CONFIG)
    set(set_install_path False)
    set(enable_pkg True)
    list(LENGTH PACKAGE_CONFIG kv_count)
    math(EXPR max_index "${kv_count} - 1")
    foreach(i RANGE 0 ${max_index} 2)
      list(GET PACKAGE_CONFIG ${i} key)
      math(EXPR j "${i} + 1")
      list(GET PACKAGE_CONFIG ${j} value)
      if("${key}" STREQUAL "ENABLE_SOURCE_PACKAGE")
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_SOURCE_PACKAGE ${value})
      elseif("${key}" STREQUAL "ENABLE_BINARY_PACKAGE")
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_BINARY_PACKAGE ${value})
      elseif("${key}" STREQUAL "INSTALL_PATH")
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_INSTALL_PATH ${value})
        set(set_install_path True)
      elseif("${key}" STREQUAL "ENABLE_DEFAULT_PACKAGE_NAME_RULE")
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_DEFAULT_PACKAGE_NAME_RULE ${value})
      elseif("${key}" STREQUAL "VENDOR_NAME")
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_VENDOR_NAME ${value})
      elseif("${key}" STREQUAL "ENABLE_CPACK")
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_CPACK ${value})
        set(enable_pkg ${value})
      else()
        message(WARNING "${key} is not supported now")
      endif()
    endforeach()
    if(${set_install_path} AND ${enable_pkg})
      get_property(pkg_install_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_INSTALL_PATH)
      set(CMAKE_INSTALL_PREFIX ${pkg_install_path} PARENT_SCOPE)
      set(CMAKE_INSTALL_PREFIX ${pkg_install_path})
    endif()
  endif()

  get_property(pkg_install_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_INSTALL_PATH)
  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_PACKAGE_PATH ${pkg_install_path})

  set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TYPE "${_upper_target_type}")
  get_property(target_vendor_name GLOBAL PROPERTY _ASC_PKG_${target_package_name}_VENDOR_NAME)

  set(POST_PACK_ERROR_LOG "${CMAKE_CURRENT_BINARY_DIR}/compile_error.log")
  set_property(GLOBAL PROPERTY _ASC_PKG_ERROR_FILE ${POST_PACK_ERROR_LOG})

  if ("${_upper_target_type}" STREQUAL "RUN")
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/scripts/install.sh ${CMAKE_BINARY_DIR}/scripts/upgrade.sh ${CMAKE_BINARY_DIR}/scripts/uninstall.sh
      COMMAND mkdir -p ${CMAKE_BINARY_DIR}/scripts
      COMMAND cp -r ${ASCENDC_CMAKE_SCRIPTS_PATH}/scripts/* ${CMAKE_BINARY_DIR}/scripts/
      COMMAND sed -i "s/vendor_name=customize/vendor_name=${target_vendor_name}/g" ${CMAKE_BINARY_DIR}/scripts/*
    )
    add_custom_target(modify_vendor ALL DEPENDS ${CMAKE_BINARY_DIR}/scripts/install.sh ${CMAKE_BINARY_DIR}/scripts/upgrade.sh ${CMAKE_BINARY_DIR}/scripts/uninstall.sh)

    get_system_info(SYSTEM_INFO)

    # gen version.info
    add_custom_target(gen_version_info ALL
          COMMAND bash ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/gen_version_info.sh ${ASCEND_CANN_PACKAGE_PATH} ${CMAKE_CURRENT_BINARY_DIR}
    )
    adapt_install_path(
      INPUT_PATH .
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(DIRECTORY ${CMAKE_BINARY_DIR}/scripts/ DESTINATION ${install_path} FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ GROUP_READ)
    adapt_install_path(
      INPUT_PATH packages
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(FILES ${CMAKE_SOURCE_DIR}/custom.proto DESTINATION ${install_path} OPTIONAL)
    adapt_install_path(
      INPUT_PATH packages/vendors/${target_vendor_name}/
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.info
            DESTINATION ${install_path})

    get_property(enable_cpack GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_CPACK)
    if (${enable_cpack})
      # CPack config
      set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
      set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
      set(CPACK_PACKAGE_DESCRIPTION "CPack opp project")
      set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CPack opp project")
      set(CPACK_PACKAGE_DIRECTORY ${pkg_install_path})
      get_property(enable_default_package_name GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_DEFAULT_PACKAGE_NAME_RULE)
      if(NOT enable_default_package_name)
        set(CPACK_PACKAGE_FILE_NAME "${target_package_name}.run")
      else()
        set(CPACK_PACKAGE_FILE_NAME "custom_opp_${SYSTEM_INFO}.run")
      endif()
      set(CPACK_GENERATOR External)
      set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
      set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)
      set(CPACK_EXTERNAL_PACKAGE_SCRIPT ${ASCENDC_CMAKE_SCRIPTS_PATH}/makeself.cmake)
      set(CPACK_EXTERNAL_BUILT_PACKAGES ${CPACK_PACKAGE_DIRECTORY}/_CPack_Packages/Linux/External/${CPACK_PACKAGE_FILE_NAME}/${CPACK_PACKAGE_FILE_NAME})
      if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
        set(CPACK_POST_BUILD_SCRIPTS "${CMAKE_CURRENT_BINARY_DIR}/post_pack_script.cmake")
        file(WRITE "${CPACK_POST_BUILD_SCRIPTS}" "
          set(ERROR_LOG_FILE \"${POST_PACK_ERROR_LOG}\")
          if(EXISTS \"\${ERROR_LOG_FILE}\")
            file(READ \"\${ERROR_LOG_FILE}\" ERROR_CONTENT)
            message(STATUS \"=========== Failed operators ===========\")
            message(\"\${ERROR_CONTENT}\")
            message(STATUS \"========================================\")
          endif()
        ")
      endif()
      include(CPack)
    endif()
  elseif("${_upper_target_type}" STREQUAL "STATIC")
    add_library(${target_package_name} STATIC)
    set_target_properties(
      ${target_package_name}
      PROPERTIES LINKER_LANGUAGE CXX
    )
    set(ascendc_static_file ${CMAKE_BINARY_DIR}/${target_package_name}_ascendc_final.o)
    set_source_files_properties(${ascendc_static_file} PROPERTIES GENERATED TRUE)
    target_sources(${target_package_name} PRIVATE ${ascendc_static_file})
    add_dependencies(${target_package_name} ${target_package_name}_ascendc_static_file_target)
    target_link_libraries(${target_package_name} PUBLIC
      nnopbase
      $<$<BOOL:${BUILD_WITH_INSTALLED_DEPENDENCY_CANN_PKG}>:acl_rt>
      $<$<NOT:$<BOOL:${BUILD_WITH_INSTALLED_DEPENDENCY_CANN_PKG}>>:ascendcl>
      exe_graph
      opp_registry
      register
      graph
      graph_base
      tiling_api
      -Wl,--no-as-needed
      metadef
      unified_dlog
      mmpa
      c_sec
      -Wl,--as-needed
      -Wl,--whole-archive
      rt2_registry
      -Wl,--no-whole-archive
      dl
    )
    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_package_name} PRIVATE
        ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
        ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    adapt_install_path(
      INPUT_PATH include
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(TARGETS ${target_package_name} EXPORT ${target_package_name}-targets INCLUDES DESTINATION ${install_path})
    if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
      install(CODE "
        set(ERROR_LOG_FILE \"${POST_PACK_ERROR_LOG}\")
        if(EXISTS \"\${ERROR_LOG_FILE}\")
          file(READ \"\${ERROR_LOG_FILE}\" ERROR_CONTENT)
          message(STATUS \"\")
          message(STATUS \"=========== Failed operators ===========\")
          message(\"\${ERROR_CONTENT}\")
          message(STATUS \"========================================\")
        endif()
      ")
    endif()
    adapt_install_path(
      INPUT_PATH lib/cmake/${target_package_name}
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(EXPORT ${target_package_name}-targets FILE ${target_package_name}-targets.cmake NAMESPACE ${target_package_name}:: DESTINATION ${install_path})

    set(STATIC_TARGET_NAME ${target_package_name})
    configure_file(
      ${ASCENDC_CMAKE_SCRIPTS_PATH}/ascendc_config.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/${target_package_name}-config.cmake
      @ONLY
    )
    adapt_install_path(
      INPUT_PATH lib/cmake/${target_package_name}/
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${target_package_name}-config.cmake
      DESTINATION ${install_path})

  elseif("${_upper_target_type}" STREQUAL "SHARED")
    add_library(${target_package_name} SHARED)
    set_target_properties(
      ${target_package_name}
      PROPERTIES LINKER_LANGUAGE CXX
    )
    target_link_libraries(${target_package_name} PRIVATE
      intf_pub
      $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
      nnopbase
      $<$<BOOL:${BUILD_WITH_INSTALLED_DEPENDENCY_CANN_PKG}>:acl_rt>
      $<$<NOT:$<BOOL:${BUILD_WITH_INSTALLED_DEPENDENCY_CANN_PKG}>>:ascendcl>
      exe_graph
      register
      tiling_api
    )
    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_package_name} PRIVATE
        ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
        ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    set_target_properties(${target_package_name} PROPERTIES OUTPUT_NAME cust_opapi)
    adapt_install_path(
      INPUT_PATH include
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(TARGETS ${target_package_name} EXPORT ${target_package_name}-targets INCLUDES DESTINATION ${install_path})
    if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
      install(CODE "
        set(ERROR_LOG_FILE \"${POST_PACK_ERROR_LOG}\")
        if(EXISTS \"\${ERROR_LOG_FILE}\")
          file(READ \"\${ERROR_LOG_FILE}\" ERROR_CONTENT)
          message(STATUS \"\")
          message(STATUS \"=========== Failed operators ===========\")
          message(\"\${ERROR_CONTENT}\")
          message(STATUS \"========================================\")
        endif()
      ")
    endif()
    adapt_install_path(
      INPUT_PATH lib/cmake/${target_package_name}
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(EXPORT ${target_package_name}-targets FILE ${target_package_name}-targets.cmake NAMESPACE ${target_package_name}:: DESTINATION ${install_path})

    set(STATIC_TARGET_NAME ${target_package_name})
    configure_file(
      ${ASCENDC_CMAKE_SCRIPTS_PATH}/ascendc_config.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/${target_package_name}-config.cmake
      @ONLY
    )
    adapt_install_path(
      INPUT_PATH lib/cmake/${target_package_name}/
      INPUT_TARGET ${target_package_name}
      OUTPUT_PATH install_path
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${target_package_name}-config.cmake
      DESTINATION ${install_path})
  endif()
endfunction()

function(npu_op_code_gen)
  message(STATUS "Opbuild generating sources")
  cmake_parse_arguments(OPBUILD "" "OUT_DIR;PROJECT_NAME;ACCESS_PREFIX;ENABLE_SOURCE;JOBS;PACKAGE;JOIN_OP_DEF" "SRC;COMPILE_OPTIONS;LINK_OPTIONS;OPTIONS" ${ARGN})
  if(NOT DEFINED OPBUILD_SRC)
    message(FATAL_ERROR "must provide src file for npu_op_code_gen")
  endif()
  if(NOT DEFINED OPBUILD_OUT_DIR)
    message(FATAL_ERROR "must provide output dir for npu_op_code_gen")
  endif()
  if(NOT DEFINED OPBUILD_PACKAGE)
    message(FATAL_ERROR "must provide package name for npu_op_code_gen")
  endif()
  if(NOT DEFINED OPBUILD_JOIN_OP_DEF)
    set(OPBUILD_JOIN_OP_DEF False)
  elseif(NOT "${OPBUILD_JOIN_OP_DEF}" MATCHES "^(True|False)$")
    message(WARNING "OPBUILD_JOIN_OP_DEF value '${OPBUILD_JOIN_OP_DEF}' is invalid, must be 'True' or 'False'. Setting to False.")
    set(OPBUILD_JOIN_OP_DEF False)
  endif()
  set_property(GLOBAL PROPERTY _ASC_PKG_${OPBUILD_PACKAGE}_CODE_GEN_DIRS ${OPBUILD_OUT_DIR})

  set(compile_ops_command "${ASCENDC_CMAKE_SCRIPTS_PATH}/util/compile_ascendc_all_ops_so.py" "--output-dir=${OPBUILD_OUT_DIR}" "--output=ascend_all_ops" "--cann-path=${ASCEND_CANN_PACKAGE_PATH}" "--cxx-compiler=${ASCENDC_CMAKE_COMPILER}")
  string(REPLACE ";" " " EP_SRCS "${OPBUILD_SRC}")
  list(APPEND compile_ops_command "--src-file=${EP_SRCS}")
  if(NOT "${OPBUILD_COMPILE_OPTIONS}x" STREQUAL "x")
    string(REPLACE ";" " " EP_COMPILE_OPTIONS "${OPBUILD_COMPILE_OPTIONS}")
    list(APPEND compile_ops_command "--compile-options=${EP_COMPILE_OPTIONS}")
  endif()
  if(NOT "${OPBUILD_LINK_OPTIONS}x" STREQUAL "x")
    string(REPLACE ";" " " EP_LINK_OPTIONS "${OPBUILD_LINK_OPTIONS}")
    list(APPEND compile_ops_command "--link-options=${EP_LINK_OPTIONS}")
  endif()
  if(DEFINED OPBUILD_JOBS)
    set(pos_int_regex "^[1-9][0-9]*$")
    if(NOT "${OPBUILD_JOBS}" MATCHES "${pos_int_regex}")
      message(FATAL_ERROR "JOBS must be positive number")
    endif()
    list(APPEND compile_ops_command "--parallel-jobs=${OPBUILD_JOBS}")
  endif()
  execute_process(COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${compile_ops_command}
    RESULT_VARIABLE EXEC_RESULT
    OUTPUT_VARIABLE EXEC_INFO
    ERROR_VARIABLE  EXEC_ERROR
  )
  if (NOT ${EXEC_RESULT} EQUAL 0)
    message("compile ascend_all_ops info: ${EXEC_INFO}")
    message("compile ascend_all_ops result: ${EXEC_RESULT}")
    message(FATAL_ERROR "opbuild run failed! ${EXEC_ERROR}")
  endif()

  if(DEFINED OPBUILD_OPTIONS)
    list(LENGTH OPBUILD_OPTIONS kv_count)
    math(EXPR max_index "${kv_count} - 1")
    foreach(i RANGE 0 ${max_index} 2)
      list(GET OPBUILD_OPTIONS ${i} key)
      math(EXPR j "${i} + 1")
      list(GET OPBUILD_OPTIONS ${j} value)
      if("${key}" STREQUAL "OPS_PROJECT_NAME")
        set(ENV{OPS_PROJECT_NAME} ${value})
      elseif("${key}" STREQUAL "OPS_DIRECT_ACCESS_PREFIX")
        set(ENV{OPS_DIRECT_ACCESS_PREFIX} ${value})
      elseif("${key}" STREQUAL "OPS_ACLNN_GEN")
        set(ENV{OPS_ACLNN_GEN} ${value})
      elseif("${key}" STREQUAL "OPS_ENABLE_SOURCE_PACKAGE")
        set(ENV{ENABLE_SOURCE_PACKAGE} ${value})
      elseif("${key}" STREQUAL "OPS_VENDOR_NAME")
        set(ENV{ASCEND_VENDOR_NAME} ${value})
      elseif("${key}" STREQUAL "OPS_PROTO_SEPARATE")
        set(ENV{OPS_PROTO_SEPARATE} ${value})
      endif()
    endforeach()
  endif()

  set(ENV{OPS_PRODUCT_NAME} "${ASCEND_COMPUTE_UNIT}")

  if (${OPBUILD_JOIN_OP_DEF})
    set(RAW_COMPUTE_UNIT_ARG "")
  else()
    set(RAW_COMPUTE_UNIT_ARG "--compute_unit=${ASCEND_COMPUTE_UNIT}")
  endif()
  execute_process(COMMAND ${ASCEND_CANN_PACKAGE_PATH}/tools/opbuild/op_build
                          ${OPBUILD_OUT_DIR}/libascend_all_ops.so ${OPBUILD_OUT_DIR} "${RAW_COMPUTE_UNIT_ARG}"
                  RESULT_VARIABLE EXEC_RESULT
                  OUTPUT_VARIABLE EXEC_INFO
                  ERROR_VARIABLE  EXEC_ERROR
  )
  set_property(GLOBAL PROPERTY ASCENDC_AUTO_GEN_PATH "${OPBUILD_OUT_DIR}")
  unset(ENV{OPS_PRODUCT_NAME})
  unset(ENV{OPS_PROJECT_NAME})
  unset(ENV{OPS_DIRECT_ACCESS_PREFIX})
  unset(ENV{OPS_ACLNN_GEN})
  unset(ENV{ENABLE_SOURCE_PACKAGE})
  unset(ENV{ASCEND_VENDOR_NAME})
  unset(ENV{OPS_PROTO_SEPARATE})

  if (NOT ${EXEC_RESULT} EQUAL 0)
    message("opbuild ops info: ${EXEC_INFO}")
    message("opbuild ops result: ${EXEC_RESULT}")
    message(FATAL_ERROR "opbuild ops error: ${EXEC_ERROR}")
  endif()

  if (${ASCEND_CHECK_OPTYPE_DUPLICATE})
    foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
      if (NOT EXISTS ${OPBUILD_OUT_DIR}/aic-${compute_unit}-ops-info.ini)
        message(FATAL_ERROR "file: ${OPBUILD_OUT_DIR}/aic-${compute_unit}-ops-info.ini not exist")
      endif()
      execute_process(COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_check_optype_duplicate.py
        --ini-file=${OPBUILD_OUT_DIR}/aic-${compute_unit}-ops-info.ini --soc-version=${compute_unit}
        RESULT_VARIABLE EXEC_RESULT
        OUTPUT_VARIABLE EXEC_INFO
        ERROR_VARIABLE  EXEC_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      if (NOT ${EXEC_RESULT} EQUAL 0)
        if (${EXEC_RESULT} EQUAL 2)
          message(FATAL_ERROR "Error: Op '${EXEC_ERROR}' is DUPLICATE with built-in operators")
        else()
          message(FATAL_ERROR "Error: ${EXEC_ERROR}")
        endif()
      endif()
    endforeach()
  endif()

  message(STATUS "Opbuild generating sources - done")
endfunction()

function(npu_op_library target_name target_type)
  string(TOUPPER ${target_type} _upper_target_type)
  set(support_types ACLNN TILING GRAPH TF_PLUGIN ONNX_PLUGIN)
  if(NOT _upper_target_type IN_LIST support_types)
      message(FATAL_ERROR "target_type ${target_type} does not support, the support list is ${support_types}")
  endif()

  cmake_parse_arguments(OPLIB "" "" "CONFIG" ${ARGN})

  set(SOURCES)
  foreach(_source ${OPLIB_UNPARSED_ARGUMENTS})
    get_filename_component(absolute_source "${_source}" ABSOLUTE)
    list(APPEND SOURCES ${absolute_source})
  endforeach()

  # Parse CONFIG key-value pairs and save as global properties
  if(OPLIB_CONFIG)
    list(LENGTH OPLIB_CONFIG kv_count)
    math(EXPR max_index "${kv_count} - 1")
    foreach(i RANGE 0 ${max_index} 2)
      list(GET OPLIB_CONFIG ${i} key)
      math(EXPR j "${i} + 1")
      list(GET OPLIB_CONFIG ${j} value)
      if("${key}" STREQUAL "SET_OPAPI_NAME")
        set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_SET_OPAPI_NAME ${value})
      elseif("${key}" STREQUAL "SET_TILING_NAME")
        set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_SET_TILING_NAME ${value})
      elseif("${key}" STREQUAL "SET_OP_PROTO_NAME")
        set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_SET_OPSPTOTO_NAME ${value})
      else()
        message(WARNING "CONFIG key '${key}' is not supported in npu_op_library")
      endif()
    endforeach()
  endif()

  if("${target_type}x" STREQUAL "ACLNNx")
    add_library(${target_name} OBJECT
      ${SOURCES}
    )

    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_name} PUBLIC
        ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
        ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    target_link_libraries(${target_name} PRIVATE
      intf_pub
      $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
      $<$<BOOL:${BUILD_WITH_INSTALLED_DEPENDENCY_CANN_PKG}>:acl_rt>
      $<$<NOT:$<BOOL:${BUILD_WITH_INSTALLED_DEPENDENCY_CANN_PKG}>>:ascendcl>
      nnopbase
    )
    get_property(tmp_aclnn_target GLOBAL PROPERTY ASCENDC_ACLNN_TARGET)
    list(APPEND tmp_aclnn_target ${target_name})
    set_property(GLOBAL PROPERTY ASCENDC_ACLNN_TARGET ${tmp_aclnn_target})
    set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_TYPE "ACLNN")
  elseif("${target_type}x" STREQUAL "TILINGx")
    add_library(${target_name} OBJECT
      ${SOURCES}
    )

    target_compile_definitions(${target_name} PRIVATE OP_TILING_LIB)
    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_name} PUBLIC
        ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
        ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    target_link_libraries(${target_name} PRIVATE
      nnopbase
      intf_pub
      $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
      exe_graph
      register
      tiling_api
      -Wl,--whole-archive
      rt2_registry
      -Wl,--no-whole-archive
      mmpa
    )
    get_property(tmp_tiling_target GLOBAL PROPERTY ASCENDC_TILING_TARGET)
    list(APPEND tmp_tiling_target ${target_name})
    set_property(GLOBAL PROPERTY ASCENDC_TILING_TARGET ${tmp_tiling_target})
    set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_TYPE "TILING")
  elseif("${target_type}x" STREQUAL "GRAPHx")
    add_library(${target_name} OBJECT
      ${SOURCES}
    )

    target_compile_definitions(${target_name} PRIVATE OP_PROTO_LIB)
    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_name} PUBLIC
        ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
        ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    target_link_libraries(${target_name} PRIVATE
      intf_pub
      $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
      exe_graph
      register
      tiling_api
      -Wl,--whole-archive
      rt2_registry
      -Wl,--no-whole-archive
    )
    get_property(tmp_graph_target GLOBAL PROPERTY ASCENDC_GRAPH_TARGET)
    list(APPEND tmp_graph_target ${target_name})
    set_property(GLOBAL PROPERTY ASCENDC_GRAPH_TARGET ${tmp_graph_target})
    set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_TYPE "GRAPH")
  elseif("${target_type}x" STREQUAL "TF_PLUGINx")
    add_library(${target_name} SHARED
      ${SOURCES}
    )
    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_name} PUBLIC
                              ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
                              ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    target_compile_definitions(${target_name} PRIVATE google=ascend_private)
    target_link_libraries(${target_name} PRIVATE
      intf_pub
      $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
      graph
    )
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME cust_tf_parsers)

    get_property(tmp_tf_plugin_target GLOBAL PROPERTY ASCENDC_TF_PLUGIN_TARGET)
    list(APPEND tmp_tf_plugin_target ${target_name})
    set_property(GLOBAL PROPERTY ASCENDC_TF_PLUGIN_TARGET ${tmp_tf_plugin_target})
    set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_TYPE "TF_PLUGIN")
  elseif("${target_type}x" STREQUAL "ONNX_PLUGINx")
    add_library(${target_name} SHARED
      ${SOURCES}
    )
    if(ENABLE_CROSS_COMPILE)
      target_link_directories(${target_name} PUBLIC
                              ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
                              ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
      )
    endif()
    target_compile_definitions(${target_name} PRIVATE google=ascend_private)
    target_link_libraries(${target_name} PRIVATE
      intf_pub
      $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
      graph
    )
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME cust_onnx_parsers)

    get_property(tmp_onnx_plugin_target GLOBAL PROPERTY ASCENDC_ONNX_PLUGIN_TARGET)
    list(APPEND tmp_onnx_plugin_target ${target_name})
    set_property(GLOBAL PROPERTY ASCENDC_ONNX_PLUGIN_TARGET ${tmp_onnx_plugin_target})
    set_property(GLOBAL PROPERTY _ASC_TGT_${target_name}_TYPE "ONNX_PLUGIN")
  endif()
endfunction()

function(get_current_pack_target sub_target output_var)
  get_property(package_aclnn_target GLOBAL PROPERTY ASCENDC_ACLNN_TARGET)
  get_property(package_tiling_target GLOBAL PROPERTY ASCENDC_TILING_TARGET)
  get_property(package_graph_target GLOBAL PROPERTY ASCENDC_GRAPH_TARGET)
  get_property(package_kernel_target GLOBAL PROPERTY ASCENDC_KERNEL_TARGET)
  get_property(package_tf_plugin_target GLOBAL PROPERTY ASCENDC_TF_PLUGIN_TARGET)
  get_property(package_onnx_plugin_target GLOBAL PROPERTY ASCENDC_ONNX_PLUGIN_TARGET)
  get_property(package_device_sink_target GLOBAL PROPERTY ASCENDC_DEVICE_SINK_TARGET)
  if(sub_target IN_LIST package_aclnn_target)
    set(${output_var} "ACLNN" PARENT_SCOPE)
  elseif(sub_target IN_LIST package_tiling_target)
    set(${output_var} "TILING" PARENT_SCOPE)
  elseif(sub_target IN_LIST package_graph_target)
    set(${output_var} "GRAPH" PARENT_SCOPE)
  elseif(sub_target IN_LIST package_kernel_target)
    set(${output_var} "KERNEL" PARENT_SCOPE)
  elseif(sub_target IN_LIST package_tf_plugin_target)
    set(${output_var} "TF_PLUGIN" PARENT_SCOPE)
  elseif(sub_target IN_LIST package_onnx_plugin_target)
    set(${output_var} "ONNX_PLUGIN" PARENT_SCOPE)
  elseif(sub_target IN_LIST package_device_sink_target)
    set(${output_var} "DEVICE_SINK" PARENT_SCOPE)
  else()
    message(WARNING "${sub_target} not in targets(${package_aclnn_target} ${package_tiling_target} ${package_graph_target} ${package_kernel_target})")
  endif()
endfunction()

function(npu_op_package_add target_package_name)
  cmake_parse_arguments(ADD_TARGET "" "PACKAGE_PATH" "TYPE;LIBRARY;FILES" ${ARGN})
  if(DEFINED ADD_TARGET_LIBRARY AND DEFINED ADD_TARGET_FILES)
    message(FATAL_ERROR
      "Error: Cannot specify both 'LIBRARY' and 'FILES' for package '${target_package_name}'.\n"
      "  - 'LIBRARY' is used to install compiled libraries (e.g., .so, .a)\n"
      "  - 'FILES' is used to install header/source files (e.g., .h, .cpp)\n"
      "  Please choose one or the other."
    )
  endif()
  get_property(output_type GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TYPE)
  get_property(auto_gen_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_CODE_GEN_DIRS)
  set(support_types SHARED STATIC RUN)
  if(NOT output_type IN_LIST support_types)
    message(FATAL_ERROR "${target_package_name} is not specified.")
  endif()
  get_property(current_target_vendor_name GLOBAL PROPERTY _ASC_PKG_${target_package_name}_VENDOR_NAME)
  if(DEFINED ADD_TARGET_FILES)
    if(NOT "${output_type}" STREQUAL "RUN")
      message(FATAL_ERROR "package add files only support on RUN mode")
    endif()
    if(DEFINED ADD_TARGET_PACKAGE_PATH AND DEFINED ADD_TARGET_TYPE)
      message(FATAL_ERROR "PACKAGE_PATH and LIBRARY can not defined at the same time when install files")
    endif()
    if(DEFINED ADD_TARGET_PACKAGE_PATH)
      adapt_install_path(
        INPUT_PATH "packages/vendors/${current_target_vendor_name}/${ADD_TARGET_PACKAGE_PATH}"
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(FILES ${ADD_TARGET_FILES}
          DESTINATION ${install_path} OPTIONAL)
    endif()
    if(DEFINED ADD_TARGET_TYPE)
      if("${ADD_TARGET_TYPE}x" STREQUAL "ACLNNx")
        adapt_install_path(
          INPUT_PATH "packages/vendors/${current_target_vendor_name}/op_api/include/"
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${ADD_TARGET_FILES}
          DESTINATION ${install_path} OPTIONAL)
      elseif("${ADD_TARGET_TYPE}x" STREQUAL "GRAPHx")
        adapt_install_path(
          INPUT_PATH "packages/vendors/${current_target_vendor_name}/op_proto/inc/"
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${ADD_TARGET_FILES}
            DESTINATION ${install_path} OPTIONAL)
      else()
        message(WARNING "${ADD_TARGET_TYPE} is not in support list [ACLNN, GRAPH]")
      endif()
    endif()
    return()
  endif()
  if(NOT DEFINED ADD_TARGET_LIBRARY)
    message(FATAL_ERROR "must provide LIBRARY for npu_op_package_add")
  endif()
  set(_ascendc_aclnn_target "")
  set(_ascendc_tiling_target "")
  set(_ascendc_graph_target "")
  set(_ascendc_kernel_target "")
  set(_ascendc_tf_plugin_target "")
  set(_ascendc_onnx_plugin_target "")
  set(_ascendc_device_sink_target "")
  foreach(_ascendc_sub_target ${ADD_TARGET_LIBRARY})
    set(_ascendc_enable_target)
    get_current_pack_target(${_ascendc_sub_target} _ascendc_enable_target)
    if("${_ascendc_enable_target}" STREQUAL "ACLNN")
      set(_ascendc_aclnn_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_aclnn_target}")
    elseif("${_ascendc_enable_target}" STREQUAL "GRAPH")
      set(_ascendc_graph_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_graph_target}")
    elseif("${_ascendc_enable_target}" STREQUAL "TILING")
      set(_ascendc_tiling_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_tiling_target}")
    elseif("${_ascendc_enable_target}" STREQUAL "KERNEL")
      set(_ascendc_kernel_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_kernel_target}")
    elseif("${_ascendc_enable_target}" STREQUAL "TF_PLUGIN")
      set(_ascendc_tf_plugin_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_tf_plugin_target}")
    elseif("${_ascendc_enable_target}" STREQUAL "ONNX_PLUGIN")
      set(_ascendc_onnx_plugin_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_onnx_plugin_target}")
    elseif("${_ascendc_enable_target}" STREQUAL "DEVICE_SINK")
      set(_ascendc_device_sink_target ${_ascendc_sub_target})
      message(INFO " target name: ${_ascendc_device_sink_target}")
    else()
      message(FATAL_ERROR "${_ascendc_sub_target} is not a valid library")
    endif()
  endforeach()
  if(_ascendc_device_sink_target)
    if(NOT "${output_type}" STREQUAL "RUN")
      message(FATAL_ERROR "device sink only supported in RUN mode")
    endif()
  endif()
  if(_ascendc_aclnn_target)
    if(NOT "${output_type}" STREQUAL "RUN")
      set(op_registry ${auto_gen_path}/custom_op_registry_V2.cpp)
      set_source_files_properties(${op_registry} PROPERTIES GENERATED TRUE)
      target_sources(${_ascendc_aclnn_target} PRIVATE ${op_registry})
      target_compile_definitions(${_ascendc_aclnn_target} PRIVATE ACLNN_WITH_BINARY)
    endif()
  endif()

  if(_ascendc_kernel_target)
    if("${auto_gen_path}" STREQUAL "")
      message(FATAL_ERROR "${target_package_name} has not set auto gen path.")
    endif()
    get_property(KERNEL_BINARY_PATH GLOBAL PROPERTY _ASC_TGT_KERNEL_${_ascendc_kernel_target}_BINARY_DIR)
    get_property(KERNEL_SRC_BASE GLOBAL PROPERTY _ASC_TGT_KERNEL_${_ascendc_kernel_target}_SRC_BASE)
    get_property(KERNEL_TILING_LIBRARY GLOBAL PROPERTY _ASC_TGT_KERNEL_${_ascendc_kernel_target}_TILING_LIBRARY)
    set(KERNEL_DYNAMIC_PATH ${KERNEL_BINARY_PATH}/binary/dynamic/)

    set(KERNEL_COMPILE_TARGETS "")
    set(KERNEL_COMPILE_TARGET_COUNT 0)

    foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
      # generate aic-${compute_unit}-ops-info.json
      add_ops_info_target(TARGET ${_ascendc_kernel_target}_ops_info_gen_${compute_unit}
        OUTPUT ${KERNEL_BINARY_PATH}/tbe/op_info_cfg/ai_core/${compute_unit}/aic-${compute_unit}-ops-info.json
        OPS_INFO ${auto_gen_path}/aic-${compute_unit}-ops-info.ini
      )
      add_dependencies(${_ascendc_kernel_target}_ops_info_gen_${compute_unit} ${_ascendc_kernel_target}_copy_kernel_srcs)
      # get op_type from aic-${compute_unit}-ops-info.ini
      execute_process(COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_get_op_type.py
            --ini-file=${auto_gen_path}/aic-${compute_unit}-ops-info.ini
        OUTPUT_VARIABLE OP_TYPE
        RESULT_VARIABLE EXEC_RESULT
        ERROR_VARIABLE EXEC_ERROR
      )
      if (${EXEC_RESULT})
        message(FATAL_ERROR, "get op type failed, gen error: ${EXEC_ERROR}")
      endif()

      # compile op one by one with ascendc_compile_kernel.py
      string(REPLACE "\n" ";" TYPE_NAME_LIST "${OP_TYPE}")
      foreach(TYPE_NAME IN LISTS TYPE_NAME_LIST)
        if (NOT "${TYPE_NAME}" STREQUAL "")
          string(REPLACE "-" ";" bin_sep ${TYPE_NAME})
          list(GET bin_sep 0 op_type)
          list(GET bin_sep 1 op_file)
          simple_kernel_compile(OP_TYPE ${op_type}
                                SRC ${KERNEL_SRC_BASE}/${op_file}.cpp
                                COMPUTE_UNIT ${compute_unit}
                                JSON_FILE ${KERNEL_BINARY_PATH}/tbe/op_info_cfg/ai_core/${compute_unit}/aic-${compute_unit}-ops-info.json
                                DYNAMIC_PATH ${KERNEL_DYNAMIC_PATH}
                                TILING_LIB ${KERNEL_TILING_LIBRARY}
                                TARGET_NAME ${_ascendc_kernel_target}
                                PACKAGE_NAME ${target_package_name}
                                OUT_DIR ${KERNEL_BINARY_PATH}/binary/)

          list(APPEND KERNEL_COMPILE_TARGETS ${op_type}_${compute_unit})
          math(EXPR KERNEL_COMPILE_TARGET_COUNT "${KERNEL_COMPILE_TARGET_COUNT} + 1")
        endif()
      endforeach()
    endforeach()

    if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
      get_property(error_file GLOBAL PROPERTY _ASC_PKG_ERROR_FILE)
      set(ERROR_LOG_FILE ${error_file})
      set(CHECK_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/check_all_fail_script.cmake")
      set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/all_fail_result_${target_package_name}.txt")

      # Get the number of failed operators
      file(WRITE "${CHECK_SCRIPT}" "
        set(ERROR_LOG_FILE \"${ERROR_LOG_FILE}\")
        set(OUTPUT_FILE \"${OUTPUT_FILE}\")
        if(EXISTS \"\${ERROR_LOG_FILE}\")
          file(READ \"\${ERROR_LOG_FILE}\" ERROR_LOG_CONTENT)
          execute_process(
            COMMAND bash -c \"grep -c 'fail op-type:' '\${ERROR_LOG_FILE}' || echo 0\"
            OUTPUT_VARIABLE FAILED_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
          )
          if(NOT FAILED_COUNT)
            set(FAILED_COUNT 0)
          endif()
          message(STATUS \"Found \${FAILED_COUNT} failed operators out of ${KERNEL_COMPILE_TARGET_COUNT} total\")
          if(\${FAILED_COUNT} EQUAL ${KERNEL_COMPILE_TARGET_COUNT})
            set(ALL_FAIL_RESULT TRUE)
          else()
            set(ALL_FAIL_RESULT FALSE)
          endif()
        else()
          set(ALL_FAIL_RESULT FALSE)
        endif()
        # Write the result to a file that can be read by func.cmake
        file(WRITE \"\${OUTPUT_FILE}\" \"\${ALL_FAIL_RESULT}\")
      ")

      add_custom_target(${target_package_name}_check_all_fail
        COMMAND ${CMAKE_COMMAND} -P "${CHECK_SCRIPT}"
        DEPENDS ${KERNEL_COMPILE_TARGETS}
      )
    endif()

  endif()

  if("${output_type}" STREQUAL "RUN")
    # Collect so-name replacements and execute in one target to avoid parallel
    # sed race on the same install.sh.
    set(update_so_name_commands "")
    set(enable_update_so_name OFF)
    if(_ascendc_tiling_target)
      # Read SET_TILING_NAME config to determine output name
      get_property(set_tiling_name GLOBAL PROPERTY _ASC_TGT_${_ascendc_tiling_target}_SET_TILING_NAME)
      if(set_tiling_name)
        set(tiling_output_name "${_ascendc_tiling_target}")
        message(WARNING "TILING library output name is customized to '${tiling_output_name}' instead of default 'cust_opmaster_rt2.0'. "
          "This may cause compatibility issues in RUN package scenarios.")
      else()
        set(tiling_output_name "cust_opmaster_rt2.0")
      endif()

      if(NOT TARGET ${target_package_name}_ascendc_${_ascendc_tiling_target})
        add_library(${target_package_name}_ascendc_${_ascendc_tiling_target} SHARED)
        set_target_properties(${target_package_name}_ascendc_${_ascendc_tiling_target} PROPERTIES OUTPUT_NAME ${tiling_output_name})
        add_custom_target(optiling_compat ALL
          COMMAND ln -sf lib/linux/${CMAKE_SYSTEM_PROCESSOR}/$<TARGET_FILE_NAME:${target_package_name}_ascendc_${_ascendc_tiling_target}>
          ${CMAKE_CURRENT_BINARY_DIR}/liboptiling.so
        )
      endif()
      target_sources(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE
        $<TARGET_OBJECTS:${_ascendc_tiling_target}>
      )
      target_link_libraries(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE intf_pub
        $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
        ${_ascendc_tiling_target})
      if(ENABLE_CROSS_COMPILE)
        target_link_directories(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE
          ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
          ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
        )
      endif()
      get_target_property(opmaster_so_name ${target_package_name}_ascendc_${_ascendc_tiling_target} OUTPUT_NAME)
      if(TARGET modify_vendor)
        list(APPEND update_so_name_commands
          COMMAND sed -i "s/opmaster_so_name=libcust_opmaster_rt2.0.so/opmaster_so_name=lib${opmaster_so_name}.so/g" ${CMAKE_BINARY_DIR}/scripts/install.sh
        )
        set(enable_update_so_name ON)
      endif()
      if(DEFINED ADD_TARGET_PACKAGE_PATH)
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TILING_PATH ${ADD_TARGET_PACKAGE_PATH})
      else()
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TILING_PATH "packages/vendors/${current_target_vendor_name}/op_impl/ai_core/tbe/op_tiling/")
      endif()
      get_property(tiling_package_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TILING_PATH)
      adapt_install_path(
        INPUT_PATH ${tiling_package_path}/lib/linux/${CMAKE_SYSTEM_PROCESSOR}
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(TARGETS ${target_package_name}_ascendc_${_ascendc_tiling_target}
        LIBRARY DESTINATION ${install_path})
      adapt_install_path(
        INPUT_PATH ${tiling_package_path}
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liboptiling.so
        DESTINATION ${install_path})
    endif()

    if(_ascendc_graph_target)
      # Read SET_OPSPTOTO_NAME config to determine output name
      get_property(set_opsproto_name GLOBAL PROPERTY _ASC_TGT_${_ascendc_graph_target}_SET_OPSPTOTO_NAME)
      if(set_opsproto_name)
        set(opsproto_output_name "${_ascendc_graph_target}")
        message(WARNING "GRAPH library output name is customized to '${opsproto_output_name}' instead of default 'cust_opsproto_rt2.0'. "
          "This may cause compatibility issues in RUN package scenarios.")
      else()
        set(opsproto_output_name "cust_opsproto_rt2.0")
      endif()
      if(NOT TARGET ${target_package_name}_ascendc_cust_op_proto)
        add_library(${target_package_name}_ascendc_cust_op_proto SHARED)
        set_target_properties(${target_package_name}_ascendc_cust_op_proto PROPERTIES OUTPUT_NAME ${opsproto_output_name})
      endif()
      get_target_property(opsproto_so_name ${target_package_name}_ascendc_cust_op_proto OUTPUT_NAME)
      if(TARGET modify_vendor)
        list(APPEND update_so_name_commands
          COMMAND sed -i "s/opsproto_so_name=libcust_opsproto_rt2.0.so/opsproto_so_name=lib${opsproto_so_name}.so/g" ${CMAKE_BINARY_DIR}/scripts/install.sh
        )
        set(enable_update_so_name ON)
      endif()
      target_sources(${target_package_name}_ascendc_cust_op_proto PRIVATE $<TARGET_OBJECTS:${_ascendc_graph_target}>)
      target_link_libraries(${target_package_name}_ascendc_cust_op_proto PRIVATE intf_pub
        $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
        ${_ascendc_graph_target})
      if(ENABLE_CROSS_COMPILE)
        target_link_directories(${target_package_name}_ascendc_cust_op_proto PRIVATE
          ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
          ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
        )
      endif()
      if(DEFINED ADD_TARGET_PACKAGE_PATH)
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_GRAPH_PATH ${ADD_TARGET_PACKAGE_PATH})
      else()
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_GRAPH_PATH "packages/vendors/${current_target_vendor_name}/op_proto/")
      endif()
      get_property(graph_package_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_GRAPH_PATH)
      adapt_install_path(
        INPUT_PATH ${graph_package_path}/lib/linux/${CMAKE_SYSTEM_PROCESSOR}
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(TARGETS ${target_package_name}_ascendc_cust_op_proto
        LIBRARY DESTINATION ${install_path})
      if(auto_gen_path)
        if(EXISTS "${auto_gen_path}/op_proto.h")
          adapt_install_path(
            INPUT_PATH ${graph_package_path}/inc
            INPUT_TARGET ${target_package_name}
            OUTPUT_PATH install_path
          )
          install(FILES ${auto_gen_path}/op_proto.h
              DESTINATION ${install_path})
        endif()
        file(GLOB GROUP_PROTO_HEADERS ${auto_gen_path}/group_proto/*.h)
        if(GROUP_PROTO_HEADERS)
          adapt_install_path(
            INPUT_PATH ${graph_package_path}/inc
            INPUT_TARGET ${target_package_name}
            OUTPUT_PATH install_path
          )
          install(FILES ${GROUP_PROTO_HEADERS}
              DESTINATION ${install_path})
        endif()
      endif()
    endif()

    if(_ascendc_aclnn_target)
      # Read SET_OPAPI_NAME config to determine output name
      get_property(set_opapi_name GLOBAL PROPERTY _ASC_TGT_${_ascendc_aclnn_target}_SET_OPAPI_NAME)
      if(set_opapi_name)
        set(opapi_output_name "${_ascendc_aclnn_target}")
      else()
        set(opapi_output_name "cust_opapi")
      endif()

      if(NOT TARGET ${target_package_name}_ascendc_cust_opapi)
        add_library(${target_package_name}_ascendc_cust_opapi SHARED)
        set_target_properties(${target_package_name}_ascendc_cust_opapi PROPERTIES OUTPUT_NAME ${opapi_output_name})
      endif()
      get_target_property(opapi_so_name ${target_package_name}_ascendc_cust_opapi OUTPUT_NAME)
      if(TARGET modify_vendor)
        list(APPEND update_so_name_commands
          COMMAND sed -i "s/opapi_so_name=libcust_opapi.so/opapi_so_name=lib${opapi_so_name}.so/g" ${CMAKE_BINARY_DIR}/scripts/install.sh
        )
        set(enable_update_so_name ON)
      endif()
      target_sources(${target_package_name}_ascendc_cust_opapi PRIVATE
        $<TARGET_OBJECTS:${_ascendc_aclnn_target}>
      )
      target_link_libraries(${target_package_name}_ascendc_cust_opapi PRIVATE intf_pub
        $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
        ${_ascendc_aclnn_target})
      if(ENABLE_CROSS_COMPILE)
        target_link_directories(${target_package_name}_ascendc_cust_opapi PRIVATE
          ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
          ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
        )
      endif()

      if(DEFINED ADD_TARGET_PACKAGE_PATH)
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ACLNN_PATH ${ADD_TARGET_PACKAGE_PATH})
      else()
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ACLNN_PATH "packages/vendors/${current_target_vendor_name}/op_api/")
      endif()
      get_property(aclnn_package_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ACLNN_PATH)
      adapt_install_path(
        INPUT_PATH ${aclnn_package_path}/lib
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(TARGETS ${target_package_name}_ascendc_cust_opapi
        LIBRARY DESTINATION ${install_path})
      if(auto_gen_path)
        file(GLOB aclnn_inc ${auto_gen_path}/aclnn_*.h)
        adapt_install_path(
          INPUT_PATH ${aclnn_package_path}/include
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${aclnn_inc}
                DESTINATION ${install_path})
      endif()
    endif()

    if(TARGET modify_vendor AND enable_update_so_name AND NOT TARGET ${target_package_name}_update_so_names)
      add_custom_target(${target_package_name}_update_so_names ALL
        ${update_so_name_commands}
        DEPENDS modify_vendor
      )
    endif()

    if(_ascendc_tf_plugin_target)
      if(DEFINED ADD_TARGET_PACKAGE_PATH)
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TF_PLUGIN_PATH ${ADD_TARGET_PACKAGE_PATH})
      else()
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TF_PLUGIN_PATH "packages/vendors/${current_target_vendor_name}/framework/tensorflow")
      endif()
      get_property(tf_plugin_package_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TF_PLUGIN_PATH)
      adapt_install_path(
        INPUT_PATH ${tf_plugin_package_path}
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(TARGETS ${_ascendc_tf_plugin_target}
        LIBRARY DESTINATION ${install_path}
      )
    endif()

    if(_ascendc_onnx_plugin_target)
      if(DEFINED ADD_TARGET_PACKAGE_PATH)
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ONNX_PLUGIN_PATH ${ADD_TARGET_PACKAGE_PATH})
      else()
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ONNX_PLUGIN_PATH "packages/vendors/${current_target_vendor_name}/framework/onnx")
      endif()
      get_property(onnx_plugin_package_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ONNX_PLUGIN_PATH)

      install(TARGETS ${_ascendc_onnx_plugin_target}
        LIBRARY DESTINATION ${onnx_plugin_package_path}
      )
    endif()

    if(_ascendc_device_sink_target)
      get_property(device_sink_target_output GLOBAL PROPERTY ASCENDC_DEVICE_SINK_TARGET_OUTPUT)
      if(NOT "${device_sink_target_output}" STREQUAL "")
        adapt_install_path(
          INPUT_PATH packages/vendors/${current_target_vendor_name}/op_impl/ai_core/tbe/op_master_device/lib
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${device_sink_target_output}
          DESTINATION ${install_path}
        )
      endif()
    endif()
  elseif("${output_type}" STREQUAL "STATIC")
    if(_ascendc_tiling_target)
      if(NOT TARGET ${target_package_name}_ascendc_${_ascendc_tiling_target})
        add_library(${target_package_name}_ascendc_${_ascendc_tiling_target} SHARED)
      endif()
      target_sources(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE $<TARGET_OBJECTS:${_ascendc_tiling_target}>)
      target_link_libraries(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE intf_pub
        $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
        ${_ascendc_tiling_target})
      if(ENABLE_CROSS_COMPILE)
        target_link_directories(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE
          ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
          ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
        )
      endif()
    endif()
    if(_ascendc_tiling_target)
      add_static_library(${target_package_name}_ascendc_static_file_target
        TYPE HOST
        PACKAGE_NAME ${target_package_name}
        TARGETS ${_ascendc_tiling_target})
    endif()
    if(_ascendc_graph_target)
      add_static_library(${target_package_name}_ascendc_static_file_target
        TYPE HOST
        PACKAGE_NAME ${target_package_name}
        TARGETS ${_ascendc_graph_target})
    endif()
    if(_ascendc_aclnn_target)
      add_static_library(${target_package_name}_ascendc_static_file_target
        TYPE HOST
        PACKAGE_NAME ${target_package_name}
        TARGETS ${_ascendc_aclnn_target})
    endif()
    if(_ascendc_kernel_target)
      add_static_library(${target_package_name}_ascendc_static_file_target
        TYPE KERNEL
        PACKAGE_NAME ${target_package_name}
        TARGETS ${_ascendc_kernel_target} ${_ascendc_kernel_target}_opregistry)
    endif()
  elseif("${output_type}" STREQUAL "SHARED")
    if(_ascendc_tiling_target)
      if(NOT TARGET ${target_package_name}_ascendc_${_ascendc_tiling_target})
        add_library(${target_package_name}_ascendc_${_ascendc_tiling_target} SHARED)
      endif()
      target_sources(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE
        $<TARGET_OBJECTS:${_ascendc_tiling_target}>
      )
      target_link_libraries(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE intf_pub
        $<IF:$<TARGET_EXISTS:cust_intf_pub>,cust_intf_pub,>
        ${_ascendc_tiling_target})
      if(ENABLE_CROSS_COMPILE)
        target_link_directories(${target_package_name}_ascendc_${_ascendc_tiling_target} PRIVATE
          ${ASCENDC_CMAKE_COMPILE_COMPILER_LIBRARY}
          ${ASCENDC_CMAKE_COMPILE_RUNTIME_LIBRARY}
        )
      endif()
    endif()
    if(_ascendc_tiling_target)
      target_sources(${target_package_name} PRIVATE $<TARGET_OBJECTS:${_ascendc_tiling_target}>)
      target_link_libraries(${target_package_name} PRIVATE ${_ascendc_tiling_target})
    endif()
    if(_ascendc_graph_target)
      target_sources(${target_package_name} PRIVATE $<TARGET_OBJECTS:${_ascendc_graph_target}>)
      target_link_libraries(${target_package_name} PRIVATE ${_ascendc_graph_target})
    endif()
    if(_ascendc_aclnn_target)
      target_sources(${target_package_name} PRIVATE $<TARGET_OBJECTS:${_ascendc_aclnn_target}>)
      target_link_libraries(${target_package_name} PRIVATE ${_ascendc_aclnn_target})
    endif()
    if(_ascendc_kernel_target)
      target_link_libraries(${target_package_name} PRIVATE ${_ascendc_kernel_target} ${_ascendc_kernel_target}_opregistry)
      add_dependencies(${target_package_name} ${_ascendc_kernel_target} ${_ascendc_kernel_target}_opregistry)
    endif()
  endif()

  if(_ascendc_kernel_target)
    if("${auto_gen_path}" STREQUAL "")
      message(FATAL_ERROR "${target_package_name} has not set auto gen path.")
    endif()
    get_property(kernel_binary_dir GLOBAL PROPERTY _ASC_TGT_KERNEL_${_ascendc_kernel_target}_BINARY_DIR)
    # define a target:binary to prevent kernel file from being rebuilt during the preinstall process
    if (NOT TARGET binary)
      add_custom_target(binary)
    endif()

    if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
      if (NOT TARGET ${target_package_name}_deal_all_fail)
        set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/all_fail_result_${target_package_name}.txt")
        add_custom_target(${target_package_name}_deal_all_fail
          COMMAND ${CMAKE_COMMAND} -D_OUTPUT_FILE=${OUTPUT_FILE} -P "${CMAKE_CURRENT_BINARY_DIR}/check_all_fail_${target_package_name}.cmake"
        )
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/check_all_fail_${target_package_name}.cmake" "
# Check if all kernels failed
if(EXISTS \"\${_OUTPUT_FILE}\")
  file(READ \"\${_OUTPUT_FILE}\" ALL_FAIL_VALUE)
  string(STRIP \"\${ALL_FAIL_VALUE}\" ALL_FAIL_VALUE)
  if(ALL_FAIL_VALUE STREQUAL \"TRUE\")
    message(FATAL_ERROR \"All kernels failed for ${target_package_name}. Cannot proceed with build.\")
  else()
    message(STATUS \"Some kernels succeeded for ${target_package_name}, continuing build.\")
  endif()
endif()
"
        )
      endif()
      add_dependencies(binary ${target_package_name}_deal_all_fail)
      add_dependencies(${target_package_name}_deal_all_fail ${target_package_name}_check_all_fail)
    endif()
    get_property(kernel_enable_binary_package GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_BINARY_PACKAGE)
    get_property(kernel_enable_source_package GLOBAL PROPERTY _ASC_PKG_${target_package_name}_ENABLE_SOURCE_PACKAGE)
    if(NOT "${output_type}" STREQUAL "RUN")
      if(NOT ${kernel_enable_binary_package})
        message(FATAL_ERROR "ENABLE_BINARY_PACKAGE must be true for SHARED/STATIC mode")
      endif()
    endif()
    foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
      if (${kernel_enable_binary_package} OR ${kernel_enable_source_package})
        if (${kernel_enable_binary_package})
          # gen binary_info_config.json and <file_name>.json
          if (${ASCEND_SKIP_FAILED_COMPUTE_UNIT})
            set(COMPILE_LOG "${CMAKE_CURRENT_BINARY_DIR}/compile.log")
            set(CONFIG_CMD
              "${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/insert_simplified_keys.py \
                  -p ${kernel_binary_dir}/binary/${compute_unit} && \
                  ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_ops_config.py \
                  -p ${kernel_binary_dir}/binary/${compute_unit} \
                  -s ${compute_unit} --skip-error ${ASCEND_SKIP_FAILED_COMPUTE_UNIT} && \
                  mkdir -p ${kernel_binary_dir}/binary/config/${compute_unit} && \
                  mv ${kernel_binary_dir}/binary/${compute_unit}/*.json \
                  ${kernel_binary_dir}/binary/config/${compute_unit} >> ${COMPILE_LOG} 2>&1 || true"
            )

            add_custom_target(${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config
                COMMAND bash -c "${CONFIG_CMD}"
                VERBATIM
            )
          else()
            add_custom_target(${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config
                COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/insert_simplified_keys.py
                        -p ${kernel_binary_dir}/binary/${compute_unit}
                COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_ops_config.py
                        -p ${kernel_binary_dir}/binary/${compute_unit}
                        -s ${compute_unit}
                COMMAND ${CMAKE_COMMAND} -E make_directory
                        ${kernel_binary_dir}/binary/config/${compute_unit}
                COMMAND mv ${kernel_binary_dir}/binary/${compute_unit}/*.json
                        ${kernel_binary_dir}/binary/config/${compute_unit})
          endif()
        else()
          if (NOT TARGET ${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config)
            add_custom_target(${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config)
          endif()
        endif()
        add_dependencies(binary ${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config)
        get_property(ascend_kernels GLOBAL PROPERTY ${_ascendc_kernel_target}_ASCENDC_KERNEL_TARGETS_${compute_unit})
        foreach(sub_kernels_target ${ascend_kernels})
          add_dependencies(${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config ${sub_kernels_target})
        endforeach()
      endif()
    endforeach()

    if("${output_type}" STREQUAL "RUN")
      # kernel
      # install kernel file
      if(DEFINED ADD_TARGET_PACKAGE_PATH)
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_KERNEL_PATH ${ADD_TARGET_PACKAGE_PATH})
      else()
        set_property(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_KERNEL_PATH "packages/vendors/${current_target_vendor_name}/op_impl/ai_core/tbe/${current_target_vendor_name}_impl/dynamic/")
      endif()
      # set(GLOBAL PROPERTY _ASC_PKG_${target_package_name}_TILING_PATH
      get_property(kernel_package_path GLOBAL PROPERTY _ASC_PKG_${target_package_name}_KERNEL_PATH)

      if (${kernel_enable_source_package})
        adapt_install_path(
          INPUT_PATH ${kernel_package_path}
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(DIRECTORY ${kernel_binary_dir}/binary/dynamic/
                DESTINATION ${install_path}
        )
      endif()

      if(ENABLE_COPY_KERNEL_SRC_TO_ASCENDC)
        adapt_install_path(
          INPUT_PATH ${kernel_package_path}/../ascendc
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(DIRECTORY ${kernel_binary_dir}/binary/dynamic/
        DESTINATION ${install_path}
        PATTERN "*.py" EXCLUDE
      )
      endif()

      # install *.o files and *.json files
      if (${kernel_enable_binary_package})
        set(INSTALL_DIR packages/vendors/${current_target_vendor_name}/op_impl/ai_core/tbe/)
        foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
          adapt_install_path(
            INPUT_PATH ${INSTALL_DIR}/kernel/${compute_unit}/
            INPUT_TARGET ${target_package_name}
            OUTPUT_PATH install_path
          )
          install(DIRECTORY ${kernel_binary_dir}/binary/${compute_unit}/
                  DESTINATION ${install_path}
          )
        endforeach()
        adapt_install_path(
          INPUT_PATH ${INSTALL_DIR}/kernel/config/
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(DIRECTORY ${kernel_binary_dir}/binary/config/
                DESTINATION ${install_path}
                OPTIONAL
        )
      endif()

      # install aci-xxx-ops-info.json
      foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
        adapt_install_path(
          INPUT_PATH packages/vendors/${current_target_vendor_name}/op_impl/ai_core/tbe/config/${compute_unit}
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${kernel_binary_dir}/tbe/op_info_cfg/ai_core/${compute_unit}/aic-${compute_unit}-ops-info.json
                DESTINATION ${install_path}
        )
      endforeach()
    endif()

    if(NOT "${output_type}" STREQUAL "RUN")
      if (NOT TARGET ${_ascendc_kernel_target}_op_kernel_pack)
        add_custom_target(${_ascendc_kernel_target}_op_kernel_pack
                          COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_pack_kernel.py
                          --input-path=${kernel_binary_dir}/binary
                          --output-path=${kernel_binary_dir}/binary/library
                          --enable-library=True
                          --platform=${CMAKE_SYSTEM_PROCESSOR})
        add_library(${_ascendc_kernel_target} INTERFACE)
        target_link_libraries(${_ascendc_kernel_target} INTERFACE kernels)
        target_link_directories(${_ascendc_kernel_target} INTERFACE ${kernel_binary_dir}/binary/library)
        target_include_directories(${_ascendc_kernel_target} INTERFACE ${kernel_binary_dir}/binary/library)
        add_dependencies(${_ascendc_kernel_target} ${_ascendc_kernel_target}_op_kernel_pack)
        foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
          add_dependencies(${_ascendc_kernel_target}_op_kernel_pack ${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config)
        endforeach()
      endif()
      if (NOT TARGET ${_ascendc_kernel_target}_op_registry_pack)
        string(REPLACE ";" "-" COMPUTE_UNIT "${ASCEND_COMPUTE_UNIT}")
        add_custom_command(OUTPUT ${auto_gen_path}/custom_op_registry_V2.cpp ${CMAKE_BINARY_DIR}/library/${_ascendc_kernel_target}/libopregistry.a
        COMMAND ${ASCEND_PYTHON_EXECUTABLE} ${ASCENDC_CMAKE_SCRIPTS_PATH}/util/ascendc_pack_opregistry.py
          --registry-file-path=${auto_gen_path}
          --input-path=${CMAKE_BINARY_DIR}/
          --base-path=${CMAKE_BINARY_DIR}/tmp/vendors/
          --output-path=${CMAKE_BINARY_DIR}/library/${_ascendc_kernel_target}/
          --vendor-name=${target_package_name}
          --compute-unit=${COMPUTE_UNIT}
          --framework-type=${ASCEND_FRAMEWORK_TYPE}
          --platform=${CMAKE_SYSTEM_PROCESSOR}
          --kernel-binary-path=${kernel_binary_dir}
        )

        add_custom_target(${_ascendc_kernel_target}_op_registry_pack
          DEPENDS ${auto_gen_path}/custom_op_registry_V2.cpp ${CMAKE_BINARY_DIR}/library/${_ascendc_kernel_target}/libopregistry.a
        )

        add_library(${_ascendc_kernel_target}_opregistry INTERFACE)
        target_link_libraries(${_ascendc_kernel_target}_opregistry INTERFACE opregistry)
        target_link_directories(${_ascendc_kernel_target}_opregistry INTERFACE ${CMAKE_BINARY_DIR}/library/${_ascendc_kernel_target})
        target_include_directories(${_ascendc_kernel_target}_opregistry INTERFACE ${CMAKE_BINARY_DIR}/library/${_ascendc_kernel_target})
        add_dependencies(${_ascendc_kernel_target}_opregistry ${_ascendc_kernel_target}_op_registry_pack ${auto_gen_path}/custom_op_registry_V2.cpp)
        foreach(compute_unit ${ASCEND_COMPUTE_UNIT})
          add_dependencies(${_ascendc_kernel_target}_op_registry_pack ${_ascendc_kernel_target}_ascendc_bin_${compute_unit}_gen_ops_config)
        endforeach()

        if(_ascendc_tf_plugin_target)
          add_dependencies(${_ascendc_kernel_target}_op_registry_pack ${_ascendc_tf_plugin_target})
        endif()
        if(_ascendc_onnx_plugin_target)
          add_dependencies(${_ascendc_kernel_target}_op_registry_pack ${_ascendc_onnx_plugin_target})
        endif()
      endif()

      get_property(package_aclnn_target GLOBAL PROPERTY ASCENDC_ACLNN_TARGET)
      if(package_aclnn_target)
        target_link_libraries(${package_aclnn_target} PRIVATE ${_ascendc_kernel_target} ${_ascendc_kernel_target}_opregistry)
        add_dependencies(${package_aclnn_target} ${_ascendc_kernel_target} ${_ascendc_kernel_target}_opregistry)
      endif()
    endif()
  endif()

  # install aclnn && proto files
  if(NOT "${output_type}" STREQUAL "RUN")
    if(auto_gen_path)
      file(GLOB aclnn_inc ${auto_gen_path}/aclnn_*.h)
      adapt_install_path(
        INPUT_PATH include
        INPUT_TARGET ${target_package_name}
        OUTPUT_PATH install_path
      )
      install(FILES ${aclnn_inc}
              DESTINATION ${install_path})
      if(EXISTS "${auto_gen_path}/op_proto.h")
        adapt_install_path(
          INPUT_PATH include
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${auto_gen_path}/op_proto.h
            DESTINATION ${install_path})
      endif()
      file(GLOB GROUP_PROTO_HEADERS ${auto_gen_path}/group_proto/*.h)
      if(GROUP_PROTO_HEADERS)
        adapt_install_path(
          INPUT_PATH include
          INPUT_TARGET ${target_package_name}
          OUTPUT_PATH install_path
        )
        install(FILES ${GROUP_PROTO_HEADERS}
            DESTINATION ${install_path})
      endif()
    endif()
  endif()

  get_property(npu_support_target_flag GLOBAL PROPERTY ${target_package_name}_ASCENDC_GEN_NPU_SUPPORT_TARGET_FLAG)
  if(NOT npu_support_target_flag)
    if(_ascendc_kernel_target)
      if("${auto_gen_path}" STREQUAL "")
        message(FATAL_ERROR "${target_package_name} has not set auto gen path.")
      endif()
      get_property(kernel_binary_dir GLOBAL PROPERTY _ASC_TGT_KERNEL_${_ascendc_kernel_target}_BINARY_DIR)
      set_property(GLOBAL PROPERTY ${target_package_name}_ASCENDC_GEN_NPU_SUPPORT_TARGET_FLAG True)
      add_npu_support_target(TARGET ${target_package_name}_npu_supported_ops
        OPS_INFO_DIR ${auto_gen_path}
        OUT_DIR ${kernel_binary_dir}/tbe/op_info_cfg/ai_core
        INSTALL_DIR packages/vendors/${current_target_vendor_name}/framework/${ASCEND_FRAMEWORK_TYPE}
        PACKAGE_NAME ${target_package_name}
      )
    endif()
  endif()

endfunction()
