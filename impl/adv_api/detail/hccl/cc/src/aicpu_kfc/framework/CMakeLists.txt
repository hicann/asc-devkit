# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(HCCL_PACKET_FLAG 1)

if(BUILD_OPEN_PROJECT)
    message(STATUS "open TOP_DIR = ${TOP_DIR}")
    set(HCCL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
    set(HCCL_OPEN_CODE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
    set(HCCL_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../../)
else()
    message(STATUS "close TOP_DIR = ${TOP_DIR}")
    set(HCCL_BASE_DIR ${TOP_DIR}/hcomm/src/)
    set(HCCL_OPEN_CODE_BASE_DIR ${TOP_DIR}/hcomm/src/)
    set(HCCL_THIRD_PARTY_DIR ${TOP_DIR})
    set(INSTALL_CCL_KERNEL_JSON_DIR ${INSTALL_LIBRARY_DIR})
    set(INSTALL_DPU_KERNEL_JSON_DIR ${INSTALL_LIBRARY_DIR})
endif()

message(STATUS "HCCL_OPEN_CODE_BASE_DIR = ${HCCL_OPEN_CODE_BASE_DIR}")
message(STATUS "HCCL_BASE_DIR = ${HCCL_BASE_DIR}")
message(STATUS "framework KERNEL_MODE=${KERNEL_MODE}")
message(STATUS "framework BUILD_OPEN_PROJECT=${BUILD_OPEN_PROJECT}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT BUILD_OPEN_PROJECT OR (BUILD_OPEN_PROJECT AND KERNEL_MODE))
    add_library(asc_ccl_kernel INTERFACE)

    target_compile_options(asc_ccl_kernel INTERFACE
        -Wfloat-equal
        -Wall
        -fno-common
        -fstack-protector-strong
        -fno-strict-aliasing
        -pipe
        -O3
        -std=c++14
    )

    target_link_options(asc_ccl_kernel INTERFACE
        -Wl,-z,relro
        -Wl,-z,now
        -Wl,-z,noexecstack
        -s
    )

    target_compile_definitions(asc_ccl_kernel INTERFACE
        HCCD
        CCL_KERNEL_AICPU
    )

    if(BUILD_OPEN_PROJECT AND KERNEL_MODE)
        target_compile_definitions(asc_ccl_kernel INTERFACE
            OPEN_BUILD_PROJECT
            -D_GLIBCXX_USE_CXX11_ABI=1
        )
    endif()

    set(MC2_SERVER_CCL_INCLUDE_LIST
        ${HCCL_LOCAL_HEADER_DIRS}
        ${HCCL_HCOMM_HEADER_DIR}
        ${HCCL_HCOMM_HEADER_DIR}/hccl
        ${HCCL_HCOMM_HEADER_DIR}/pkg_inc
        ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hccl
        ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu
        ${HCCL_HCOMM_HEADER_DIR}/externel_depends/tsch
        ${HCCL_BASE_DIR}/pub_inc
        ${HCCL_BASE_DIR}/pub_inc/aicpu
        ${HCCL_BASE_DIR}/pub_inc/new
        ${HCCL_CC_DIR}/src/common
        ${HCCL_BASE_DIR}/common/debug/config
        ${HCCL_BASE_DIR}/common/debug/profiling/inc
        ${HCCL_BASE_DIR}/common/stream
        ${HCCL_BASE_DIR}/framework
        ${HCCL_BASE_DIR}/framework/inc
        ${HCCL_BASE_DIR}/framework/common/src
        ${HCCL_BASE_DIR}/framework/common/src/config
        ${HCCL_BASE_DIR}/framework/common/src/exception
        ${HCCL_BASE_DIR}/framework/common/src/task
        ${HCCL_BASE_DIR}/framework/common/src/topo
        ${HCCL_BASE_DIR}/framework/common/src/thread
        ${HCCL_BASE_DIR}/framework/communicator/impl
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/channel
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/channel/device
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/data_api
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/rank_graph
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/resource
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/resource/engine
        ${HCCL_BASE_DIR}/framework/communicator/impl/independent_op/thread
        ${HCCL_BASE_DIR}/framework/device
        ${HCCL_BASE_DIR}/framework/device/aicpu_kfc
        ${HCCL_BASE_DIR}/framework/device/aicpu_kfc/inc
        ${HCCL_BASE_DIR}/framework/device/common
        ${HCCL_BASE_DIR}/framework/device/framework
        ${HCCL_BASE_DIR}/framework/device/inc
        ${HCCL_BASE_DIR}/framework/next/coll_comms/dfx
        ${HCCL_BASE_DIR}/framework/next/coll_comms/dfx/profiling/aicpu
        ${HCCL_BASE_DIR}/framework/next/coll_comms/communicator
        ${HCCL_BASE_DIR}/framework/next/coll_comms/communicator/aicpu
        ${HCCL_BASE_DIR}/framework/next/coll_comms/communicator/ns_recovery/aicpu
        ${HCCL_BASE_DIR}/framework/next/comms/comm_engine_res/threads
        ${HCCL_BASE_DIR}/framework/next/comms/comm_engine_res/threads/slaves
        ${HCCL_BASE_DIR}/framework/next/comms/endpoint_pairs/channels/aicpu
        ${HCCL_BASE_DIR}/framework/next/comms/endpoint_pairs/channels/aicpu/device
        ${ASCEND_CANN_PACKAGE_PATH}/${HCCL_HOST_ARCH_PREFIX}/pkg_inc/base
        ${ASCEND_CANN_PACKAGE_PATH}/${HCCL_HOST_ARCH_PREFIX}/pkg_inc/runtime
        ${ASCEND_CANN_PACKAGE_PATH}/${HCCL_HOST_ARCH_PREFIX}/pkg_inc/profiling
        ${THIRD_PARTY_NLOHMANN_PATH}
    )

    set(MC2_SERVER_ORION_HEAD_LIST
        ${HCCL_BASE_DIR}/legacy/common
        ${HCCL_BASE_DIR}/legacy/common/exception
        ${HCCL_BASE_DIR}/legacy/common/types
        ${HCCL_BASE_DIR}/legacy/common/utils
        ${HCCL_BASE_DIR}/legacy/framework/communicator
        ${HCCL_BASE_DIR}/legacy/framework/communicator/aicpu
        ${HCCL_BASE_DIR}/legacy/framework/dfx/aicpu/common
        ${HCCL_BASE_DIR}/legacy/framework/dfx/aicpu/profiling
        ${HCCL_BASE_DIR}/legacy/framework/dfx/common
        ${HCCL_BASE_DIR}/legacy/service/collective
        ${HCCL_BASE_DIR}/legacy/unified_platform/external_system
        ${HCCL_BASE_DIR}/legacy/unified_platform/pub_inc
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/buffer
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/buffer/aicpu
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/connection/aicpu
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/notify/aicpu
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/stream/aicpu
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/transport/aicpu
    )
    list(REMOVE_DUPLICATES MC2_SERVER_CCL_INCLUDE_LIST)
    list(REMOVE_DUPLICATES MC2_SERVER_ORION_HEAD_LIST)

    target_include_directories(asc_ccl_kernel INTERFACE
        ${MC2_SERVER_CCL_INCLUDE_LIST}
        ${MC2_SERVER_ORION_HEAD_LIST}
    )

    if(TARGET json)
        target_link_libraries(asc_ccl_kernel INTERFACE json)
        add_dependencies(asc_ccl_kernel json)
    endif()

    set(MC2_SERVER_CCL_OBJECT_ALLOWLIST
        mc2_server_aicpu_kfc_common
    )

    function(add_mc2_server_ccl_object target_name)
        if("${target_name}" IN_LIST MC2_SERVER_CCL_OBJECT_ALLOWLIST)
            add_mc2_server_object(${target_name} ${ARGN})
        else()
            add_mc2_server_detached_object(${target_name} ${ARGN})
        endif()
    endfunction()

    configure_mc2_server_minimal_target()
    add_subdirectory(device/aicpu_kfc)
endif()
