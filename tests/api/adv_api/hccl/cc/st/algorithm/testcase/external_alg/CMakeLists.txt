# --------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# --------------------------------------------------------------------------------------------

# ==========================================================================================================
# Target : HCCL 外部算法名映射专属 UT
# 与 AIC 目标 (-DAICPU_COMPILE) 不同，本目标无 AICPU 守卫且 -DMC2_CLIENT_ENABLE_CCU=1，
# 全量编译所有注册现场（15 个 executor 文件 / 全部 selector / template），46 行 sidecar 全部注册，
# 承载 external_alg 全部 UT（含 V2 注册表与 AlgMetaRegistry 的满射断言与 46 行精确锚）。
# ==========================================================================================================
set(HCCL_EXTERNAL_ALG_TARGET ascendc_ut_hccl_cc_external_alg)
set(HCCL_CC_SRC_DIR ${ASCENDC_DIR}/impl/adv_api/detail/hccl/cc/src)
set(HCCL_CC_INCLUDE_DIR ${ASCENDC_DIR}/include/adv_api/hccl/internal)
set(EXT_ALG_UTILS_AICPU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/aicpu)
# hccl_proxy 提供 ST 专用的 ccu_primitives_dl.hpp / ccu_func_dl.hpp 等 stub 头，
# 置于 include 列表最前（与 mc2_ccu 一致）以优先于 common/hcomm_dlsym 的真实现
set(EXT_ALG_UTILS_HCCL_PROXY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/hccl_proxy)
# stub 源依赖的仿真世界 / 通信器头
set(EXT_ALG_UTILS_SIM_WORLD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/sim_world)
set(EXT_ALG_UTILS_COMMUNICATOR_DIR ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator)
set(EXT_ALG_UTILS_TOPO_MODEL_DIR ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/topo_model)

