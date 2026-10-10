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
set(ASC_COMM_TAG_ID 140efa94dc4090ddc84f7b34e6b56ea0c1b313d0)

# asc-comm 与 asc-devkit 按同级目录放置。路径以本文件所在目录(<devkit>/cmake/third_party)为锚，
# 对于CI环境已经存在asc-comm代码, 不去拉取代码否则使用submodule方式拉取asc-comm仓代码
message(STATUS "[ThirdPartyLib][asc-comm] project source dir: ${PROJECT_SOURCE_DIR}")
get_filename_component(_ASC_COMM_DEVKIT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." REALPATH)
if(NOT ASC_COMM_SOURCE_DIR)
    foreach(_asc_comm_candidate
        "${_ASC_COMM_DEVKIT_ROOT}/asc-comm"
        "${_ASC_COMM_DEVKIT_ROOT}/../asc-comm"
        "${_ASC_COMM_DEVKIT_ROOT}/../asc/asc-comm"
        "${_ASC_COMM_DEVKIT_ROOT}/../../asc-comm"
        "${_ASC_COMM_DEVKIT_ROOT}/../../asc/asc-comm"
        "${_ASC_COMM_DEVKIT_ROOT}/../../../asc-comm"
        "${_ASC_COMM_DEVKIT_ROOT}/../../../asc/asc-comm")
        if(EXISTS "${_asc_comm_candidate}/CMakeLists.txt")
            set(ASC_COMM_SOURCE_DIR "${_asc_comm_candidate}")
            break()
        endif()
    endforeach()
endif()

if(ASC_COMM_SOURCE_DIR AND EXISTS "${ASC_COMM_SOURCE_DIR}")
    get_filename_component(ASC_COMM_SOURCE_PATH ${ASC_COMM_SOURCE_DIR} REALPATH)
    message(STATUS "[ThirdPartyLib][asc-comm] Find source dir: ${ASC_COMM_SOURCE_PATH}")
else()
    set(ASC_COMM_SOURCE_DIR "${_ASC_COMM_DEVKIT_ROOT}/../asc-comm")
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
    PATTERN "hcomm" EXCLUDE
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

# 构建 asc-comm CCU 动态库（数据面实现 + host-launch 兼容 wrapper），
# 并将库与公开 DSL 头文件交付到 asc-devkit run 包。
# CCU 源码归属 asc-comm，跨 SO 依赖通过已安装 CANN pkg_inc 和 POD ABI 解决。
set(ASC_COMM_CCU_CMAKE_DIR ${ASC_COMM_SOURCE_PATH}/src/ccu)
if(EXISTS "${ASC_COMM_CCU_CMAKE_DIR}/CMakeLists.txt")
    if(NOT TARGET asccomm_ccu)
        add_subdirectory(${ASC_COMM_CCU_CMAKE_DIR} ${CMAKE_BINARY_DIR}/asccomm_ccu)
    endif()

    if(TARGET asccomm_ccu)
        if(NOT TARGET asccomm_ccu_build)
            add_custom_target(asccomm_ccu_build ALL DEPENDS asccomm_ccu)
        endif()
        install(TARGETS asccomm_ccu
            LIBRARY DESTINATION ${CMAKE_SYSTEM_PROCESSOR}-linux/lib64 ${INSTALL_OPTIONAL}
            COMPONENT asc-devkit)
        install(DIRECTORY ${ASC_COMM_SOURCE_PATH}/include/ccu/hcomm/
            DESTINATION ${CMAKE_SYSTEM_PROCESSOR}-linux/include/ccu/hcomm
            COMPONENT asc-devkit
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
    endif()
else()
    message(STATUS "[ThirdPartyLib][asc-comm] Missing CCU cmake dir: ${ASC_COMM_CCU_CMAKE_DIR}, skip CCU so build")
endif()
