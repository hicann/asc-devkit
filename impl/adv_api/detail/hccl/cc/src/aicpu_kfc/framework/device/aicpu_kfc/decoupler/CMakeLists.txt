# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
set(src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/comm_kfc_aicpu_server.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/comm_kfc_dispatcher.cc
)

set(OPEN_HCCL_SRC_ROOT
    ${HCCL_CC_DIR}/src
)

set(open_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/comm_kfc_open_kernel_adapter.cc
)

set(open_exec_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/comm_kfc_open_kernel_executor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/cann_dispatch_bridge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/open_res_ctx_cache.cc
)

add_library(ccl_kernel_decoupler_open OBJECT ${open_src_list})
set_target_properties(ccl_kernel_decoupler_open PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(ccl_kernel_decoupler_open PRIVATE
    $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(ccl_kernel_decoupler_open PRIVATE
    $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(ccl_kernel_decoupler_open BEFORE PRIVATE
    ${HCCL_HCOMM_HEADER_DIR}/../
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/inc
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/inc
    ${OPEN_HCCL_SRC_ROOT}/common
    ${OPEN_HCCL_SRC_ROOT}/common/hcomm_dlsym
    ${HCCL_HCOMM_HEADER_DIR}/ccu
    ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu
    ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu_new

    ${MC2_SERVER_CCL_INCLUDE_LIST}
    ${MC2_SERVER_ORION_HEAD_LIST}
)

target_link_libraries(ccl_kernel_decoupler_open PRIVATE
    $<BUILD_INTERFACE:acl_rt_headers>
    $<BUILD_INTERFACE:runtime_headers>
    c_sec
    unified_dlog
)

add_library(ccl_kernel_decoupler_open_exec OBJECT ${open_exec_src_list})
set_target_properties(ccl_kernel_decoupler_open_exec PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(ccl_kernel_decoupler_open_exec PRIVATE
    $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_DEFINITIONS>
    AICPU_COMPILE
)

target_compile_options(ccl_kernel_decoupler_open_exec PRIVATE
    $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(ccl_kernel_decoupler_open_exec BEFORE PRIVATE
    ${HCCL_LOCAL_HEADER_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${HCCL_HCOMM_HEADER_DIR}/../
    ${HCCL_HCOMM_HEADER_DIR}
    ${HCCL_HCOMM_HEADER_DIR}/hccl
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/inc
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/topo
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/executor
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/executor/channel
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/executor/registry
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/template
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/template/registry
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/template/wrapper
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/inc
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/topo
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/executor
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/executor/channel
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/executor/registry
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/template
    ${OPEN_HCCL_SRC_ROOT}/ops/op_common/template/registry
    ${OPEN_HCCL_SRC_ROOT}/common
    ${OPEN_HCCL_SRC_ROOT}/common/hcomm_dlsym

    ${HCCL_BASE_DIR}/pub_inc
    ${HCCL_HCOMM_HEADER_DIR}/pkg_inc
    ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hccl
    ${HCCL_HCOMM_HEADER_DIR}/ccu
    ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu
    ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu_new
)

target_link_libraries(ccl_kernel_decoupler_open_exec PRIVATE
    c_sec
    unified_dlog
    acl_rt_headers
    runtime_headers
)

if(TARGET mc2_server)
    add_library(mc2_server_decoupler OBJECT ${src_list})
    set_target_properties(mc2_server_decoupler PROPERTIES POSITION_INDEPENDENT_CODE ON)

    target_compile_definitions(mc2_server_decoupler PRIVATE
        $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_DEFINITIONS>
        MC2_SERVER_ONLY
    )

    target_compile_options(mc2_server_decoupler PRIVATE
        $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_OPTIONS>
    )

    target_include_directories(mc2_server_decoupler BEFORE PRIVATE
        ${HCCL_BASE_DIR}/pub_inc
        ${HCCL_HCOMM_HEADER_DIR}/../
        ${OPEN_HCCL_SRC_ROOT}/ops/op_common/inc
        ${OPEN_HCCL_SRC_ROOT}/common/hcomm_dlsym
        ${OPEN_HCCL_SRC_ROOT}/common/hcomm_dlsym/ccu
        ${HCCL_HCOMM_HEADER_DIR}/ccu
        ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu
        ${HCCL_HCOMM_HEADER_DIR}/pkg_inc/hcomm/ccu_new
        ${HCCL_BASE_DIR}/legacy/unified_platform/resource/stream/aicpu

        ${MC2_SERVER_CCL_INCLUDE_LIST}
        ${MC2_SERVER_ORION_HEAD_LIST}
    )

    target_link_libraries(mc2_server_decoupler PRIVATE
        $<BUILD_INTERFACE:acl_rt_headers>
        $<BUILD_INTERFACE:ascend_hal_headers>
        $<BUILD_INTERFACE:metadef_headers>
        $<BUILD_INTERFACE:runtime_headers>
        c_sec
        unified_dlog
    )

    target_sources(mc2_server PRIVATE
        $<TARGET_OBJECTS:mc2_server_decoupler>
        $<TARGET_OBJECTS:ccl_kernel_decoupler_open>
        $<TARGET_OBJECTS:ccl_kernel_decoupler_open_exec>
    )
endif()
