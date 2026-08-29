# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(HCCL_CC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HCCL_CC_INCLUDE_DIR ${ASCENDC_DIR}/include/adv_api/hccl/internal)
set(HCCL_LOCAL_HEADER_DIRS
    ${ASCENDC_DIR}
    ${HCCL_CC_INCLUDE_DIR}
)

if(BUILD_OPEN_PROJECT AND KERNEL_MODE)
    set(TOOLCHAIN_DIR ${ASCEND_CANN_PACKAGE_PATH}/toolkit/toolchain/hcc)
    message(STATUS "TOOLCHAIN_DIR=${TOOLCHAIN_DIR}")
    if (NOT "x${TOOLCHAIN_DIR}" STREQUAL "x")
        if (NOT IS_DIRECTORY ${TOOLCHAIN_DIR})
            message(FATAL_ERROR "specify cross compile toolchain directory(${TOOLCHAIN_DIR}) is not exist")
        endif()
    endif()
    if (NOT EXISTS ${TOOLCHAIN_DIR}/bin/aarch64-target-linux-gnu-gcc OR
        NOT EXISTS ${TOOLCHAIN_DIR}/bin/aarch64-target-linux-gnu-g++)
        message(FATAL_ERROR "aarch64 cross compiler is not found in ${TOOLCHAIN_DIR}/bin")
    endif()
endif()

set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")

set(CMAKE_CXX_COMPILE_OBJECT
    "<CMAKE_CXX_COMPILER> <DEFINES> -D__FILE__='\"$(notdir $(abspath <SOURCE>))\"' -Wno-builtin-macro-redefined <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>"
)
set(CMAKE_C_COMPILE_OBJECT
    "<CMAKE_C_COMPILER> <DEFINES> -D__FILE__='\"$(notdir $(abspath <SOURCE>))\"' -Wno-builtin-macro-redefined <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>"
)

if(ENABLE_CUSTOM)
    # 自定义算子打包
    if(NOT CUSTOM_OPS_NAME)
        message(FATAL_ERROR "Custom ops name is not set")
    endif()
    if(NOT CUSTOM_OPS_PATH)
        message(FATAL_ERROR "Custom ops path is not set")
    endif()
    if(NOT CUSTOM_OPS_VENDOR)
        message(FATAL_ERROR "Custom ops vendor is not set")
    endif()

    if(NOT EXISTS ${CUSTOM_OPS_PATH})
        message(FATAL_ERROR "Custom ops path does not exist: ${CUSTOM_OPS_PATH}")
    endif()
    if(NOT EXISTS "${CUSTOM_OPS_PATH}/CMakeLists.txt")
        message(FATAL_ERROR "CMakeLists.txt not found. Please ensure it's a CMake project: ${CUSTOM_OPS_PATH}")
    endif()

    message(STATUS "Custom ops name: ${CUSTOM_OPS_NAME}")
    message(STATUS "Custom ops vendor: ${CUSTOM_OPS_VENDOR}")
    message(STATUS "Custom ops path: ${CUSTOM_OPS_PATH}")

    set(BUILD_DEVICE_DIR ${CMAKE_SOURCE_DIR}/build_device)
    set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
    set(CUSTOM_OPS_OPP_PATH "opp/vendors/${CUSTOM_OPS_VENDOR}")
    set(CUSTOM_OPS_OPP_INC_PATH "${CUSTOM_OPS_OPP_PATH}/include")
    set(CUSTOM_OPS_OPP_LIB_PATH "${CUSTOM_OPS_OPP_PATH}/lib64")
    set(CUSTOM_OPS_OPP_SCRIPTS_PATH "${CUSTOM_OPS_OPP_PATH}/scripts")
    set(CUSTOM_OPS_OPP_AICPU_CONFIG_PATH "${CUSTOM_OPS_OPP_PATH}/aicpu/config")
    set(CUSTOM_OPS_OPP_AICPU_KERNEL_PATH "${CUSTOM_OPS_OPP_PATH}/aicpu/kernel")

    include(${HCCL_CC_DIR}/cmake/config.cmake)
    include(${HCCL_CC_DIR}/cmake/func.cmake)

    # AICPU Device 构建目标
    add_custom_target(custom_aicpu)
    # CPack 打包
    pack_custom()
    # 添加自定义算子工程
    add_subdirectory(${CUSTOM_OPS_PATH})

else()
    include(${HCCL_CC_DIR}/cmake/config.cmake)
    include(${HCCL_CC_DIR}/cmake/func.cmake)
    add_subdirectory(src)

    if(DEVICE_MODE AND KERNEL_MODE)
        cann_pack_targets_and_files(
            OUTPUT_TARGET "generate_device_aicpu_package"
            OUTPUT "mc2_server.tar.gz"
            TARGETS mc2_server mc2_kernel_compat
            MANIFEST "bin_hash.cfg"
            TAR_ROOT_DIR "aicpu_kernels_device"
        )
        add_cann_sign_file(
            INPUT "mc2_server.tar.gz"
            CONFIG "${HCCL_CC_DIR}/sign.xml"
            RESULT_VAR "mc2_server_sign_file"
            VERSION "${CANN_VERSION_asc-devkit_VERSION}"
            DEPENDS generate_device_aicpu_package
        )
        install(FILES ${mc2_server_sign_file}
            DESTINATION opp/built-in/op_impl/aicpu/kernel
            COMPONENT asc-devkit
        )
    endif()

endif()
