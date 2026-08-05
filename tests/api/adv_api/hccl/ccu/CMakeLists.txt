# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except be in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

set(CCU_UT_TARGET ascendc_ut_adv_api_hccl_ccu_mc2)

set(HCCL_CC_DIR ${ASCENDC_DIR}/impl/adv_api/detail/hccl/cc)
set(HCCL_CC_SRC_DIR ${HCCL_CC_DIR}/src)
set(HCCL_CC_INCLUDE_DIR ${ASCENDC_DIR}/include/adv_api/hccl/internal)
set(HCCL_LOCAL_PKG_INC ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc)
set(HCCL_ST_PROXY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stub/hccl_proxy)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(HCCL_CANN_ARCH_DIR ${ASCEND_CANN_PACKAGE_PATH}/x86_64-linux)
else()
    set(HCCL_CANN_ARCH_DIR ${ASCEND_CANN_PACKAGE_PATH}/aarch64-linux)
endif()

set(CCU_INCLUDE_LIST
    ${UT_COMMON_INCLUDE_LIST}
    ${CANN_3RD_LIB_PATH}/gtest/include
    ${ASCENDC_DIR}
    ${HCCL_CC_INCLUDE_DIR}
    ${HCCL_CC_INCLUDE_DIR}/hcomm
    ${HCCL_CC_INCLUDE_DIR}/hcomm/hccl
    ${HCCL_LOCAL_PKG_INC}
    ${HCCL_LOCAL_PKG_INC}/hccl
    ${HCCL_CC_INCLUDE_DIR}/hcomm/ccu
    ${HCCL_CC_INCLUDE_DIR}/hcomm/types
    ${HCCL_LOCAL_PKG_INC}/hcomm/ccu
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCEND_CANN_PACKAGE_PATH}/include/acl
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/aicpu
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/mmpa
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/profiling
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/toolchain
    ${HCCL_CANN_ARCH_DIR}/pkg_inc/runtime
    ${HCCL_CANN_ARCH_DIR}/pkg_inc/runtime/runtime
    # Common source paths - must include subdirs for relative #include paths
    ${HCCL_CC_SRC_DIR}
    ${HCCL_CC_SRC_DIR}/common
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym
    ${HCCL_CC_SRC_DIR}/ops
    ${HCCL_CC_SRC_DIR}/ops/op_common
    ${HCCL_CC_SRC_DIR}/ops/op_common/inc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/channel
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/registry
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector
    ${HCCL_CC_SRC_DIR}/ops/op_common/template
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/registry
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/aicpu
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/wrapper
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/aiv
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu/kernel
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo
    ${HCCL_CC_SRC_DIR}/ops/all_gather
    ${HCCL_CC_SRC_DIR}/ops/all_gather/selector
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/all_reduce
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/selector
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/template
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/selector
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template/ccu
    ${HCCL_CC_SRC_DIR}/aicpu_kfc/legacy/common/types
    ${HCCL_CC_SRC_DIR}/aicpu_kfc/legacy/common/utils

    ${HCCL_ST_PROXY_DIR}
    ${HCCL_ST_PROXY_DIR}/communicator
    ${HCCL_ST_PROXY_DIR}/topo_model
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/sim_world
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/common/utils
    ${HCCL_CC_SRC_DIR}/aicpu_kfc/legacy/common/exception
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${CCU_UT_TARGET}
    ${ASCENDC_DIR}/tests/main.cpp
    test_ccu_mc2.cpp
    # CCU kernel stub and necessary external dependency stubs from existing st framework
    # All stub files from st framework
    ${HCCL_ST_PROXY_DIR}/aclrt_stub.cc
    ${HCCL_ST_PROXY_DIR}/hccl_algorithm_stub.cc
    ${HCCL_ST_PROXY_DIR}/hccl_stub.cc
    ${HCCL_ST_PROXY_DIR}/hcclinner_stub.cc
    ${HCCL_ST_PROXY_DIR}/log_stub.cc
    ${HCCL_ST_PROXY_DIR}/mmpa_stub.cc
    ${HCCL_ST_PROXY_DIR}/error_manager_stub.cc
    ${HCCL_ST_PROXY_DIR}/hcomm_ccu_stub.cc
    ${HCCL_ST_PROXY_DIR}/hccl_profiling_dfx_stub.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_communicator.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_thread_manager.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_thread.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_channel.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_channel_manager.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_channel_exchange_handler.cc
    ${HCCL_ST_PROXY_DIR}/communicator/sim_context_manager.cc
    ${HCCL_ST_PROXY_DIR}/topo_model/topo_model.cc
    ${HCCL_ST_PROXY_DIR}/hccl_op_common_stub.cc
    # Sim world stub files
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/sim_world/sim_world.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/sim_world/sim_npu.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/sim_world/sim_task.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/sim_world/sim_task_queue.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/sim_world/comm_domain.cc
    # Cann host bridge stub
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/cann_host_bridge_stub.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/exception_defination_stub.cc
    # Source under test (minimal impl dependencies)
    ${HCCL_CC_SRC_DIR}/common/hccl_mc2.cc
    ${HCCL_CC_SRC_DIR}/common/hccl_alloc_ctx_res.cc
)

target_include_directories(${CCU_UT_TARGET} PRIVATE
    ${CCU_INCLUDE_LIST}
)

target_compile_options(${CCU_UT_TARGET} PRIVATE
    -std=c++17
    -O0
    -g
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-attributes
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${CCU_UT_TARGET} PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)

target_link_libraries(${CCU_UT_TARGET} PRIVATE
    intf_llt_pub_basic
    c_sec
    pthread
    dl
)

run_llt_test(
    TARGET ${CCU_UT_TARGET}
    TASK_NUM 1
)
