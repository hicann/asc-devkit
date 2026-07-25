# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
add_compile_options(-Wno-error)

include(gen_kernel_tiling.cmake)

# generate tiling_api.h softlink
if(NOT BUILD_OPEN_PROJECT)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/tiling/tiling_api.h
    COMMAND mkdir ${CMAKE_BINARY_DIR}/tiling
    COMMAND ln -s ${ASCENDC_INCLUDE_DIR}/adv_api/tiling_api.h
            ${CMAKE_BINARY_DIR}/tiling/tiling_api.h
    DEPENDS ${ASCENDC_INCLUDE_DIR}/adv_api/tiling_api.h)
  add_custom_target(gen_kernel_api ALL
                    DEPENDS ${CMAKE_BINARY_DIR}/tiling/tiling_api.h)
  add_dependencies(kernel_tiling_headers gen_kernel_api)
endif()

add_subdirectory(tiling)

if (BUILD_OPEN_PROJECT)
  add_cann_device_project(asc-devkit)
endif()


# copy include to asc impl
if(NOT "${DPRODUCT_SIDE}" STREQUAL "device")
  install(
    FILES ${KERNEL_TILING_HEAD}
    DESTINATION
      ${INSTALL_LIBRARY_DIR}/asc/include/adv_api/
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit)

  # copy detail to asc impl
  install(
    DIRECTORY detail/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/adv_api/detail
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit FILES_MATCHING
    PATTERN "*.h"
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "hccl_inner_dl.h" EXCLUDE
    PATTERN "hccl_rank_graph_dl.h" EXCLUDE
    PATTERN "hccl_res_dl.h" EXCLUDE
    PATTERN "hcomm_dlsym.h" EXCLUDE
    PATTERN "hcomm_host_profiling_dl.h" EXCLUDE
    REGEX "common/adapter_error_manager_pub\\.h$" EXCLUDE)

  # copy tiling to asc impl
  install(
    DIRECTORY tiling/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/adv_api/tiling
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit FILES_MATCHING
    PATTERN "*.h"
    PATTERN "CMakeLists.txt" EXCLUDE)

  # cp include to asc include
  install(
    DIRECTORY ${ASCENDC_INCLUDE_DIR}/adv_api/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include/adv_api/
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit
    REGEX "hccl/internal" EXCLUDE)

  install(
    FILES
      ${ASCENDC_INCLUDE_DIR}/adv_api/hccl/internal/hccl_msg.h
      ${ASCENDC_INCLUDE_DIR}/adv_api/hccl/internal/hccl_tiling_msg.h
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include/adv_api/hccl/internal
    ${INSTALL_OPTIONAL} COMPONENT asc-devkit)

  # generate softlink
  configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/kernel_headers.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/kernel_headers.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kernel_headers.cmake COMPONENT asc-devkit)

  configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/kernel_impl.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/kernel_impl.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kernel_impl.cmake COMPONENT asc-devkit)

  configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/kernel_directory.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/kernel_directory.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kernel_directory.cmake COMPONENT asc-devkit)

  configure_file(
      ${ASCENDC_ADV_API_CMAKE_DIR}/utils_headers.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/utils_headers.cmake
      @ONLY
  )
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/utils_headers.cmake COMPONENT asc-devkit)
endif()

if (BUILD_OPEN_PROJECT)
    add_subdirectory(detail/hccl/cc)
endif()

if(NOT BUILD_OPEN_PROJECT)
  install_package(
    PACKAGE kernel_tiling
    TARGETS kernel_tiling_headers
    FILES ${KERNEL_TILING_HEAD}
    DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include/adv_api/)
endif()