# ---- 增长面：注册现场全量收集（未来新增算法文件自动纳入满射断言守护，免人肉同步）----
# executor/selector 只收 4 个集合通信算子目录；不收 kfc_server——若编入将注册（本目标
# AICPU_COMPILE / HCCL_CANN_COMPAT_850 均未定义，ins_kfc_server_sole_executor.cc 的注册行
# 生效），超出 46 行 sidecar 范围，故 GLOB 不收；interface_graph_mode 无 executor/selector/
# template，天然不收。
# 局限：未来新增 ops/<new_op>/executor 目录不会被本 GLOB 收集，且 46 锚对新 cmd 静默——
# 新增 op 时需同步本清单与测试侧 COLL_CMDS。
file(GLOB EXT_ALG_EXECUTORS CONFIGURE_DEPENDS
    ${HCCL_CC_SRC_DIR}/ops/all_gather/executor/*.cc
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/executor/*.cc
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/executor/*.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/executor/*.cc)

file(GLOB EXT_ALG_SELECTORS CONFIGURE_DEPENDS
    ${HCCL_CC_SRC_DIR}/ops/all_gather/selector/*.cc
    ${HCCL_CC_SRC_DIR}/ops/reduce_scatter/selector/*.cc
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/selector/*.cc
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/selector/*.cc)

file(GLOB EXT_ALG_TEMPLATES CONFIGURE_DEPENDS
    ${HCCL_CC_SRC_DIR}/ops/*/template/aicpu/*.cc
    ${HCCL_CC_SRC_DIR}/ops/*/template/ccu/*.cc
    ${HCCL_CC_SRC_DIR}/ops/*/template/ccu/kernel/*.cc)
# kfc_server 模板经 ops/*/template/ccu 通配进入，剔除：若编入将注册（本目标
# AICPU_COMPILE / HCCL_CANN_COMPAT_850 均未定义，kfc_server 注册行生效），超出
# 46 行 sidecar 范围，故 GLOB 不收
list(FILTER EXT_ALG_TEMPLATES EXCLUDE REGEX "kfc_server")
# 与产品构建对齐：all_gather / reduce_scatter 的 Legacy direct-launch ccu 模板产品已停编
# （对应各 template/ccu/CMakeLists.txt 的注释行，原由：引用已废弃的旧 hcomm::CcuKernelArg
# API，当前头文件体系下不可编）。46 行 sidecar 的注册行均不引用它们（CCU 注册只引用
# KFC Mem2Mem 系列与 all_to_all_v direct-launch 系列，后者用现行 CcuKernelArgBase API）。
# all_reduce / all_to_all_v 的 ccu 模板产品全编，无需排除。
set(EXT_ALG_EXCLUDED_TEMPLATES
    # 产品 mc2_client（host，SHARED）编入 load_kernel.cc / mc2_aicpu_task_cache_clear.cc，
    # 但二者依赖 UT 环境无法满足的 aclrtCreateStreamWithConfig / LoadBinaryFromFile
    # （adapter_acl.cc，AIC kernel 目标同样注释停编），且其符号无人引用——
    # g_binKernelHandle 由 hccl_algorithm_stub.cc 提供 stub 定义（与 AIC 目标做法一致）
    op_common/template/aicpu/load_kernel.cc
    op_common/template/aicpu/mc2_aicpu_task_cache_clear.cc
    all_gather/template/ccu/ccu_temp_all_gather_mesh_1D_mem2mem.cc
    all_gather/template/ccu/ccu_temp_all_gather_mesh_1D.cc
    all_gather/template/ccu/ccu_temp_all_gather_nhr_1D_mem2mem.cc
    all_gather/template/ccu/ccu_temp_all_gather_2dies_mesh_1d_mem2mem.cc
    all_gather/template/ccu/ccu_temp_all_gather_2dies_mesh_1D.cc
    all_gather/template/ccu/ccu_temp_all_gather_nhr_1D_multi_jetty_mem2mem.cc
    all_gather/template/ccu/kernel/ccu_kernel_all_gather_mesh1d_mem2mem.cc
    all_gather/template/ccu/kernel/ccu_kernel_all_gather_mesh1d.cc
    all_gather/template/ccu/kernel/ccu_kernel_kfc_all_gather_mesh1d.cc
    all_gather/template/ccu/kernel/ccu_kernel_all_gather_nhr1d_mem2mem.cc
    all_gather/template/ccu/kernel/ccu_kernel_all_gather_2dies_mesh1d_mem2mem.cc
    all_gather/template/ccu/kernel/ccu_kernel_all_gather_nhr1d_multi_jetty_mem2mem.cc
    all_gather/template/ccu/kernel/ccu_kernel_all_gather_2dies_mesh_1D.cc
    reduce_scatter/template/ccu/ccu_temp_reduce_scatter_mesh_1D_mem2mem.cc
    reduce_scatter/template/ccu/ccu_temp_reduce_scatter_mesh_1D.cc
    reduce_scatter/template/ccu/ccu_temp_reduce_scatter_nhr_1D_mem2mem.cc
    reduce_scatter/template/ccu/ccu_temp_reduce_scatter_mesh_1D_2die_mem2mem.cc
    reduce_scatter/template/ccu/ccu_temp_reduce_scatter_nhr_1D_multi_jetty_mem2mem.cc
    reduce_scatter/template/ccu/ccu_temp_reduce_scatter_mesh2die.cc
    reduce_scatter/template/ccu/kernel/ccu_kernel_reduce_scatter_mesh1d_mem2mem.cc
    reduce_scatter/template/ccu/kernel/ccu_kernel_reduce_scatter_mesh1d.cc
    reduce_scatter/template/ccu/kernel/ccu_kernel_reduce_scatter_nhr1d_mem2mem.cc
    reduce_scatter/template/ccu/kernel/ccu_kernel_reduce_scatter_mesh1d_2die_mem2mem.cc
    reduce_scatter/template/ccu/kernel/ccu_kernel_reduce_scatter_nhr1d_multi_jetty_mem2mem.cc
    reduce_scatter/template/ccu/kernel/ccu_kernel_reduce_scatter_mesh2die.cc
)
# 逐项按全路径精确移除（REGEX 片段把文件名当正则用有元字符隐患）
foreach(_excluded IN LISTS EXT_ALG_EXCLUDED_TEMPLATES)
    list(REMOVE_ITEM EXT_ALG_TEMPLATES ${HCCL_CC_SRC_DIR}/ops/${_excluded})
endforeach()

# ---- 固定面：external_alg / common / op_common / topo / stub ----
# executor 注册项经 vtable 急切实例化引用模板 out-of-line 虚函数符号，模板文件必须编入；
# topo / common / stub 条目抄自 mc2_ccu 目标与 aicpu kernel 库的现成清单。
set(EXT_ALG_FIXED_SOURCES
    # external_alg 模块本体（parser 为头文件实现，无 .cc）
    ${HCCL_CC_SRC_DIR}/ops/op_common/external_alg/alg_meta_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/external_alg/external_alg_resolver.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/external_alg/external_alg_rules.cc
    # common 链（hccl_alloc_ctx_res / hccl_mc2 及其依赖）
    ${HCCL_CC_SRC_DIR}/common/hccl_alloc_ctx_res.cc
    ${HCCL_CC_SRC_DIR}/common/hccl_mc2.cc
    ${HCCL_CC_SRC_DIR}/common/utils.cc
    ${HCCL_CC_SRC_DIR}/common/config_log.cc
    ${HCCL_CC_SRC_DIR}/common/sal.cc
    ${HCCL_CC_SRC_DIR}/common/log.cc
    ${HCCL_CC_SRC_DIR}/common/adapter_error_manager_pub.cc
    ${HCCL_CC_SRC_DIR}/common/alg_env_config.cc
    ${HCCL_CC_SRC_DIR}/common/param_check.cc
    ${HCCL_CC_SRC_DIR}/common/alg_type.cc
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym/hcomm_primitives_dl.cc
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym/ccu_res_dl.cc
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym/hccl_ccu_res_dl.cc
    # op_common
    ${HCCL_CC_SRC_DIR}/ops/op_common/op_common.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/cann_host_bridge.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/ccu_log.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/omnipipe_data_slice_calc.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/channel/channel.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/channel/channel_request.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/registry/coll_alg_exec_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/registry/coll_alg_v2_exec_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/executor_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/executor_v2_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector/auto_selector_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector/selector_registry.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector/execute_selector.cc
    # template 基类与注册表（GLOB 只收 template/aicpu、template/ccu 子目录，
    # 不覆盖 op_common/template 直下与 wrapper / registry / dfx / task_cache 文件）
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/alg_template_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/alg_v2_template_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu_alg_template_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/template_utils.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/registry/alg_template_register.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/registry/alg_v2_template_register.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/wrapper/alg_data_trans_wrapper.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/wrapper/dpu_alg_data_trans_wrapper.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/aicpu/dfx/task_exception_fun.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/aicpu/task_cache/mc2_aicpu_task_cache.cc
    # topo
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_host.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_base.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_1d.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_multilevel.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_ubx.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_ubx_1d.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_pcie_mix.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/topo_match_concurrent_v2.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/physical_level_build.cc
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo/physical_level_normalize.cc
    # stub（host 侧替代设备 / 外部库依赖）。channel_stub.cc / topo_match_aicpu_stub.cc
    # 是 AIC kernel 库（不编 channel.cc / topo_match_ubx.cc）专用的替身，本目标直接编入
    # 产品真实现，编 stub 反而重复定义，故不编
    ${EXT_ALG_UTILS_AICPU_DIR}/dlhcomm_function_stub.cc
    # hccl_proxy stub 源（aclrt / hccl / hcclinner / ccu 原语 / log / mmpa / error_manager
    # 等 host 侧替代实现）及其依赖的仿真世界实现（communicator / topo_model，
    # 由 stub 源符号传递引用）
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/aclrt_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/ccu_primitives_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/hccl_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/hccl_algorithm_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/hcclinner_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/log_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/mmpa_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/error_manager_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/hcomm_ccu_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/hccl_profiling_dfx_stub.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/ccu_kernel_proxy.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_communicator.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_thread.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_thread_manager.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_channel.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_channel_manager.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_channel_exchange_handler.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/communicator/sim_context_manager.cc
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}/topo_model/topo_model.cc
    # 仿真世界（stub 传递依赖：SimNpu / SimTaskQueue / CcuSt::RegisterManager / HcclException）
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}/ccu_sim_world.cc
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}/comm_domain.cc
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}/sim_npu.cc
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}/sim_task.cc
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}/sim_task_queue.cc
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}/sim_world.cc
    # 异常与工具（HcclException / checker 工具，stub 传递依赖）
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/common/exception/exception_defination.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/common/exception/hccl_exception.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/common/utils/check_utils.cc
)

