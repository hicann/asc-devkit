# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

add_cann_third_party(makeself-fetch)

set(script_prefix ${CMAKE_CURRENT_SOURCE_DIR}/scripts/package/scripts/)
install(DIRECTORY ${script_prefix}/
    DESTINATION share/info/asc-devkit/script
    COMPONENT asc-devkit
    FILE_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE  # 文件权限
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE  # 目录权限
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)
set(SCRIPTS_FILES
    ${CANN_CMAKE_DIR}/scripts/install/check_version_required.awk
    ${CANN_CMAKE_DIR}/scripts/install/common_func.inc
    ${CANN_CMAKE_DIR}/scripts/install/common_interface.sh
    ${CANN_CMAKE_DIR}/scripts/install/common_interface.csh
    ${CANN_CMAKE_DIR}/scripts/install/common_interface.fish
    ${CANN_CMAKE_DIR}/scripts/install/version_compatiable.inc
    ${CANN_CMAKE_DIR}/scripts/package/merge_binary_info_config.py
)

install(FILES ${SCRIPTS_FILES}
    DESTINATION share/info/asc-devkit/script COMPONENT asc-devkit
)

set(COMMON_FILES
    ${CANN_CMAKE_DIR}/scripts/install/install_common_parser.sh
    ${CANN_CMAKE_DIR}/scripts/install/common_func_v2.inc
    ${CANN_CMAKE_DIR}/scripts/install/common_installer.inc
    ${CANN_CMAKE_DIR}/scripts/install/script_operator.inc
    ${CANN_CMAKE_DIR}/scripts/install/version_cfg.inc
)

set(PACKAGE_FILES
    ${COMMON_FILES}
    ${CANN_CMAKE_DIR}/scripts/install/multi_version.inc
)

set(CONF_FILES
    ${CANN_CMAKE_DIR}/scripts/package/cfg/path.cfg
)
install(FILES ${CONF_FILES}
    DESTINATION ${CMAKE_SYSTEM_PROCESSOR}-linux/conf COMPONENT asc-devkit
)
install(FILES ${PACKAGE_FILES}
   DESTINATION share/info/asc-devkit/script COMPONENT asc-devkit
)
install(FILES ${CMAKE_BINARY_DIR}/version.asc-devkit.info
    DESTINATION share/info/asc-devkit
    RENAME version.info
    COMPONENT asc-devkit
)

# 安装到目标位置
install(DIRECTORY ${MAKESELF_PATH}
        DESTINATION ${INSTALL_LIBRARY_DIR}/tikcpp/ascendc_kernel_cmake/fwk_modules/util
        FILE_PERMISSIONS
            OWNER_READ OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
        COMPONENT asc-devkit
        PATTERN ".github" EXCLUDE
        PATTERN ".gitignore" EXCLUDE
        PATTERN ".gitmodules" EXCLUDE
        PATTERN "test" EXCLUDE
)

if (NOT ENABLE_COV AND NOT ENABLE_UT)
    set_cann_cpack_config(
        asc-devkit
        OUTPUT "${CMAKE_SOURCE_DIR}/build_out"
        ENABLE_DEVICE "${ENABLE_BUILD_DEVICE}"
        PACKAGE_TYPE "${PACKAGE_TYPE}"
    )
endif()
