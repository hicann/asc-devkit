# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
if(BUILD_OPEN_PROJECT)
    set(INCLUDE_LIST
        ${HCCL_LOCAL_HEADER_DIRS}
        ${HCCL_CC_INCLUDE_DIR}/hcomm
        ${HCCL_CC_INCLUDE_DIR}/hcomm/hccl
        ${HCCL_CC_INCLUDE_DIR}/hcomm/ccu
        ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc/hccl
        ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc/hcomm/ccu
        ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/ccu

        # hcomm头文件
        ${ASCEND_CANN_PACKAGE_PATH}/include/ccu
        ${ASCEND_CANN_PACKAGE_PATH}/include/hcomm
        ${ASCEND_CANN_PACKAGE_PATH}/include/hcomm/ccu
    )
else()
    set(INCLUDE_LIST
        ${TOP_DIR}/inc
        ${TOP_DIR}/inc/aicpu/tsd
        ${TOP_DIR}/inc/driver
        ${TOP_DIR}/inc/external
        ${TOP_DIR}/inc/external/acl
        ${TOP_DIR}/inc/aicpu/
        ${TOP_DIR}/inc/aicpu/aicpu_schedule/aicpu_sharder
        ${TOP_DIR}/abl/qos/qosmng
        ${TOP_DIR}/abl/atrace/inc/utrace
        ${TOP_DIR}/abl/msprof/inc
        ${TOP_DIR}/abl/libc_sec/include
        ${TOP_DIR}/inc/aicpu/common
        ${TOP_DIR}/ace/csruntime/inc/tsch
        ${TOP_DIR}/asl/hss/inc
        ${TOP_DIR}/asl/hss/inc/pub
        ${TOP_DIR}/asl/hss/common/hash_table
        ${TOP_DIR}/metadef
        ${TOP_DIR}/metadef/pkg_inc
        ${TOP_DIR}/metadef/inc
        ${TOP_DIR}/metadef/inc/common
        ${TOP_DIR}/metadef/inc/external
        ${TOP_DIR}/runtime/include/external
        ${TOP_DIR}/runtime/include/external/acl
        ${TOP_DIR}/runtime/pkg_inc/base
        ${TOP_DIR}/runtime/pkg_inc/mmpa

        ${TOP_DIR}/open_source/json/include

        ${TOP_DIR}/hccl/src/common

        ${TOP_DIR}/hcomm/inc
        ${TOP_DIR}/hcomm/inc/hccl
        ${TOP_DIR}/hcomm/pkg_inc
        ${TOP_DIR}/hcomm/pkg_inc/hccl
        ${TOP_DIR}/hcomm/ccu
        ${TOP_DIR}/hcomm/pkg_inc/hcomm/ccu
        ${TOP_DIR}/hcomm/include
        ${TOP_DIR}/hcomm/include/hccl
    )
endif()

if(PRODUCT_SIDE STREQUAL "host")
    add_library(mc2_compat SHARED)

    if(BUILD_OPEN_PROJECT)
        target_compile_definitions(mc2_compat PRIVATE
            OPEN_BUILD_PROJECT
            $<$<STREQUAL:${PRODUCT_SIDE},host>:_GLIBCXX_USE_CXX11_ABI=0>
        )
    else()
        target_compile_definitions(mc2_compat PRIVATE
            $<$<STREQUAL:${PRODUCT_SIDE},host>:_GLIBCXX_USE_CXX11_ABI=0>
        )
    endif()

    target_include_directories(mc2_compat PRIVATE
        ${INCLUDE_LIST}
    )

    target_compile_options(mc2_compat PRIVATE
        -Werror
        -fno-common
        -fno-strict-aliasing
        -pipe
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g>
        -std=c++14
        -fstack-protector-all
    )

    target_link_options(mc2_compat PRIVATE
        -Wl,-z,relro
        -Wl,-z,now
        -Wl,-z,noexecstack
        $<$<CONFIG:Release>:-s>
    )

    if(BUILD_OPEN_PROJECT)
        target_link_libraries(mc2_compat PRIVATE
            $<BUILD_INTERFACE:c_sec_headers>
            $<BUILD_INTERFACE:acl_rt_headers>
            -Wl,--no-as-needed
            acl_rt
            unified_dlog
            -Wl,--no-as-needed
        )
    else()
        target_link_libraries(mc2_compat PRIVATE
            $<BUILD_INTERFACE:slog_headers>
            -Wl,--no-as-needed
            unified_dlog
            -Wl,--no-as-needed
        )
    endif()

    install(TARGETS mc2_compat
        DESTINATION "${CMAKE_SYSTEM_PROCESSOR}-linux/lib64" COMPONENT asc-devkit
    )

    target_sources(mc2_compat PRIVATE
        hccl_rank_graph_dl.cc
        hccl_res_dl.cc
        hcomm_primitives_dl.cc
        hcomm_dlsym.cc
        hccl_inner_dl.cc
        hcomm_host_profiling_dl.cc
        ccu_primitives_impl_dl.cc
        ccu_res_dl.cc
        hccl_ccu_res_dl.cc
        hccl_res_expt_dl.cc
        hccl_host_comm_dl.cc
        ccu_launch_dl.cc
    )
endif()

if((DEVICE_MODE AND BUILD_OPEN_PROJECT) OR (NOT BUILD_OPEN_PROJECT))
    add_library(mc2_kernel_compat SHARED
        hcomm_device_dlsym.cc
        hcomm_primitives_dl.cc
        hcomm_diag_dl.cc
        hcomm_device_profiling_dl.cc
    )

    target_include_directories(mc2_kernel_compat PRIVATE
        ${INCLUDE_LIST}
    )

    target_compile_options(mc2_kernel_compat PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-O3>
        -fstack-protector-all
    )

    target_link_options(mc2_kernel_compat PRIVATE
        -Wl,-z,relro
        -Wl,-z,now
        -Wl,-z,noexecstack
        $<$<CONFIG:Release>:-s>
    )

    target_link_libraries(mc2_kernel_compat PRIVATE
         $<BUILD_INTERFACE:slog_headers>
         $<BUILD_INTERFACE:c_sec_headers>
         $<BUILD_INTERFACE:acl_rt_headers>
     )
endif()
