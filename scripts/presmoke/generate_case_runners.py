#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from __future__ import annotations

import argparse
import json
import logging
import re
import shlex
from dataclasses import asdict, dataclass, field, replace
from pathlib import Path
from typing import Iterable, List

from presmoke.discover import discover_examples
from presmoke.model import Cell, Command, ExampleSpec
from presmoke.planner import is_cmake_configure
from presmoke.planner import build_cells, rewrite_command
from presmoke.readme_spec import NPU_TOOL_WRAPPERS, parse_readme


LOG = logging.getLogger(__name__)


SPECIAL_PREFIXES = (
    "01_simd_cpp_api/02_features/00_",
    "01_simd_cpp_api/02_features/01_",
    "01_simd_cpp_api/02_features/99_",
)

CUSTOM_OP_PACKAGE_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
PARALLEL_OPS_PACKAGE_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"
TILING_SINK_PROGRAMMING_CASE = "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming"
TILING_SINK_GENERATE_TASK_PATTERN_PREFIX = (
    "GenerateTaskForSinkOp:Node [AddCustomTilingSink, AddCustomTilingSink] starts to generate tasks"
)
TILING_SINK_GENERATE_TASK_PATTERN_SUFFIX = " for the tiling sink, sk_flag [0]."
TILING_SINK_GENERATE_TASK_PATTERN = TILING_SINK_GENERATE_TASK_PATTERN_PREFIX + TILING_SINK_GENERATE_TASK_PATTERN_SUFFIX

CUSTOM_OP_PACKAGE_DEPENDENTS = {
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    TILING_SINK_PROGRAMMING_CASE,
}

# These projects either build host-only apps, superbuild subprojects, or C API
# samples with their own explicit --npu-arch option. Passing
# CMAKE_ASC_ARCHITECTURES to their top-level CMake only produces an unused
# variable warning.
NO_CMAKE_ARCH_INJECTION_CASES = {
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib",
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "02_simd_c_api/00_introduction/00_quickstart/hello_world_npu",
    "02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add",
}

# 不是缺 tensorflow 本身：.devcontainer/requirements.txt 装的是 tensorflow-cpu 2.19.1。
# 缺的是 README「安装TensorFlow插件包」那一步的 Ascend TF 插件（tfplugin），它是独立的
# CANN 包、且锁定 TF 2.6.5，CI 镜像里没有。
TENSORFLOW_SKIP_REASON = (
    "requires the Ascend TensorFlow plugin package (tfplugin, pinned to TF 2.6.5), which is "
    "not installed in CI; skipped by presmoke"
)
MATMUL_L2CACHE_SKIP_REASON = "data size (~308M) causes overtime in cpu mode; cpu mode skipped by presmoke"
LONG_TIME_REASON = "long execution time in cpu mode; cpu mode skipped by presmoke"
MS_SANITIZER_SKIP_REASON = "msSanitizer is a MindStudio tool sample; temporarily skipped by presmoke"
NEGATIVE_SCENARIO_REASON = (
    "troubleshooting sample: this scenario fails by design (see the scenario table in README.md); skipped by presmoke"
)
# 下面三条是「缺陷待修」，和上面那些「按设计/缺环境」不同性质：它们本来应该通过，是在 A5 CI
# 节点上才坏。以前这类用例是用清单里的 # 注释掉的，结果在报告里彻底消失、理由只活在注释里；
# 改成 SKIP_CONFIG 之后每轮都会带着理由出现在 ALL_CASE_TIMINGS.tsv 和 JUnit 里。
A5_MATMUL_28CORE_SKIP_REASON = (
    "wrong results only on the 28-core A5 CI runner (Ascend950PR_9579); passes on 20 and 36 "
    "cores; skipped by presmoke pending a fix from the sample owner"
)
A5_ACCURACY_SKIP_REASON = (
    "accuracy mismatch on A5 only (CompareResult failed / test failed in the run log while the "
    "sample still exits 0); the same cases pass on A2; skipped by presmoke pending a fix from "
    "the sample owner"
)
A5_SIMT_HYBRID_HANG_SKIP_REASON = (
    "hangs silently during teardown on the A5 CI runner after the compute itself succeeded "
    "(no device fault in plog) and blocks later cases sharing the card; skipped by presmoke "
    "pending a fix from the sample owner"
)
A5_SIMT_HYBRID_COLLATERAL_SKIP_REASON = (
    "collateral victim: gets stuck behind the same-card teardown hang of the "
    "simd_simt_stream_compaction / simd_simt_adaptive_max_pool3d_grad cases; passes on its own; "
    "skipped by presmoke until that hang is fixed"
)
# 三个独立的阻塞点：
# 1. README 的命令块是「SAMPLE_DIR=$PWD ... source ... export ... cd x && ... && cd ..」这种
#    要跨步骤保持 shell 状态的序列，而 runner 的每条命令各跑在独立子 shell 里，
#    变量/cd/source 都不会传递下去。
# 2. 要先把隔壁 02_features/99_acl_based/00_acl_compilation/custom_op 工程编出来并安装
#    （cmake --build --target binary package，再跑生成的 custom_opp_*.run）。
# 3. 它**故意触发设备端访问异常**：把 Add 的输出地址换成无效的 0x100000000000。参考
#    simd_simt_ub_usage_limit 场景 12/13/14 的教训，这类故障在设备上要约 289s 才上报，
#    期间同卡的其它用例会一起被堵到 TIMEOUT。
# 注意不是缺 torch：.devcontainer/requirements.txt 装了 torch 2.7.1 + torch_npu，
# 同轮 6 个 torch 用例都是 PASS。
PYTHON_EXCEPTION_DUMP_SKIP_REASON = (
    "needs a stateful shell session (cd/source/export across steps), requires the sibling "
    "custom_op project to be built and installed first, and deliberately injects a device "
    "fault; skipped by presmoke"
)


@dataclass
class SkipConfig:
    reason: str = ""
    modes: List[str] = field(default_factory=list)
    archs: List[str] = field(default_factory=list)


@dataclass
class _SkipContext:
    all_modes_skipped: bool
    skip_reason: str
    custom_op_package_case: bool
    custom_op_dependency: bool


@dataclass
class _SkipResult:
    confidence: str
    reasons: List[str]
    build_cmds: List[Command]
    run_cmds: List[Command]
    verify_cmds: List[Command]


