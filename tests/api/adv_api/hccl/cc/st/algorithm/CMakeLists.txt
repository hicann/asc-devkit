# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

set(HCCL_CHECKER_TARGET ascendc_ut_adv_api_hccl_cc_algorithm_ascend950pr_9599_AIC)
set(HCCL_CHECKER_KERNEL_TARGET ascendc_ut_hccl_cc_algorithm_mc2_server)
set(HCCL_CHECKER_MODEL_PROXY_TARGET ascendc_ut_hccl_cc_algorithm_model_proxy)
set(HCCL_CHECKER_VERIFIER_TARGET ascendc_ut_hccl_cc_algorithm_verifier)
set(HCCL_CHECKER_SELECTOR_TARGET ascendc_ut_hccl_cc_algorithm_selectors)
set(HCCL_CHECKER_EXECUTOR_TARGET ascendc_ut_hccl_cc_algorithm_executors)

set(HCCL_CC_DIR ${ASCENDC_DIR}/impl/adv_api/detail/hccl/cc)
set(HCCL_CC_SRC_DIR ${HCCL_CC_DIR}/src)
set(HCCL_CC_INCLUDE_DIR ${ASCENDC_DIR}/include/adv_api/hccl/internal)
set(HCCL_DEV_BASE ${HCCL_CC_DIR})
set(HCCL_CHECKER_LOCAL_PKG_INC ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(HCCL_CHECKER_CANN_ARCH_DIR ${ASCEND_CANN_PACKAGE_PATH}/x86_64-linux)
else()
    set(HCCL_CHECKER_CANN_ARCH_DIR ${ASCEND_CANN_PACKAGE_PATH}/aarch64-linux)
endif()
set(UTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils/src)
set(TESTCASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/testcase)

set(HCCL_CHECKER_COMMON_INCLUDE_LIST
    ${UT_COMMON_INCLUDE_LIST}
    ${CANN_3RD_LIB_PATH}/gtest/include
    ${ASCENDC_DIR}
    ${HCCL_CC_INCLUDE_DIR}
    ${HCCL_CC_INCLUDE_DIR}/hcomm
    ${HCCL_CC_INCLUDE_DIR}/hcomm/types
    ${HCCL_CC_INCLUDE_DIR}/hcomm/hccl
    ${HCCL_CHECKER_LOCAL_PKG_INC}
    ${HCCL_CHECKER_LOCAL_PKG_INC}/hccl
    ${HCCL_CC_INCLUDE_DIR}/hcomm/ccu
    ${HCCL_CHECKER_LOCAL_PKG_INC}/hcomm/ccu
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCEND_CANN_PACKAGE_PATH}/include/acl
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/aicpu
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/mmpa
    ${HCCL_CHECKER_CANN_ARCH_DIR}/pkg_inc/runtime
    ${HCCL_CHECKER_CANN_ARCH_DIR}/pkg_inc/runtime/runtime
    ${HCCL_CC_SRC_DIR}/common
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym
    ${HCCL_CC_SRC_DIR}/aicpu_kfc/pub_inc
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
    ${HCCL_CC_SRC_DIR}/ops/interface_graph_mode
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/executor
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/selector
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template/aicpu
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template/aiv
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/template/ccu/kernel
    ${HCCL_CC_SRC_DIR}/ops/all_gather
    ${HCCL_CC_SRC_DIR}/ops/all_gather/executor
    ${HCCL_CC_SRC_DIR}/ops/all_gather/selector
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template/aicpu
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template/aiv
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/all_gather/template/ccu/kernel
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/executor
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/selector
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/aicpu
    ${UTILS_DIR}/sim_world
    ${UTILS_DIR}/common/utils
    ${UTILS_DIR}/common/exception
    ${UTILS_DIR}/hccl_verifier
    ${UTILS_DIR}/hccl_verifier/semantics_check
    ${UTILS_DIR}/hccl_verifier/task_graph_generator
    ${UTILS_DIR}/hccl_verifier/mem_conflict_check
    ${UTILS_DIR}/hccl_verifier/singletask_check
    ${UTILS_DIR}/hccl_proxy
    ${UTILS_DIR}/hccl_proxy/communicator
    ${UTILS_DIR}/hccl_proxy/topo_model
)

add_subdirectory(utils)
add_subdirectory(testcase)
