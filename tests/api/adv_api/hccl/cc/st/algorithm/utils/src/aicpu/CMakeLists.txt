# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

set(INCLUDE_LIST
    ${HCCL_CC_INCLUDE_DIR}
    ${HCCL_DEV_BASE}/src/common
    ${HCCL_DEV_BASE}/src/common/hcomm_dlsym

    ${HCCL_DEV_BASE}/src/ops
    ${HCCL_DEV_BASE}/src/ops/op_common
    ${HCCL_DEV_BASE}/src/ops/op_common/inc

    ${HCCL_DEV_BASE}/src/ops/op_common/executor
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/channel
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/registry

    ${HCCL_DEV_BASE}/src/ops/op_common/selector

    ${HCCL_DEV_BASE}/src/ops/op_common/template
    ${HCCL_DEV_BASE}/src/ops/op_common/template/registry
    ${HCCL_DEV_BASE}/src/ops/op_common/template/aicpu
    ${HCCL_DEV_BASE}/src/ops/op_common/template/wrapper
    ${HCCL_DEV_BASE}/src/ops/op_common/template/aiv
    ${HCCL_DEV_BASE}/src/ops/op_common/template/ccu
    ${HCCL_DEV_BASE}/src/ops/op_common/template/ccu/kernel

    ${HCCL_DEV_BASE}/src/ops/op_common/topo

    ${HCCL_DEV_BASE}/src/ops/reduce_scatter
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/selector
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aiv
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/ccu
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/ccu/kernel

    ${HCCL_DEV_BASE}/src/ops/all_gather
    ${HCCL_DEV_BASE}/src/ops/all_gather/executor
    ${HCCL_DEV_BASE}/src/ops/all_gather/selector
    ${HCCL_DEV_BASE}/src/ops/all_gather/template
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/ccu
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aiv

    ${HCCL_DEV_BASE}/src/ops/all_to_all_v
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/executor
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/selector
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/template
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/template/aicpu

    ${HCCL_DEV_BASE}/src/common/hcomm_dlsym
)

list(APPEND INCLUDE_LIST
    ${HCCL_CHECKER_COMMON_INCLUDE_LIST}
)

add_library(${HCCL_CHECKER_SELECTOR_TARGET} STATIC
    ${HCCL_DEV_BASE}/src/ops/all_gather/selector/all_gather_auto_selector.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/selector/reduce_scatter_auto_selector.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/selector/alltoall_auto_selector.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/selector/alltoallv_auto_selector.cc
)

target_include_directories(${HCCL_CHECKER_SELECTOR_TARGET} PRIVATE
    ${INCLUDE_LIST}
)

target_compile_options(${HCCL_CHECKER_SELECTOR_TARGET} PRIVATE
    -std=c++17
    -g
    -O0
    -fstack-protector-all
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${HCCL_CHECKER_SELECTOR_TARGET} PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)

add_library(${HCCL_CHECKER_EXECUTOR_TARGET} STATIC
    ${HCCL_DEV_BASE}/src/ops/op_common/topo/topo_match_pcie_mix.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_v2_reduce_scatter_sole_executor.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_reduce_scatter_parallel_executor.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_v2_reduce_scatter_sequence_executor.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_v2_reduce_scatter_sequence_executor_aicpu.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1D.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_nhr.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1D_meshchunk.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1D_Z_axis_detour.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_aicpu_reduce_nhr.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1d_dpu.cc

    ${HCCL_DEV_BASE}/src/ops/all_gather/executor/ins_v2_all_gather_sole_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/executor/ins_v2_all_gather_parallel_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/executor/ins_v2_all_gather_sequence_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu/ins_temp_all_gather_mesh_1D.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu/ins_temp_all_gather_nhr.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu/ins_temp_all_gather_nhr_dpu.cc

    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/executor/ins_v2_all_to_all_v_sole_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/executor/ins_v2_all_to_all_concurrent_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/template/aicpu/ins_temp_all_to_all_v_mesh_1D.cc
)

target_include_directories(${HCCL_CHECKER_EXECUTOR_TARGET} PRIVATE
    ${INCLUDE_LIST}
)