add_executable(${HCCL_EXTERNAL_ALG_TARGET}
    ../main.cc
    external_alg_testcase.cc
    external_alg_full_testcase.cpp
    ${EXT_ALG_EXECUTORS}
    ${EXT_ALG_SELECTORS}
    ${EXT_ALG_TEMPLATES}
    ${EXT_ALG_FIXED_SOURCES}
)

target_include_directories(${HCCL_EXTERNAL_ALG_TARGET} PRIVATE
    ${EXT_ALG_UTILS_HCCL_PROXY_DIR}
    ${EXT_ALG_UTILS_SIM_WORLD_DIR}
    ${EXT_ALG_UTILS_COMMUNICATOR_DIR}
    ${EXT_ALG_UTILS_TOPO_MODEL_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/hccl_verifier
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/common/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/src/common/exception
    ${UT_COMMON_INCLUDE_LIST}
    ${CANN_3RD_LIB_PATH}/gtest/include
    ${ASCENDC_DIR}
    ${HCCL_CC_INCLUDE_DIR}
    ${HCCL_CC_INCLUDE_DIR}/hcomm
    ${HCCL_CC_INCLUDE_DIR}/hcomm/types
    ${HCCL_CC_INCLUDE_DIR}/hcomm/hccl
    ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc
    ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc/hccl
    ${HCCL_CC_INCLUDE_DIR}/hcomm/ccu
    ${HCCL_CC_INCLUDE_DIR}/hcomm/pkg_inc/hcomm/ccu
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCEND_CANN_PACKAGE_PATH}/include/acl
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/aicpu
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/mmpa
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/runtime/runtime
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/profiling
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/toolchain
    ${HCCL_CC_SRC_DIR}
    ${HCCL_CC_SRC_DIR}/common
    ${HCCL_CC_SRC_DIR}/common/hcomm_dlsym
    ${HCCL_CC_SRC_DIR}/ops
    ${HCCL_CC_SRC_DIR}/ops/op_common
    ${HCCL_CC_SRC_DIR}/ops/op_common/inc
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/channel
    ${HCCL_CC_SRC_DIR}/ops/op_common/executor/registry
    ${HCCL_CC_SRC_DIR}/ops/op_common/external_alg
    ${HCCL_CC_SRC_DIR}/ops/op_common/selector
    ${HCCL_CC_SRC_DIR}/ops/op_common/template
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/registry
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/aicpu
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/wrapper
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/aiv
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/op_common/template/ccu/kernel
    ${HCCL_CC_SRC_DIR}/ops/op_common/topo
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
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/all_to_all_v/template/ccu/kernel
    ${HCCL_CC_SRC_DIR}/ops/all_reduce
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/executor
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/selector
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/template
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/template/aicpu
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/template/ccu
    ${HCCL_CC_SRC_DIR}/ops/all_reduce/template/ccu/kernel
)

target_compile_options(${HCCL_EXTERNAL_ALG_TARGET} PRIVATE
    -std=c++17
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

target_compile_definitions(${HCCL_EXTERNAL_ALG_TARGET} PRIVATE
    -DUT_TEST
    -DMC2_CLIENT_ENABLE_CCU=1
)

target_link_libraries(${HCCL_EXTERNAL_ALG_TARGET} PRIVATE
    intf_llt_pub_basic
    c_sec
    pthread
    dl
)

run_llt_test(
    TARGET ${HCCL_EXTERNAL_ALG_TARGET}
    TASK_NUM 1
)