SKIP_CONFIG = {
    # Temporarily exclude these cases from dav-3510 NPU presmoke.
    "01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute__scenario_2": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose__scenario_2": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/05_sort/sort": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/05_sort/topk": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/08_transpose/add_broadcast": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/08_transpose/broadcast": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/08_transpose/transdata": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/08_transpose/transpose": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/batch_matmul_tensor_api": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/copy_in_tensor_api": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/copy_out_tensor_api": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/matmul_bias_fusion": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/matmul_mxfp4_tensor_api_high_performance": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/matmul_tensor_api": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/07_tensor_api/mmad_tensor_api": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/02_features/02_matrix_compute/01_mmad_direction": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/02_features/02_matrix_compute/03_matmul_mx": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/03_c_api/00_data_movement/data_copy_gm2l1": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2gm": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2l1": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2ub": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "02_simd_c_api/03_c_api/00_data_movement/data_copy_ub2l1": SkipConfig(
        reason="temporarily excluded from dav-3510 NPU presmoke", modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/01_utilities/02_dump/experimental/python_exception_dump": SkipConfig(
        reason=PYTHON_EXCEPTION_DUMP_SKIP_REASON, modes=["npu"]
    ),
    "01_simd_cpp_api/01_utilities/05_sanitizer/ms_sanitizer": SkipConfig(
        reason=MS_SANITIZER_SKIP_REASON, modes=["npu"]
    ),
    # c_api 版和 cpp 版同形：README 里是 6 条**可选**的 mssanitizer 调用（"根据调测需求
    # 执行对应的命令"），不是一条执行序列。cpp 版早就 skip 了，这个漏了。
    "02_simd_c_api/01_utilities/05_sanitizer/ms_sanitizer": SkipConfig(reason=MS_SANITIZER_SKIP_REASON, modes=["npu"]),
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin": SkipConfig(
        reason=TENSORFLOW_SKIP_REASON, modes=["npu"]
    ),
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom": SkipConfig(
        reason=TENSORFLOW_SKIP_REASON, modes=["npu"]
    ),
    # README 的场景表："1：Case 1的溢出场景，2：Case 1的优化场景，3：Case 2的溢出场景，
    # 4：Case 2的优化场景"，并明确写了溢出场景的预期输出是
    # "[Failed] Case accuracy verification failed!"。1 和 3 跑失败才是对的。
    "03_simt_api/05_troubleshooting/stack_overflow__scenario_1": SkipConfig(
        reason=NEGATIVE_SCENARIO_REASON, modes=["npu", "sim"]
    ),
    "03_simt_api/05_troubleshooting/stack_overflow__scenario_3": SkipConfig(
        reason=NEGATIVE_SCENARIO_REASON, modes=["npu", "sim"]
    ),
    # README 的"执行结果"表：12/13/14 是 SIMT VF 写越界，Kernel 运行结果一栏写的就是"失败"
    # （VEC访问UB越界）。在 ascend950-smoke 上实测这三个都在 8s 内 run 阶段 rc=1，与 README
    # 一致。其余 0-11 实测全部 rc=0 通过。
    #
    # 这三个必须 skip 的原因比"反例"更重：它们在设备上制造的故障要约 289s 才上报，而 CI 是
    # 每卡 4 并发，期间同卡的其它用例会一起被堵到 TIMEOUT(300s)。A5 CI 第二轮的证据：
    #   - 场景 11 是 README 写明的合法访问、预期成功，却跑了 301.0s 然后 FAIL；
    #   - 场景 3 最终 PASS，但耗时 297.1s；
    #   - 13 自己只用 8.2s，12/14 却是 297.1s/301.0s —— 耗时不是用例的属性。
    # 曾经据此把 10 个同卡用例当成"CI 节点挂死"禁掉，那是误判，已随本次放开。
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_12": SkipConfig(
        reason=NEGATIVE_SCENARIO_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_13": SkipConfig(
        reason=NEGATIVE_SCENARIO_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_14": SkipConfig(
        reason=NEGATIVE_SCENARIO_REASON, modes=["npu", "sim"]
    ),
    # 场景 10 是另一类反例，不能和 12/13/14 混为一谈：README 表格写的是"Kernel运行结果=成功、
    # 精度结果=失败、未报告运行错误但出现精度异常"（MainScalar 写进 Data Cache，破坏 SIMT
    # 缓存的 GM 数据）。但在 ascend950-smoke 上串行实测它是 rc=0 —— 即这个精度异常是非确定
    # 性的。不管按 README 还是按实测，它都不该待在一个 pass/fail 门禁里。
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_10": SkipConfig(
        reason=NEGATIVE_SCENARIO_REASON, modes=["npu", "sim"]
    ),
    "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache": SkipConfig(
        reason=MATMUL_L2CACHE_SKIP_REASON, modes=["cpu"]
    ),
    "01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance": SkipConfig(
        reason=LONG_TIME_REASON, modes=["cpu"]
    ),
    "01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_eltwise_high_performance": SkipConfig(
        reason=LONG_TIME_REASON, modes=["cpu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance": SkipConfig(
        reason=LONG_TIME_REASON, modes=["cpu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/05_best_practices/04_memory_access/bank_conflict_nd2nz": SkipConfig(
        reason=LONG_TIME_REASON, modes=["cpu"], archs=["dav-3510"]
    ),
    # ---- A5 CI 节点特有缺陷，不是用例本身的属性 ----
    # 三个 28 核算错的 matmul。前两个在 A2 上也存在且是通过的，所以必须 arch 限定；
    # matmul_tscm_src_vecout 本身只支持 dav-3510，限不限都一样，为可读性一起写上。
    "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_channelsplit_output": SkipConfig(
        reason=A5_MATMUL_28CORE_SKIP_REASON, modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_format_nd_align": SkipConfig(
        reason=A5_MATMUL_28CORE_SKIP_REASON, modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm_src_vecout": SkipConfig(
        reason=A5_MATMUL_28CORE_SKIP_REASON, modes=["npu"], archs=["dav-3510"]
    ),
    # 三个 A5 精度不符，A2 上都是 success，所以必须 arch 限定。
    "01_simd_cpp_api/04_advanced_api/03_quantization/quant": SkipConfig(
        reason=A5_ACCURACY_SKIP_REASON, modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/03_quantization/antiquant": SkipConfig(
        reason=A5_ACCURACY_SKIP_REASON, modes=["npu"], archs=["dav-3510"]
    ),
    "01_simd_cpp_api/04_advanced_api/02_normalization/layernorm": SkipConfig(
        reason=A5_ACCURACY_SKIP_REASON, modes=["npu"], archs=["dav-3510"]
    ),
    # teardown 静默挂死的定位证据（2026-11-09，同一份 4 卡分片，只差这几个的启用状态）：
    # - 两条已排除的归因，别再往那查：① 工具链看到的核数 —— 把 9599 的 ini 改成 28 核不复现；
    #   ② 「同卡的场景 12/13/14 制造 VEC 越界、设备故障约 289s 才上报把别人堵住」—— 12/13/14
    #   已 skip 不再上卡，这些放开后仍有 6 个跑满 301.0s（git=8f667bf0，7 FAIL，card_0 1249s），
    #   且该轮 1636 个 plog 里 0 条 ERROR，根本没有设备故障。
    # - lambda_operator_hybrid：禁用时 11.3s / 15.3s / 15.0s，放开那一轮 297.7s；
    #   ub_usage_limit 场景 4：禁用时 14.4s，放开那一轮 301.0s —— 两个都是纯连带受害者。
    # - 不是「hybrid 并发」本身的问题：这些 skip 掉之后，card_2 同轮并发跑 14 个 hybrid 用例
    #   全绿、run 都在 7-17s。
    # - adaptive_max_pool3d_grad 打完两遍 test pass!、场景 4 打完 [Case1] result=PASSED 之后才
    #   挂，即挂在 teardown/退出，计算本身是成功的。
    # - 那一轮分片按字典序，11 个 hybrid 挤在 card_0 队尾连跑：第一批 4 个槽（入队等待
    #   546.7~549.2s）全挂；第二批 adaptive 和 3 个正常用例同批、只有它自己挂；第三批
    #   stream_compaction 1/2 挂且把 lambda 拖到 297.7s。前 180 位零失败。
    # - 在 ascend950-smoke（9599/36 核、串行）上这些 run 只要 7-53s 且全过。
    # 元凶：同批入队的正常用例都过，只有它们自己挂；挂在 teardown，计算已经成功。
    "05_simd_simt_hybrid/02_best_practices/simd_simt_stream_compaction__scenario_0": SkipConfig(
        reason=A5_SIMT_HYBRID_HANG_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/02_best_practices/simd_simt_stream_compaction__scenario_1": SkipConfig(
        reason=A5_SIMT_HYBRID_HANG_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/02_best_practices/simd_simt_stream_compaction__scenario_2": SkipConfig(
        reason=A5_SIMT_HYBRID_HANG_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/02_best_practices/simd_simt_adaptive_max_pool3d_grad": SkipConfig(
        reason=A5_SIMT_HYBRID_HANG_SKIP_REASON, modes=["npu", "sim"]
    ),
    # 连带受害者：2026-11-09 那轮里场景 3/5 和 softmax 场景 1 实测分别 13.5s/11.0s/10.8s PASS，
    # 场景 2/11 是和 stream_compaction 场景 0 同批被拖挂的。等挂死修好应优先放开这 5 个。
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_2": SkipConfig(
        reason=A5_SIMT_HYBRID_COLLATERAL_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_3": SkipConfig(
        reason=A5_SIMT_HYBRID_COLLATERAL_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_5": SkipConfig(
        reason=A5_SIMT_HYBRID_COLLATERAL_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit__scenario_11": SkipConfig(
        reason=A5_SIMT_HYBRID_COLLATERAL_SKIP_REASON, modes=["npu", "sim"]
    ),
    "05_simd_simt_hybrid/02_best_practices/simd_simt_softmax__scenario_1": SkipConfig(
        reason=A5_SIMT_HYBRID_COLLATERAL_SKIP_REASON, modes=["npu", "sim"]
    ),
}

ARCH_OVERRIDES: dict[str, List[str]] = {}

# 手工维护、不由本脚本生成的 case。它们在 examples/ 下没有对应样例目录（编的是
# tests/ 下的整套测试），因此不会出现在 reports 里：remove_stale_runners 会把它们当野文件
# unlink，write_manifest 也不会给它们留条目。而 manifest 里没有条目时 load_runner_info
# 会把 supported_archs 回退成「当前 arch」，于是这两个只在 A2 跑的 case 会在 A5 上也被
# 编排，而 runner 的 cwd examples/<source_case> 不存在，clean 阶段直接 rc=127。
# 这里连元数据一起登记，由 hand_written_reports 写进 manifest。
HAND_WRITTEN_CASES: dict[str, dict[str, object]] = {
    "03_simt_api/presmoke_math__simt": {
        "source_case": "03_simt_api/00_introduction",
        "supported_archs": ["dav-2201"],
        "supported_modes": ["npu"],
    },
    "05_simd_simt_hybrid/presmoke_math__hybrid": {
        "source_case": "03_simt_api/00_introduction",
        "supported_archs": ["dav-2201"],
        "supported_modes": ["npu"],
    },
}

ARCH_ENV_KEYS = {"ARCH", "ASC_ARCH", "ASCEND_ARCH", "CMAKE_ASC_ARCHITECTURES", "NPU_ARCH"}

BASH_LICENSE_HEADER = [
    "#!/bin/bash",
    "# ----------------------------------------------------------------------------------------------------------",
    "# Copyright (c) 2026 Huawei Technologies Co., Ltd.",
    "# This program is free software, you can redistribute it and/or modify it under the terms and conditions of",
    '# CANN Open Software License Agreement Version 2.0 (the "License").',
    "# Please refer to the License for details. You may not use this file except in compliance with the License.",
    '# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,',
    "# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.",
    "# See LICENSE in the root of the software repository for the full text of the License.",
    "# ----------------------------------------------------------------------------------------------------------",
]


@dataclass
class CaseReport:
    case: str
    runner: str
    source: str
    confidence: str
    source_case: str = ""
    supported_archs: List[str] = field(default_factory=list)
    supported_modes: List[str] = field(default_factory=list)
    target_runnable: bool = False
    skip: bool = False
    skip_reason: str = ""
    skip_modes: List[str] = field(default_factory=list)
    skip_archs: List[str] = field(default_factory=list)
    reasons: List[str] = field(default_factory=list)
    commands: List[str] = field(default_factory=list)


@dataclass
class RunnerRenderSpec:
    rel: str
    build_cmds: List[Command]
    run_cmds: List[Command]
    verify_cmds: List[Command]
    source_rel: str = ""
    scenario_num: int | None = None
    custom_op_dependency: bool = False
    custom_op_package_case: bool = False
    skip_reason: str = ""
    skip_modes: List[str] = field(default_factory=list)
    skip_archs: List[str] = field(default_factory=list)
    all_modes_skipped: bool = False  # 语义见 custom_op_guard()


def main() -> int:
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    args = parse_args()
    project_root = Path(args.project_root).resolve()
    examples_root = project_root / "examples"
    runners_root = project_root / "scripts" / "presmoke" / "cases"
    reports_root = project_root / "scripts" / "presmoke" / "reports"
    runners_root.mkdir(parents=True, exist_ok=True)
    reports_root.mkdir(parents=True, exist_ok=True)

    specs = [parse_readme(path, examples_root) for path in discover_examples(examples_root)]
    apply_case_overrides(specs)
    cells, _ = build_cells(specs, args.arch, [args.mode])
    cells = expand_scenario_cells(cells, project_root)
    cells = [cell for cell in cells if args.arch in cell.example.archs and args.mode in cell.example.modes]
    runnable_by_rel = {cell.example.rel_path: cell for cell in cells}
    all_cells = [cell_for_spec(spec, args.arch, args.mode) for spec in specs]
    all_cells = expand_scenario_cells(all_cells, project_root)
    reports: List[CaseReport] = []
    for cell in all_cells:
        report = write_runner(
            project_root, runners_root, cell, runnable_on_target=cell.example.rel_path in runnable_by_rel
        )
        reports.append(report)
    reports.extend(hand_written_reports(project_root, runners_root, args.arch, args.mode))

    remove_stale_runners(runners_root, reports)
    write_manifest(reports_root, reports)
    LOG.info("generated_runners=%s", len(reports))
    LOG.info("low_confidence=%s", sum(1 for item in reports if item.confidence != "high"))
    LOG.info("reports_dir=%s", reports_root)
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate per-case presmoke run.sh wrappers")
    parser.add_argument("--project-root", default=Path(__file__).resolve().parents[2])
    parser.add_argument("--arch", default="dav-2201")
    parser.add_argument("--mode", default="npu")
    return parser.parse_args()


def cell_for_spec(spec, arch: str, mode: str) -> Cell:
    commands = [rewrite_command(command, arch, mode) for command in spec.commands]
    build_dir = spec.path / f"build_{mode}"
    return Cell(spec, arch, mode, commands, build_dir)


def apply_case_overrides(specs) -> None:
    for spec in specs:
        if spec.rel_path in ARCH_OVERRIDES:
            spec.archs = ARCH_OVERRIDES[spec.rel_path]


def write_runner(project_root: Path, runners_root: Path, cell, runnable_on_target: bool) -> CaseReport:
    rel = cell.example.rel_path
    runner_dir = runners_root / rel
    runner_dir.mkdir(parents=True, exist_ok=True)
    runner = runner_dir / "run.sh"
    confidence, reasons = runner_confidence(cell, runnable_on_target)
    render_spec, confidence = build_runner_render_spec(cell, confidence, reasons)
    text = render_runner(render_spec)
    runner.write_text(text, encoding="utf-8")
    runner.chmod(0o755)
    skip_reason = render_spec.skip_reason
    skip_modes = render_spec.skip_modes
    skip_archs = render_spec.skip_archs
    all_modes_skipped = bool(skip_reason and all(mode in skip_modes for mode in cell.example.modes))
    return CaseReport(
        case=rel,
        runner=runner.relative_to(project_root).as_posix(),
        source_case=source_rel_for_cell(project_root, cell),
        source=cell.example.source,
        confidence=confidence,
        supported_archs=cell.example.archs,
        supported_modes=cell.example.modes,
        target_runnable=runnable_on_target,
        skip=all_modes_skipped,
        skip_reason=skip_reason,
        skip_modes=skip_modes,
        skip_archs=skip_archs,
        reasons=reasons,
        commands=[command.raw for command in cell.commands if command.raw],
    )


def expand_scenario_cells(cells: Iterable[Cell], project_root: Path) -> List[Cell]:
    expanded: List[Cell] = []
    for cell in cells:
        cmake_file = cell.example.path / "CMakeLists.txt"
        values = parse_scenario_values_from_cmake(cmake_file)
        if not values:
            values = parse_scenario_values_from_readme(cell.example.path / "README.md")
        values = filter_by_cmake_gate(values, cmake_file)
        if len(values) <= 1:
            expanded.append(cell)
            continue
        default_value = default_scenario_value(cell, cmake_file)
        if default_value is None or default_value not in values:
            expanded.append(cell)
        for value in values:
            expanded.append(make_scenario_cell(cell, project_root, value))
    return expanded


def filter_by_cmake_gate(values: List[int], cmake_file: Path) -> List[int]:
    """用 CMakeLists 自己的入参校验把场景号过一遍。

    上面那些 parse_scenario_values_* 是正则从 CMakeLists/README 里抠数字的，会从
    *校验正则本身* 抠出不存在的场景号：`MATCHES "^([1-9]|1[0-6])$"` 里的 `1[0-6]`
    产出一个 0，`matmul_hif8_high_performance` 则多出一个 8。这些场景一配 cmake 就
    FATAL_ERROR，而且会被写进清单和分片，整轮必红。
    CMakeLists 里能找到权威的入参校验时，以它为准做交集；找不到就保持原样。
    """
    allowed = cmake_allowed_scenarios(cmake_file)
    if allowed is None:
        return values
    return [value for value in values if value in allowed]


def cmake_allowed_scenarios(cmake_file: Path) -> set[int] | None:
    if not cmake_file.exists():
        return None
    text = cmake_file.read_text(encoding="utf-8", errors="ignore")
    match = re.search(r"set\s*\(\s*VALID_SCENARIOS\s+([0-9\s]+)\)", text)
    if match:
        return {int(item) for item in match.group(1).split()}
    # 只认 if(NOT SCENARIO_NUM MATCHES "...") 这种校验形式；分支用的
    # if(SCENARIO_NUM MATCHES "^[0-1]$") 不是全集，拿来做交集会误删。
    match = re.search(r"NOT\s+SCENARIO(?:_NUM)?\s+MATCHES\s+\"([^\"]+)\"", text)
    if not match:
        return None
    pattern = match.group(1).strip().lstrip("^").rstrip("$")
    try:
        regex = re.compile(f"^(?:{pattern})$")
    except re.error:
        return None
    return {value for value in range(0, 33) if regex.match(str(value))}


def parse_scenario_values_from_cmake(cmake_file: Path) -> List[int]:
    if not cmake_file.exists():
        return []
    text = cmake_file.read_text(encoding="utf-8", errors="ignore")
    if "SCENARIO_NUM" not in text and "SCENARIO" not in text:
        return []

    values: set[int] = set()
    for match in re.finditer(r"set\s*\(\s*SCENARIO_NUM\s+[^)]*CACHE\s+STRING\s+\"([^\"]*)\"", text):
        values.update(parse_scenario_values_from_description(match.group(1)))
    for match in re.finditer(r"set\s*\(\s*SCENARIO\s+[^)]*CACHE\s+STRING\s+\"([^\"]*)\"", text):
        values.update(parse_scenario_values_from_description(match.group(1)))
    for match in re.finditer(r"SCENARIO_NUM[^\n\r\"]*\"([^\"]*)\"", text):
        values.update(parse_scenario_values_from_text(match.group(1)))
    for match in re.finditer(r"set\s*\(\s*VALID_SCENARIOS\s+([^)]+)\)", text):
        values.update(parse_scenario_values_from_text(match.group(1)))
    for match in re.finditer(r"SCENARIO_NUM[^\n\r]*(?:must be|Valid values are|specify)[^\n\r]*", text):
        values.update(parse_scenario_values_from_text(match.group(0)))
    return sorted(value for value in values if 0 <= value <= 32)


def parse_scenario_values_from_description(text: str) -> List[int]:
    mapping_keys = re.findall(r"(?:^|[(:,;]\s*)(\d+)\s*=", text)
    if mapping_keys:
        return sorted({int(value) for value in mapping_keys})
    return parse_scenario_values_from_text(text)


def parse_scenario_values_from_readme(readme_file: Path) -> List[int]:
    if not readme_file.exists():
        return []
    text = readme_file.read_text(encoding="utf-8", errors="ignore")
    values: set[int] = set()
    values.update(parse_scenario_values_from_html_tables(text))
    for line in text.splitlines():
        if "SCENARIO" not in line:
            continue
        table_match = re.search(r"\|\s*`?SCENARIO`?\s*\|\s*([^|]+)\|", line)
        if table_match:
            values.update(parse_scenario_values_from_text(table_match.group(1)))
            continue
        if re.search(r"(?:取值|Scenario|场景|default|默认)", line):
            values.update(parse_scenario_values_from_text(line))
    return sorted(value for value in values if 0 <= value <= 32)


def parse_scenario_values_from_html_tables(text: str) -> List[int]:
    values: set[int] = set()
    for table in re.findall(r"<table\b.*?</table>", text, flags=re.S | re.I):
        if "SCENARIO" not in table:
            continue
        for row in re.findall(r"<tr\b.*?</tr>", table, flags=re.S | re.I):
            cells = re.findall(r"<td\b[^>]*>(.*?)</td>", row, flags=re.S | re.I)
            if not cells:
                continue
            first_cell = re.sub(r"<.*?>", "", cells[0]).strip()
            if re.fullmatch(r"\d+", first_cell):
                values.add(int(first_cell))
    return sorted(value for value in values if 0 <= value <= 32)


def parse_scenario_values_from_text(text: str) -> List[int]:
    values: set[int] = set()
    text = collect_regex_character_class_scenarios(text, values)
    for start, end in re.findall(r"(?<!\d)(\d+)\s*-\s*(\d+)(?!\d)", text):
        first = int(start)
        last = int(end)
        if 0 <= first <= last <= 32:
            values.update(range(first, last + 1))
    for number in re.findall(r"(?<!\d)(\d+)(?!\d)", text):
        value = int(number)
        if 0 <= value <= 32:
            values.add(value)
    return sorted(values)


def collect_regex_character_class_scenarios(text: str, values: set[int]) -> str:
    def replace(match: re.Match) -> str:
        for char in match.group(1):
            values.add(int(char))
        return " "

    return re.sub(r"\^\[([0-9]+)\]\$", replace, text)


def default_scenario_value(cell: Cell, cmake_file: Path) -> int | None:
    for command in cell.commands:
        value = scenario_value_from_command(command)
        if value is not None:
            return value
    if not cmake_file.exists():
        return None
    text = cmake_file.read_text(encoding="utf-8", errors="ignore")
    match = re.search(r"set\s*\(\s*SCENARIO_NUM\s+\"?(\d+)\"?", text)
    return int(match.group(1)) if match else None


def scenario_value_from_command(command: Command) -> int | None:
    if "SCENARIO_NUM" in command.env:
        value = command.env["SCENARIO_NUM"]
        if value.isdigit():
            return int(value)
    if "SCENARIO" in command.env:
        value = command.env["SCENARIO"]
        if value.isdigit():
            return int(value)
    match = re.search(r"(?:-DSCENARIO_NUM=|SCENARIO_NUM=)(\d+)", command.raw)
    if match:
        return int(match.group(1))
    match = re.search(r"(?:-DSCENARIO=|SCENARIO=)(\d+)", command.raw)
    return int(match.group(1)) if match else None


def make_scenario_cell(cell: Cell, project_root: Path, scenario_num: int) -> Cell:
    source_rel = source_rel_for_cell(project_root, cell)
    scenario_rel = f"{source_rel}__scenario_{scenario_num}"
    commands = [rewrite_command_for_scenario(command, scenario_num) for command in cell.commands]
    archs = supported_archs_for_scenario(cell.example.archs, cell.example.path / "CMakeLists.txt", scenario_num)
    example = ExampleSpec(
        cell.example.path,
        scenario_rel,
        commands,
        archs,
        cell.example.modes,
        cell.example.source,
        cell.example.suggestions,
    )
    return Cell(
        example, cell.arch, cell.mode, commands, cell.example.path / f"build_{cell.mode}_scenario_{scenario_num}"
    )


def rewrite_command_for_scenario(command: Command, scenario_num: int) -> Command:
    raw = command.raw
    if raw:
        raw = replace_scenario_num_arg(raw, scenario_num)
        has_cmake_scenario = "-DSCENARIO_NUM=" in raw or "-DSCENARIO=" in raw
        if command.kind == "cmake" and is_cmake_configure(raw) and not has_cmake_scenario:
            raw = f"{raw} -DSCENARIO_NUM={scenario_num}"
    env = dict(command.env)
    env["SCENARIO_NUM"] = str(scenario_num)
    if "SCENARIO" in env:
        env["SCENARIO"] = str(scenario_num)
    return Command(raw, command.kind, env)


def replace_scenario_num_arg(command: str, scenario_num: int) -> str:
    value = str(scenario_num)
    patterns = [
        r"(-DSCENARIO_NUM=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(-DSCENARIO=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(\bSCENARIO_NUM=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(\bSCENARIO=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(-scenarioNum(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(-scenario_num(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(--scenario(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(--mode(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
    ]
    for pattern in patterns:
        command = re.sub(pattern, lambda match: match.group(1) + value, command)
    return command


def supported_archs_for_scenario(base_archs: List[str], cmake_file: Path, scenario_num: int) -> List[str]:
    if not cmake_file.exists():
        return base_archs
    text = scenario_arch_text(cmake_file)
    if scenario_only_supports_arch(text, scenario_num, "dav-3510"):
        return [arch for arch in base_archs if arch == "dav-3510"]
    if scenario_only_supports_arch(text, scenario_num, "dav-2201"):
        return [arch for arch in base_archs if arch == "dav-2201"]
    return base_archs


def scenario_arch_text(cmake_file: Path) -> str:
    parts = [cmake_file.read_text(encoding="utf-8", errors="ignore")]
    readme = cmake_file.parent / "README.md"
    if readme.exists():
        parts.append(readme.read_text(encoding="utf-8", errors="ignore"))
    for asc_file in sorted(cmake_file.parent.glob("*.asc")):
        parts.append(asc_file.read_text(encoding="utf-8", errors="ignore"))
    return "\n".join(parts)


def scenario_only_supports_arch(text: str, scenario_num: int, arch: str) -> bool:
    scenario = str(scenario_num)
    if re.search(rf"场景[0-9/、,，\s]*{scenario}[0-9/、,，\s]*仅支持{arch}", text):
        return True
    if re.search(
        rf"<tr>(?:(?!</tr>).)*<td>\s*{scenario}\s*</td>(?:(?!</tr>).)*仅支持\s*{arch}(?:(?!</tr>).)*</tr>",
        text,
        flags=re.S,
    ):
        return True
    if arch == "dav-3510" and re.search(
        rf"(?:场景|Scenario)\s*{scenario}[^\n\r]*(?:仅在|only supports)[^\n\r]*Ascend\s*950", text, flags=re.IGNORECASE
    ):
        return True
    if arch == "dav-2201" and re.search(
        rf"SCENARIO_NUM\s*=\s*{scenario}[^\n\r]*(?:不支持|does not support|not support)[^\n\r]*Ascend\s*950",
        text,
        flags=re.IGNORECASE,
    ):
        return True
    if arch == "dav-2201" and re.search(
        rf"SCENARIO_NUM\s*={scenario}`?\s*时[^\n\r]*(?:不支持|does not support|not support)[^\n\r]*Ascend\s*950",
        text,
        flags=re.IGNORECASE,
    ):
        return True
    if re.search(rf"{scenario}\s*=[^\n\r,，)]*(?:仅|only supports).*(?:Ascend\s*950|{arch})", text):
        return True
    if arch == "dav-3510" and scenario != "1":
        if re.search(r"A2/A3架构仅支持Scenario\s*1", text, flags=re.IGNORECASE):
            return True

    for match in re.finditer(r"if\s*\((.*?)\)(.*?)(?:endif\s*\(\)|$)", text, flags=re.S):
        condition = match.group(1)
        body = match.group(2)
        if not re.search(rf"SCENARIO_NUM[^\n\r)]*(?:STREQUAL|EQUAL)\s*\"?{scenario}\"?", condition):
            continue
        combined = condition + "\n" + body
        if arch == "dav-3510" and (
            f'NOT CMAKE_ASC_ARCHITECTURES STREQUAL "{arch}"' in combined
            or f"only supports {arch}" in combined
            or f"仅支持{arch}" in combined
            or "仅Ascend 950" in combined
        ):
            return True
        if arch == "dav-2201" and (
            'CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510"' in combined and "only supports dav-2201" in combined
        ):
            return True
    return False


def scenario_num_from_case_rel(rel: str) -> int | None:
    match = re.search(r"__scenario_(\d+)$", rel)
    return int(match.group(1)) if match else None


def source_rel_for_cell(project_root: Path | None, cell: Cell) -> str:
    if project_root is None:
        return re.sub(r"__scenario_\d+$", "", cell.example.rel_path)
    examples_root = project_root / "examples"
    try:
        return cell.example.path.relative_to(examples_root).as_posix()
    except ValueError:
        return re.sub(r"__scenario_\d+$", "", cell.example.rel_path)


def runner_confidence(cell, runnable_on_target: bool) -> tuple[str, List[str]]:
    confidence, reasons = classify_confidence(cell)
    if not runnable_on_target:
        reasons.append("unsupported_on_target_arch_or_mode")
        confidence = downgrade(confidence)
    return confidence, reasons


def build_runner_render_spec(cell, confidence: str, reasons: List[str]) -> tuple[RunnerRenderSpec, str]:
    rel = cell.example.rel_path
    source_rel = source_rel_for_cell(None, cell)
    build_cmds = build_commands_for_runner(cell.commands)
    run_cmds = run_commands_for_runner(cell.commands, build_cmds)
    build_cmds, run_cmds = merge_export_commands_into_run(build_cmds, run_cmds)
    verify_cmds = [command for command in cell.commands if command.raw and command.kind == "verify"]
    if not run_cmds:
        run_cmds = [Command(":", "run")]
        if "no_run_step" not in reasons:
            reasons.append("no_run_step")
            confidence = downgrade(confidence)
    if not verify_cmds:
        verify_cmds = [Command(":", "verify")]
    custom_op_package_case = source_rel == CUSTOM_OP_PACKAGE_CASE
    custom_op_dependency = requires_custom_op_package(source_rel)
    declared = declared_skip_config(source_rel, cell.example.rel_path)
    skip_reason, skip_modes, skip_archs = declared.reason, declared.modes, declared.archs
    # no-op 改写会把命令本身删掉，而一个 run.sh 要服务所有 arch，所以只有「不限 arch」的全模式
    # skip 才能走这条路。arch 限定的靠 run.sh 里的 SKIP_ARCHS 运行时守卫 + 选型层拦截。
    all_modes_skipped = bool(skip_reason) and not skip_archs and all(mode in skip_modes for mode in cell.example.modes)
    skip_ctx = _SkipContext(
        all_modes_skipped=all_modes_skipped,
        skip_reason=skip_reason,
        custom_op_package_case=custom_op_package_case,
        custom_op_dependency=custom_op_dependency,
    )
    result = _apply_skip_resolution(skip_ctx, confidence, reasons, (build_cmds, run_cmds, verify_cmds))
    return (
        RunnerRenderSpec(
            rel=rel,
            source_rel=source_rel,
            scenario_num=scenario_num_from_case_rel(rel),
            build_cmds=result.build_cmds,
            run_cmds=result.run_cmds,
            verify_cmds=result.verify_cmds,
            custom_op_dependency=custom_op_dependency,
            custom_op_package_case=custom_op_package_case,
            skip_reason=skip_reason,
            skip_modes=skip_modes,
            skip_archs=skip_archs,
            all_modes_skipped=all_modes_skipped,
        ),
        result.confidence,
    )


def _apply_skip_resolution(
    ctx: _SkipContext,
    confidence: str,
    reasons: List[str],
    stage_cmds: tuple[List[Command], List[Command], List[Command]],
) -> _SkipResult:
    build_cmds, run_cmds, verify_cmds = stage_cmds
    if ctx.all_modes_skipped:
        reasons.append("explicit_skip")
        noop_build, noop_run, noop_verify = noop_stage_commands()
        return _SkipResult("high", reasons, noop_build, noop_run, noop_verify)
    if ctx.skip_reason:
        reasons.append("partial_mode_skip")
        return _SkipResult("high", reasons, build_cmds, run_cmds, verify_cmds)
    if ctx.custom_op_package_case:
        reasons.append("custom_op_package_provider")
        noop_build, noop_run, noop_verify = noop_stage_commands()
        return _SkipResult(confidence, reasons, noop_build, noop_run, noop_verify)
    if ctx.custom_op_dependency:
        reasons.append("requires_custom_op_package")
    return _SkipResult(confidence, reasons, build_cmds, run_cmds, verify_cmds)


def noop_stage_commands() -> tuple[List[Command], List[Command], List[Command]]:
    return [Command(":", "build")], [Command(":", "run")], [Command(":", "verify")]


def build_commands_for_runner(commands: Iterable[Command]) -> List[Command]:
    command_list = list(commands)
    result: List[Command] = []
    for command in command_list:
        if not command.raw:
            continue
        if command.kind in {"cmake", "make", "gen_data", "shell"}:
            result.append(command)
            continue
        if is_atc_prerequisite_command(command, command_list):
            result.append(command)
    return result


def run_commands_for_runner(commands: Iterable[Command], build_cmds: List[Command]) -> List[Command]:
    result: List[Command] = []
    for command in commands:
        if not command.raw or command.kind not in {"run", "package_run"}:
            continue
        if command not in build_cmds:
            result.append(command)
    return result


def merge_export_commands_into_run(
    build_cmds: List[Command], run_cmds: List[Command]
) -> tuple[List[Command], List[Command]]:
    export_cmds: List[str] = []
    remaining_build_cmds: List[Command] = []
    for command in build_cmds:
        if is_export_command(command.raw):
            export_cmds.append(command.raw)
        else:
            remaining_build_cmds.append(command)
    if not export_cmds or not run_cmds:
        return build_cmds, run_cmds

    first_run = run_cmds[0]
    merged_run = Command("; ".join([*export_cmds, first_run.raw]), first_run.kind, dict(first_run.env))
    return remaining_build_cmds, [merged_run, *run_cmds[1:]]


def is_export_command(command: str) -> bool:
    return bool(re.match(r"^export\s+[A-Za-z_][A-Za-z0-9_]*=", command.strip()))


def classify_confidence(cell) -> tuple[str, List[str]]:
    reasons: List[str] = []
    confidence = "high"
    rel = cell.example.rel_path
    commands = [command.raw for command in cell.commands if command.raw]
    joined = "\n".join(commands)
    if cell.example.source != "readme":
        reasons.append(f"source_{cell.example.source}")
        confidence = downgrade(confidence)
    if rel.startswith(SPECIAL_PREFIXES):
        reasons.append("special_02_features_00_01_02")
        confidence = "low"
    if re.search(r"\$[A-Za-z_][A-Za-z0-9_]*|\$\{[^}]+}", joined):
        reasons.append("shell_variable_reference")
        confidence = "low"
    if re.search(r"(^|[;\s])(for|while|if|case|function)\b", joined):
        reasons.append("shell_control_flow")
        confidence = "low"
    if "\\\n" in joined or any(command.rstrip().endswith("\\") for command in commands):
        reasons.append("line_continuation")
        confidence = "low"
    if any("atc " in command or command.startswith("atc") for command in commands):
        reasons.append("atc_command")
        confidence = downgrade(confidence)
    if any("tensorflow" in command.lower() or "onnx" in command.lower() for command in commands):
        reasons.append("framework_command")
        confidence = downgrade(confidence)
    if not any(command.startswith("cmake") for command in commands):
        reasons.append("no_cmake_step")
        confidence = downgrade(confidence)
    if not any(command.startswith("make") for command in commands):
        reasons.append("no_make_step")
        confidence = downgrade(confidence)
    return confidence, sorted(set(reasons))


def is_atc_prerequisite_command(command: Command, commands: Iterable[Command]) -> bool:
    if command.kind != "run" or not command.raw.startswith("python3 "):
        return False
    command_list = list(commands)
    try:
        index = command_list.index(command)
    except ValueError:
        return False
    return any(item.raw.startswith("atc ") for item in command_list[index + 1 :])


def downgrade(confidence: str) -> str:
    if confidence == "high":
        return "medium"
    return confidence


def requires_custom_op_package(rel: str) -> bool:
    if rel == CUSTOM_OP_PACKAGE_CASE:
        return True
    if rel in CUSTOM_OP_PACKAGE_DEPENDENTS:
        return True
    if "/00_framework/01_tensorflow/" in rel:
        return True
    return False


def declared_skip_config(rel: str, scenario_rel: str = "") -> SkipConfig:
    """SKIP_CONFIG 的原始声明，不按 arch 过滤。

    manifest 是按单个 --arch 生成的，所以它必须带着完整声明，由 load_runner_info 在选型时
    按真实 arch 求值。以前生成阶段就按 arch 过滤一次，于是 archs 限定的登记整条进不了
    manifest、在目标 arch 上完全失效 —— gelu_eltwise_high_performance / gelu_high_performance
    / bank_conflict_nd2nz 三条长期是这个状态，写了等于没写。
    """
    # 先按带 __scenario_N 的全名查，查不到再退回样例路径。有些样例只有个别场景是「反例」
    # ——  故意触发错误、README 的场景表里就写着预期失败 —— 不能把整条样例一起 skip。
    return SKIP_CONFIG.get(scenario_rel) or SKIP_CONFIG.get(rel, SkipConfig())


def render_runner(spec: RunnerRenderSpec) -> str:
    spec = apply_scenario_to_render_spec(spec)
    if spec.rel == PARALLEL_OPS_PACKAGE_CASE:
        return render_parallel_ops_package_runner(spec)
    if spec.rel == TILING_SINK_PROGRAMMING_CASE:
        return render_tiling_sink_programming_runner(spec)
    build_prefix = custom_op_guard(spec.custom_op_dependency, spec.all_modes_skipped)
    run_prefix = custom_op_guard(spec.custom_op_package_case, spec.all_modes_skipped)
    inject_cmake_arch = spec.rel not in NO_CMAKE_ARCH_INJECTION_CASES
    lines = [
        *runner_header(spec),
        *runner_function(
            "case_build", [*build_prefix, *indent_commands(spec.build_cmds, inject_cmake_arch=inject_cmake_arch)]
        ),
        *runner_function("case_run", [*run_prefix, *indent_commands(spec.run_cmds, default_cd_build=True)]),
        *runner_function("case_verify", indent_commands(spec.verify_cmds, default_cd_build=True)),
        "case_clean() {",
        "    presmoke_default_clean",
        "}",
        "",
        'presmoke_case_main "$@"',
        "",
    ]
    return "\n".join(lines)


def apply_scenario_to_render_spec(spec: RunnerRenderSpec) -> RunnerRenderSpec:
    if spec.scenario_num is None:
        return spec
    # 用 replace，逐字段重建会漏抄新字段。
    return replace(
        spec,
        build_cmds=[rewrite_command_for_scenario(command, spec.scenario_num) for command in spec.build_cmds],
        run_cmds=[rewrite_command_for_scenario(command, spec.scenario_num) for command in spec.run_cmds],
        verify_cmds=[rewrite_command_for_scenario(command, spec.scenario_num) for command in spec.verify_cmds],
    )


def render_tiling_sink_programming_runner(spec: RunnerRenderSpec) -> str:
    build_prefix = custom_op_guard(spec.custom_op_dependency, spec.all_modes_skipped)
    lines = [
        *runner_header(spec),
        *runner_function("case_build", [*build_prefix, *indent_commands(spec.build_cmds)]),
        *runner_function(
            "case_run",
            [
                "    local sink_log_pattern",
                f"    sink_log_pattern={shlex.quote(TILING_SINK_GENERATE_TASK_PATTERN_PREFIX)}",
                f"    sink_log_pattern+={shlex.quote(TILING_SINK_GENERATE_TASK_PATTERN_SUFFIX)}",
                *indent_tiling_sink_run_commands(spec.run_cmds),
                '    presmoke_verify_tiling_sink_task_log_for_pid "$case_pid" "$sink_log_pattern"',
            ],
        ),
        *runner_function("case_verify", indent_commands(spec.verify_cmds, default_cd_build=True)),
        "case_clean() {",
        "    presmoke_default_clean",
        "}",
        "",
        'presmoke_case_main "$@"',
        "",
    ]
    return "\n".join(lines)


def render_parallel_ops_package_runner(spec: RunnerRenderSpec) -> str:
    lines = [
        *runner_header(spec),
        "case_build() {",
        "    local cmake_args=(cmake -S . -B build)",
        '    if [[ -n "${NLOHMANN_JSON_URL:-}" ]]; then',
        '        cmake_args+=("-DNLOHMANN_JSON_URL=$NLOHMANN_JSON_URL")',
        "    fi",
        '    if [[ -n "${RUN_MODE_ARG:-}" ]]; then',
        '        cmake_args+=("$RUN_MODE_ARG")',
        "    fi",
        "",
        '    mkdir -p "$BUILD_DIR"',
        '    (cd "$CASE_DIR" && soc_version=$SOC_VERSION presmoke_run_command "${cmake_args[@]}")',
        '    (cd "$CASE_DIR" && soc_version=$SOC_VERSION presmoke_run_command cmake --build build -j)',
        "}",
        "",
        *runner_function("case_run", indent_commands(spec.run_cmds, default_cd_build=True)),
        *runner_function("case_verify", indent_commands(spec.verify_cmds, default_cd_build=True)),
        "case_clean() {",
        '    rm -rf "$CASE_DIR/build" "$BUILD_DIR"',
        "}",
        "",
        'presmoke_case_main "$@"',
        "",
    ]
    return "\n".join(lines)


def custom_op_guard(enabled: bool, all_modes_skipped: bool) -> List[str]:
    # 判据是 all_modes_skipped 不是 skip_reason：arch/模式限定的 skip 在别的 arch/模式上
    # 还是真跑，skip_reason 非空也不能省掉守卫。
    return ["    presmoke_ensure_custom_op_package"] if enabled and not all_modes_skipped else []


def runner_header(spec: RunnerRenderSpec) -> List[str]:
    source_rel = spec.source_rel or spec.rel
    return [
        *BASH_LICENSE_HEADER,
        "",
        "set -euo pipefail",
        "",
        f"CASE_REL={shlex.quote(source_rel)}",
        *skip_reason_lines(spec.skip_reason),
        *skip_modes_lines(spec.skip_modes),
        *skip_archs_lines(spec.skip_archs),
        'SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"',
        f'source "$SCRIPT_DIR/{case_entry_relative_path(spec.rel)}"',
        'presmoke_case_init "$CASE_REL"',
        *scenario_build_dir_lines(spec.scenario_num),
        "",
    ]


def scenario_build_dir_lines(scenario_num: int | None) -> List[str]:
    if scenario_num is None:
        return []
    return [f'BUILD_DIR="$CASE_DIR/build_${{MODE}}_scenario_{scenario_num}"', "export BUILD_DIR"]


def skip_reason_lines(skip_reason: str) -> List[str]:
    if not skip_reason:
        return []
    return [f"SKIP_REASON={shlex.quote(skip_reason)}", ""]


def skip_modes_lines(skip_modes: List[str]) -> List[str]:
    if not skip_modes:
        return []
    modes_str = " ".join(skip_modes)
    return [f"SKIP_MODES=({modes_str})", ""]


def skip_archs_lines(skip_archs: List[str]) -> List[str]:
    # 一个 run.sh 服务所有 arch，所以 arch 限定只能在运行时判，不能在生成时烘进去。
    if not skip_archs:
        return []
    archs_str = " ".join(skip_archs)
    return [f"SKIP_ARCHS=({archs_str})", ""]


def runner_function(name: str, body: List[str]) -> List[str]:
    return [f"{name}() {{", '    mkdir -p "$BUILD_DIR"', *body, "}", ""]


def case_entry_relative_path(rel: str) -> str:
    depth = len(Path(rel).parts)
    return "../" * depth + "_case_entry.sh"


def indent_commands(
    commands: Iterable[Command], default_cd_build: bool = False, inject_cmake_arch: bool = True
) -> List[str]:
    lines: List[str] = []
    for command in commands:
        if isinstance(command, str):
            command = Command(command, "shell")
        lines.append(indent_command(command, default_cd_build, inject_cmake_arch))
    return lines


def indent_tiling_sink_run_commands(commands: Iterable[Command]) -> List[str]:
    lines: List[str] = ["    local case_pid case_rc=0"]
    for command in commands:
        raw = rewrite_runtime_arch_options(command.raw)
        raw = rewrite_runtime_cmake_options(raw)
        quoted = shlex.quote(f"exec {raw}")
        env_prefix = command_env_prefix(command)
        lines.extend(
            [
                "    (",
                f'        cd "{command_workdir(raw, default_cd_build=True)}"',
                "        exec env \\",
                "            ASCEND_GLOBAL_LOG_LEVEL=1 \\",
                f"            {env_prefix}bash -lc {quoted}",
                "    ) &",
                "    case_pid=$!",
                '    wait "$case_pid" || case_rc=$?',
                "    if (( case_rc != 0 )); then",
                '        return "$case_rc"',
                "    fi",
            ]
        )
    return lines


def indent_command(command: Command, default_cd_build: bool = False, inject_cmake_arch: bool = True) -> str:
    raw = command.raw
    if not raw or raw == ":":
        return "    :"
    raw = rewrite_runtime_arch_options(raw)
    raw = rewrite_runtime_cmake_options(raw, inject_cmake_arch)
    quoted = shlex.quote(raw)
    env_prefix = command_env_prefix(command)
    runner = f"{env_prefix}bash -lc {quoted}"
    return f'    (cd "{command_workdir(raw, default_cd_build)}" && {runner})'


def command_workdir(command: str, default_cd_build: bool = False) -> str:
    if command.startswith("cmake "):
        if is_cmake_configure(command) and (" .." in command or command.endswith("..")):
            return "$BUILD_DIR"
        return "$CASE_DIR"
    if command.startswith("make") or command.startswith("atc "):
        return "$BUILD_DIR"
    if command_runs_from_build_dir(command, default_cd_build):
        return "$BUILD_DIR"
    return "$CASE_DIR"


def command_runs_from_build_dir(command: str, default_cd_build: bool = False) -> bool:
    if re.match(r"^(export\s+[A-Za-z_][A-Za-z0-9_]*=.*;\s*)+\./", command.strip()):
        return True
    if command.startswith("python3 ../"):
        return True
    if "python3 ../" in command and ("--application=" in command or command.startswith("msprof ")):
        return True
    inner = strip_profiler_wrapper(command)
    if inner is not None:
        return command_runs_from_build_dir(inner, default_cd_build)
    return default_cd_build and command.startswith("./") and not command.startswith("./build/")


def strip_profiler_wrapper(command: str) -> str | None:
    """剥掉 NPU 工具包装器，返回它真正要执行的命令；不是包装器时返回 None。

    这些工具真正要跑的二进制在参数里（`msopprof ./demo`、`msprof op ./demo`、
    `mssanitizer ./demo --tool=racecheck`），而那个二进制是 make 产物、在 build 目录下 ——
    README 的命令块本身也是 cd build 之后才跑的。只看命令首词会把它们判成 CASE_DIR，工具
    于是找不到 ./demo（mssanitizer 报 "program path NOT found"）。注意 `msprof op` 这种
    形式**找不到目标也返回 0**（只打一行 "application to be profiled is not exist"），
    所以这类用例会常年绿着空过，不会有人发现。
    """
    parts = command.strip().split()
    if not parts or parts[0] not in NPU_TOOL_WRAPPERS:
        return None
    rest = parts[1:]
    if rest and rest[0] == "op":
        rest = rest[1:]
    return " ".join(item for item in rest if not item.startswith("-"))


def rewrite_runtime_cmake_options(command: str, inject_cmake_arch: bool = True) -> str:
    if not command.startswith("cmake ") or not is_cmake_configure(command):
        return command
    command = re.sub(
        r"-DCMAKE_ASC_ARCHITECTURES=(?:'[^']*'|\"[^\"]*\"|[^ \t;]+)",
        '-DCMAKE_ASC_ARCHITECTURES="$ARCH"' if inject_cmake_arch else "",
        command,
    )
    command = re.sub(r"-DCMAKE_ASC_RUN_MODE=(?:'[^']*'|\"[^\"]*\"|[^ \t;]+)", "", command)
    command = re.sub(r"[ \t]+", " ", command).strip()
    if inject_cmake_arch and "-DCMAKE_ASC_ARCHITECTURES=" not in command:
        command = f'{command} -DCMAKE_ASC_ARCHITECTURES="$ARCH"'
    return f"{command} $RUN_MODE_ARG"


def rewrite_runtime_arch_options(command: str) -> str:
    command = re.sub(
        r"--npu-arch=(?:'[^']*'|\"[^\"]*\"|\$[A-Za-z_][A-Za-z0-9_]*|\$\{[^}]+}|[^ \t;]+)", "--npu-arch=$ARCH", command
    )
    command = re.sub(
        r"-DNPU_ARCH=(?:'[^']*'|\"[^\"]*\"|\$[A-Za-z_][A-Za-z0-9_]*|\$\{[^}]+}|[^ \t;]+)", '-DNPU_ARCH="$ARCH"', command
    )
    return command


def command_env_prefix(command: Command) -> str:
    env = dict(command.env)
    env.setdefault("soc_version", "$SOC_VERSION")
    for key in ARCH_ENV_KEYS:
        if key in env:
            env[key] = "$ARCH"
    if not env:
        return ""
    parts = [f"{key}={quote_env_value(str(value))}" for key, value in sorted(env.items())]
    return " ".join(parts) + " "


def quote_env_value(value: str) -> str:
    if value in {"$ARCH", "$SOC_VERSION"}:
        return value
    if re.search(r"(^|[^\\])\$[A-Za-z_{]", value):
        return value
    return shlex.quote(value)


def write_manifest(reports_root: Path, reports: List[CaseReport]) -> None:
    (reports_root / "case_runner_manifest.json").write_text(
        # 末尾换行不能省：pre-commit 的 end-of-file-fixer 会补上，不写就每次重跑都多一条假 diff。
        json.dumps([asdict(report) for report in reports], ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )


def hand_written_reports(project_root: Path, runners_root: Path, arch: str, mode: str) -> List[CaseReport]:
    """给 HAND_WRITTEN_CASES 里实际存在的 run.sh 补 manifest 条目。"""
    reports: List[CaseReport] = []
    for rel, meta in HAND_WRITTEN_CASES.items():
        runner = runners_root / rel / "run.sh"
        if not runner.exists():
            continue
        archs = list(meta["supported_archs"])
        modes = list(meta["supported_modes"])
        reports.append(
            CaseReport(
                case=rel,
                runner=runner.relative_to(project_root).as_posix(),
                source="hand-written",
                confidence="high",
                source_case=str(meta["source_case"]),
                supported_archs=archs,
                supported_modes=modes,
                target_runnable=arch in archs and mode in modes,
            )
        )
    return reports


def remove_stale_runners(runners_root: Path, reports: List[CaseReport]) -> None:
    expected = {item.case for item in reports}
    for runner in list(runners_root.rglob("run.sh")):
        rel = runner.parent.relative_to(runners_root).as_posix()
        if rel in expected:
            continue
        LOG.info("remove_stale_runner=%s", rel)
        runner.unlink()
        remove_empty_parents(runner.parent, runners_root)


def remove_empty_parents(path: Path, stop: Path) -> None:
    while path != stop:
        try:
            path.rmdir()
        except OSError:
            return
        path = path.parent


if __name__ == "__main__":
    raise SystemExit(main())