target_compile_options(${HCCL_CHECKER_EXECUTOR_TARGET} PRIVATE
    -std=c++17
    -g
    -O0
    -fstack-protector-all
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${HCCL_CHECKER_EXECUTOR_TARGET} PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)

add_library(${HCCL_CHECKER_KERNEL_TARGET} STATIC
    ${HCCL_DEV_BASE}/src/common/utils.cc
    # ${HCCL_DEV_BASE}/src/common/adapter_acl.cc
    ${HCCL_DEV_BASE}/src/common/config_log.cc
    ${HCCL_DEV_BASE}/src/common/sal.cc
    ${HCCL_DEV_BASE}/src/common/log.cc
    ${HCCL_DEV_BASE}/src/common/adapter_error_manager_pub.cc
    ${HCCL_DEV_BASE}/src/common/alg_env_config.cc
    ${HCCL_DEV_BASE}/src/common/hcomm_dlsym/hcomm_primitives_dl.cc

    channel_stub.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/channel/channel_request.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/registry/coll_alg_exec_registry.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/registry/coll_alg_v2_exec_registry.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/executor_base.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/executor/executor_v2_base.cc

    ${HCCL_DEV_BASE}/src/ops/op_common/template/alg_template_base.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/alg_v2_template_base.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/template_utils.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/aicpu/kernel_launch.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/aicpu/dfx/task_exception_fun.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/aicpu/task_cache/mc2_aicpu_task_cache.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/registry/alg_template_register.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/registry/alg_v2_template_register.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/wrapper/alg_data_trans_wrapper.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/template/wrapper/dpu_alg_data_trans_wrapper.cc

    ${HCCL_DEV_BASE}/src/ops/op_common/topo/topo.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/topo/topo_match_base.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/topo/topo_match_pcie_mix.cc
    topo_match_aicpu_stub.cc

    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_v2_reduce_scatter_sole_executor.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_reduce_scatter_parallel_executor.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_v2_reduce_scatter_sequence_executor.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/executor/ins_v2_reduce_scatter_sequence_executor_aicpu.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1D.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_nhr.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1D_meshchunk.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/template/aicpu/ins_temp_reduce_scatter_mesh_1D_Z_axis_detour.cc

    ${HCCL_DEV_BASE}/src/ops/all_gather/executor/ins_v2_all_gather_sole_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/executor/ins_v2_all_gather_parallel_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu/ins_temp_all_gather_mesh_1D.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu/ins_temp_all_gather_nhr.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/executor/ins_v2_all_gather_sequence_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/template/aicpu/ins_temp_all_gather_nhr_dpu.cc

    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/executor/ins_v2_all_to_all_v_sole_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/executor/ins_v2_all_to_all_concurrent_executor.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/template/aicpu/ins_temp_all_to_all_v_mesh_1D.cc
)

target_sources(${HCCL_CHECKER_KERNEL_TARGET} PRIVATE
    ${HCCL_DEV_BASE}/src/common/param_check.cc
    ${HCCL_DEV_BASE}/src/common/alg_type.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/op_common.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/cann_host_bridge.cc
    dlhcomm_function_stub.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/topo/topo_host.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/selector/auto_selector_base.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/selector/selector_registry.cc
    ${HCCL_DEV_BASE}/src/ops/op_common/selector/execute_selector.cc
    ${HCCL_DEV_BASE}/src/ops/all_gather/selector/all_gather_auto_selector.cc
    ${HCCL_DEV_BASE}/src/ops/reduce_scatter/selector/reduce_scatter_auto_selector.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/selector/alltoall_auto_selector.cc
    ${HCCL_DEV_BASE}/src/ops/all_to_all_v/selector/alltoallv_auto_selector.cc
)

target_include_directories(${HCCL_CHECKER_KERNEL_TARGET} PRIVATE
    ${INCLUDE_LIST}
)

target_compile_options(${HCCL_CHECKER_KERNEL_TARGET} PRIVATE
    -std=c++17
    -g
    -O0
    -fstack-protector-all
    -D_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_definitions(${HCCL_CHECKER_KERNEL_TARGET} PRIVATE
    -DUT_TEST
    -DAICPU_COMPILE
)
