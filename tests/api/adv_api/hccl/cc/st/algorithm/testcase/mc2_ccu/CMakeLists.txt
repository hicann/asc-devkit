# --------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# --------------------------------------------------------------------------------------------

# ==========================================================================================================
# Target : selector / CalcRes / channel full-chain ST
# Compiles real selector / executor / template + kernel, verifies full chain
# ==========================================================================================================
set(HCCL_CCU_ST_TARGET ascendc_ut_adv_api_ccu_st)
set(HCCL_MC2_CCU_ALL_TO_ALL_V_DIR ${CMAKE_CURRENT_SOURCE_DIR}/all_to_all_v)
set(HCCL_MC2_CCU_ALL_TO_ALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/all_to_all)

set(HCCL_SELECTOR_PRODUCT_SOURCES
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/selector/alltoall_auto_selector.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/selector/alltoallv_auto_selector.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/executor/ins_v2_all_to_all_v_sole_executor.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/aicpu/ins_temp_ubx_all_to_all_v_mesh_1D.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu/ccu_temp_all_to_all_mesh_1D.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu/ccu_temp_all_to_all_v_mesh_1D.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_mesh1d.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_v_mesh1d.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/op_common.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector/auto_selector_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector/execute_selector.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector/selector_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/executor_v2_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/registry/coll_alg_v2_exec_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/registry/coll_alg_exec_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu_alg_template_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/alg_v2_template_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/alg_template_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/template_utils.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu/ccu_kernel_alg_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu/ccu_kernel_utils.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/channel/channel.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/channel/channel_request.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_host.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_1d.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_multilevel.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_ubx.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_ubx_1d.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_pcie_mix.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/physical_level_build.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/physical_level_normalize.cc
    ${HCCL_CC_SRC_DIR}/common/hccl_alloc_ctx_res.cc
    ${HCCL_CC_SRC_DIR}/common/hccl_mc2.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/ccu_log.cc
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym/ccu_res_dl.cc
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym/hccl_ccu_res_dl.cc
)

add_executable(
    ${HCCL_CCU_ST_TARGET}
    ${ASCENDC_DIR}/tests/main.cpp
    ${HCCL_MC2_CCU_ALL_TO_ALL_V_DIR}/all_to_all_v_testcase.cpp
    ${HCCL_MC2_CCU_ALL_TO_ALL_V_DIR}/ccu_all_to_all_v_mesh1d_adapter.cc
    ${HCCL_MC2_CCU_ALL_TO_ALL_V_DIR}/ccu_all_to_all_v.cc
    ${HCCL_MC2_CCU_ALL_TO_ALL_DIR}/all_to_all_testcase.cpp
    ${HCCL_MC2_CCU_ALL_TO_ALL_DIR}/ccu_all_to_all_mesh1d_adapter.cc
    ${HCCL_MC2_CCU_ALL_TO_ALL_DIR}/ccu_all_to_all.cc
    ${UTILS_DIR}/ccu_scenario/ccu_fixture.cc
    ${HCCL_SELECTOR_PRODUCT_SOURCES}
)

target_include_directories(
    ${HCCL_CCU_ST_TARGET}
    PRIVATE
    ${UTILS_DIR}/hccl_proxy
    ${HCCL_MC2_CCU_ALL_TO_ALL_V_DIR}
    ${HCCL_MC2_CCU_ALL_TO_ALL_DIR}
    ${HCCL_CHECKER_COMMON_INCLUDE_LIST}
    ${UTILS_DIR}/ccu_scenario
    ${HCCL_CC_SRC_DIR}
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/selector
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/executor
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu/kernel
    ${HCCL_CC_SRC_DIR}/ops/kfc_server/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/kfc_server/template/ccu/kernel
    ${HCCL_CC_INCLUDE_DIR}/hcomm/types
    ${HCCL_CC_INCLUDE_DIR}/hcomm/ccu
    ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc/hcomm/ccu
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/profiling
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/toolchain
)

target_compile_options(
    ${HCCL_CCU_ST_TARGET}
    PRIVATE
    -std=c++14
    -O0
    -g
    -fno-inline
    -fprofile-update=atomic
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-attributes
    -Wno-unknown-pragmas
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${HCCL_CCU_ST_TARGET} PRIVATE -DUT_TEST -DMC2_CLIENT_ENABLE_CCU=1)

target_link_libraries(
    ${HCCL_CCU_ST_TARGET}
    PRIVATE
    -Wl,--whole-archive
    hccl-model-proxy
    -Wl,--no-whole-archive
    ${HCCL_CHECKER_KERNEL_TARGET}
    hccl-verifier
    intf_llt_pub_basic
    c_sec
    pthread
    dl
)
