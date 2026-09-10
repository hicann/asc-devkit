# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

# copy std to highlevel_api impl
install(
  DIRECTORY std debug base
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/utils
  ${INSTALL_OPTIONAL} COMPONENT asc-devkit FILES_MATCHING
  PATTERN "*.h"
  PATTERN "CMakeLists.txt" EXCLUDE)

install(
  DIRECTORY ${ASCENDC_INCLUDE_DIR}/utils/
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include/utils/ ${INSTALL_OPTIONAL} COMPONENT asc-devkit
  PATTERN "utils/stub" EXCLUDE
)

install(FILES common_types.h sys_constants.h sys_macros.h
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/utils ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

if(NOT BUILD_OPEN_PROJECT)
    install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/host_api/include/tiling)" COMPONENT asc-devkit)
    install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/host_api/include/log)" COMPONENT asc-devkit)
    install(CODE "file(CREATE_LINK ../../../../../asc/include/utils/tiling/template_argument.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/host_api/include/tiling/template_argument.h SYMBOLIC)" COMPONENT asc-devkit)
    install(CODE "file(CREATE_LINK ../../../../../asc/include/utils/log/asc_cpu_log.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/host_api/include/log/asc_cpu_log.h SYMBOLIC)" COMPONENT asc-devkit)
endif()
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/tiling/platform)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/tiling/context)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/highlevel_api/utils/log)" COMPONENT asc-devkit)

install(CODE "file(CREATE_LINK ../../utils/tiling/platform/platform_ascendc.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/tiling/platform/platform_ascendc.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../utils/context/context_builder.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/tiling/context/context_builder.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../utils/tiling/template_argument.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/tiling/template_argument.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../../asc/include/utils/log/asc_cpu_log.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/highlevel_api/utils/log/asc_cpu_log.h SYMBOLIC)" COMPONENT asc-devkit)

set(TEMPLATE_ARG   ${CMAKE_CURRENT_BINARY_DIR})
set(TEMPLATE_ARG_HAED  ${TEMPLATE_ARG}/ascendc/host_api/tiling/template_argument.h)
set(ASC_CPU_LOG_HAED  ${CMAKE_CURRENT_BINARY_DIR}/ascendc/host_api/log/asc_cpu_log.h)

add_custom_command(OUTPUT ${TEMPLATE_ARG_HAED}
    COMMAND ${CMAKE_COMMAND} -E copy ${ASCENDC_DIR}/include/utils/tiling/template_argument.h ${TEMPLATE_ARG_HAED}
    DEPENDS ${ASCENDC_DIR}/include/utils/tiling/template_argument.h)

add_custom_command(OUTPUT ${ASC_CPU_LOG_HAED}
    COMMAND ${CMAKE_COMMAND} -E copy ${ASCENDC_DIR}/include/utils/log/asc_cpu_log.h ${ASC_CPU_LOG_HAED}
    DEPENDS ${ASCENDC_DIR}/include/utils/log/asc_cpu_log.h)

add_custom_target(template_argument ALL
    DEPENDS ${TEMPLATE_ARG_HAED})

add_custom_target(tiling_log_target ALL
    DEPENDS ${ASC_CPU_LOG_HAED})

add_dependencies(kernel_tiling_headers template_argument tiling_log_target)
target_include_directories(kernel_tiling_headers INTERFACE
    $<BUILD_INTERFACE:${TEMPLATE_ARG}>
)
