# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set(ASCENDC_API_PATH @INSTALL_LIBRARY_DIR@)
set(ASCENDC_INSTALL_BASE_PATH $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${ASCENDC_API_PATH})

file(MAKE_DIRECTORY ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/adv_api/detail)
file(
    CREATE_LINK ../../utils/std
    ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/adv_api/detail/std
    SYMBOLIC)

file(MAKE_DIRECTORY ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/basic_api/utils)
file(
    CREATE_LINK ../../utils/std
    ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/basic_api/utils/std
    SYMBOLIC)
file(
    CREATE_LINK ../../utils/debug
    ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/basic_api/utils/debug
    SYMBOLIC)
file(
    CREATE_LINK ../../utils/common_types.h
    ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/basic_api/utils/common_types.h
    SYMBOLIC)
file(
    CREATE_LINK ../../utils/sys_constants.h
    ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/basic_api/utils/sys_constants.h
    SYMBOLIC)
file(
    CREATE_LINK ../../utils/sys_macros.h
    ${ASCENDC_INSTALL_BASE_PATH}/asc/impl/basic_api/utils/sys_macros.h
    SYMBOLIC)
