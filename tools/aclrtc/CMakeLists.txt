# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ SRC)

add_library(acl_rtc SHARED
    ${SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/specialization/resource_registry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/common/file_utils.cpp
)

if(BUILD_OPEN_PROJECT)
    target_include_directories(acl_rtc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
    target_include_directories(acl_rtc PRIVATE
        ${ASCEND_CANN_PACKAGE_PATH}/include
        ${ASCEND_CANN_PACKAGE_PATH}/include/acl
        ${ASCEND_CANN_PACKAGE_PATH}/include/pkg_inc/base/
        ${CCEC_PATH}/../include
    )
    target_link_libraries(acl_rtc PRIVATE
        acl_rt
    )
else()
    target_include_directories(acl_rtc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${TOP_DIR}/runtime/include/external
 	    ${TOP_DIR}/runtime/include/external/acl
        ${CCEC_PATH}/../include
    )
endif()

target_include_directories(acl_rtc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/specialization
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/common
)

target_link_directories(acl_rtc PRIVATE
    ${CCEC_PATH}/../lib
)

target_link_libraries(acl_rtc PRIVATE
    Boost::filesystem
    $<BUILD_INTERFACE:intf_pub>
    $<BUILD_INTERFACE:mmpa_headers>
    $<BUILD_INTERFACE:metadef_headers>
    dl
    c_sec_static
    unified_dlog
    mmpa
    json
)

target_compile_options(acl_rtc PRIVATE
    -fPIE
    -fno-strict-aliasing
    -fno-common
    -Wall
    -Wextra
    -Wfloat-equal
    -Werror
    -Wformat
    -D_FORTIFY_SOURCE=2
    -Os
)

target_link_options(acl_rtc PRIVATE
    -Wl,-z,relro,-z,now,-z,noexecstack
    -Wl,-Bsymbolic
    -Wl,--gc-sections
    $<$<CONFIG:Release>:-s>
)

install(TARGETS acl_rtc
    LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}/lib64 ${INSTALL_OPTIONAL} COMPONENT asc-devkit
    ARCHIVE DESTINATION ${INSTALL_LIBRARY_DIR}/lib64 ${INSTALL_OPTIONAL} COMPONENT asc-devkit
    RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}/lib64 ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/acl/acl_rt_compile.h
    DESTINATION ${INSTALL_LIBRARY_DIR}/include/acl COMPONENT asc-devkit
)
