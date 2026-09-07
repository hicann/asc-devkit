# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/c_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

install(DIRECTORY ${ASCENDC_IMPL_DIR}/c_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

# Keep legacy installed include paths available during the compatibility period.
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/c_api/sys_var\")" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../defs/enum.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/c_api/utils/enum.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../utils/sys_var.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/c_api/sys_var/sys_var.h SYMBOLIC)" COMPONENT asc-devkit)
