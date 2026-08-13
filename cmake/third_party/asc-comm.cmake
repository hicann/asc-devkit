# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

# 后续如果更新asc-comm代码需要同步更新至devkit run包需要更新change id
set(ASC_COMM_TAG_ID 823662c58068ec0f588923630774a184403322dc)

# asc-comm 与 asc-devkit 按同级目录放置。路径以本文件所在目录(<devkit>/cmake/third_party)为锚，
# 对于CI环境已经存在asc-comm代码, 不去拉取代码否则使用submodule方式拉取asc-comm仓代码
message(STATUS "[ThirdPartyLib][asc-comm] project source dir: ${PROJECT_SOURCE_DIR}")
get_filename_component(_ASC_COMM_DEVKIT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." REALPATH)
set(ASC_COMM_SOURCE_DIR "${_ASC_COMM_DEVKIT_ROOT}/../asc-comm")
if(EXISTS "${ASC_COMM_SOURCE_DIR}")
    get_filename_component(ASC_COMM_SOURCE_PATH ${ASC_COMM_SOURCE_DIR} REALPATH)
    message(STATUS "[ThirdPartyLib][asc-comm] Find source dir: ${ASC_COMM_SOURCE_PATH}")
else()
    if(EXISTS "${CMAKE_BINARY_DIR}/_deps/asc-comm-subbuild")
        file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/_deps/asc-comm-subbuild)
    endif()
    include(FetchContent)

    FetchContent_Declare(
        asc-comm
        GIT_REPOSITORY https://gitcode.com/cann/asc-comm.git
        GIT_TAG ${ASC_COMM_TAG_ID}
        GIT_PROGRESS TRUE
        SOURCE_DIR ${ASC_COMM_SOURCE_DIR}
    )
    FetchContent_Populate(asc-comm)

    get_filename_component(ASC_COMM_SOURCE_PATH ${ASC_COMM_SOURCE_DIR} REALPATH)
endif()

# asc-comm 迁出后，将 src 与 include 目录统一打包到 comm_api（hcomm 除外）；
# hcomm 的 include 和 impl 代码分别打包到 adv_api 和 adv_api/detail 下，
# 并在 comm_api 下建立 hcomm 的软链接指向 adv_api。
set(ASC_COMM_SRC_DIR ${ASC_COMM_SOURCE_PATH}/src)
set(ASC_COMM_INCLUDE_DIR ${ASC_COMM_SOURCE_PATH}/include)
if(NOT EXISTS ${ASC_COMM_SRC_DIR})
    message(FATAL_ERROR "[ThirdPartyLib][asc-comm] Missing src dir: ${ASC_COMM_SRC_DIR}")
endif()
if(NOT EXISTS ${ASC_COMM_INCLUDE_DIR})
    message(FATAL_ERROR "[ThirdPartyLib][asc-comm] Missing include dir: ${ASC_COMM_INCLUDE_DIR}")
endif()

# 将 src 下代码（hcomm 除外）统一打包到 asc/impl/comm_api/
install(DIRECTORY ${ASC_COMM_SRC_DIR}/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/comm_api
    COMPONENT asc-devkit
    FILES_MATCHING PATTERN "*.h"
    REGEX "aicore/hcomm" EXCLUDE
)

# 将 include 下代码（hcomm 和 direct_drive 除外）统一打包到 asc/include/comm_api/
install(DIRECTORY ${ASC_COMM_INCLUDE_DIR}/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include/comm_api
    COMPONENT asc-devkit
    FILES_MATCHING PATTERN "*.h"
    REGEX "aicore/hcomm" EXCLUDE
    REGEX "direct_drive" EXCLUDE
)

# 将 hcomm 的 include 打包到 asc/include/adv_api/hcomm/
install(DIRECTORY ${ASC_COMM_INCLUDE_DIR}/aicore/hcomm/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include/adv_api/hcomm
    COMPONENT asc-devkit
    FILES_MATCHING PATTERN "*.h"
)

# 将 hcomm 的 impl 代码打包到 asc/impl/adv_api/detail/hcomm/
install(DIRECTORY ${ASC_COMM_SRC_DIR}/aicore/hcomm/
    DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl/adv_api/detail/hcomm
    COMPONENT asc-devkit
    FILES_MATCHING PATTERN "*.h"
)

# 在 comm_api 下为 hcomm 建立软链接，指向 adv_api 下的实际内容
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/comm_api/aicore)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/impl/comm_api/aicore)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../adv_api/hcomm \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/comm_api/aicore/hcomm SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../adv_api/detail/hcomm \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/impl/comm_api/aicore/hcomm SYMBOLIC)" COMPONENT asc-devkit)
