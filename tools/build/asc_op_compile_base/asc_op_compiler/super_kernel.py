#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
super kernel
"""

import os
import stat
from .global_storage import global_var_storage
from .super_kernel_utility import (
    CommonUtility,
    gen_func_align_attribute,
    get_wait_flag_for_chip,
    AscendCLogLevel,
)
from .super_kernel_op_compile import compile_super_kernel, gen_file_header
from .super_kernel_constants import (
    SuperKernelPreLoadMode,
    SuperKernelDataCacheMode,
    SuperKernelEarlyStartMode,
    SubOperatorType,
    SuperKernelDebugDcciAllMode,
    SuperKernelDebugSyncAllMode,
    SuperKernelFeedSyncAllMode,
    SuperKernelProfilingMode,
    ERR_CODE,
    SuperKernelDeviceType,
    SuperKernelKernelType,
)
from .super_kernel_sub_op_infos import indent_code_func, SubOperatorInfos
from .super_kernel_op_infos import SuperOperatorInfos


def kernel_meta_type_to_device_type(kernel_type: SuperKernelKernelType):
    aiv_configs = [
        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
    ]
    aic_configs = [
        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
    ]
    mix_configs = [
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
    ]

    if kernel_type in aiv_configs:
        return SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIV.value
    if kernel_type in aic_configs:
        return SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIC.value
    if kernel_type in mix_configs:
        return SuperKernelDeviceType.KERNEL_DEVICE_TYPE_MIX.value
    return SuperKernelDeviceType.KERNEL_DEVICE_TYPE_MAX.value


def gen_early_start_config(
    pre_sub_operator: SubOperatorInfos, sub_operator: SubOperatorInfos
):
    pre_sub_operator_device_type = kernel_meta_type_to_device_type(
        pre_sub_operator.kernel_type
    )
    sub_operator_device_type = kernel_meta_type_to_device_type(sub_operator.kernel_type)

    if (
        pre_sub_operator_device_type
        == SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIC.value
    ):
        prev_sub_kernel_config = 0
    elif (
        pre_sub_operator_device_type
        == SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIV.value
    ):
        prev_sub_kernel_config = 1
    elif (
        pre_sub_operator_device_type
        == SuperKernelDeviceType.KERNEL_DEVICE_TYPE_MIX.value
    ):
        prev_sub_kernel_config = 2
    else:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            f"Do not support previous sub kernel device type: {pre_sub_operator_device_type}. \
                Should be AIC, AIV or MIX.",
        )

    if sub_operator_device_type == SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIC.value:
        cur_sub_kernel_config = 0
    elif sub_operator_device_type == SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIV.value:
        cur_sub_kernel_config = 1
    elif sub_operator_device_type == SuperKernelDeviceType.KERNEL_DEVICE_TYPE_MIX.value:
        cur_sub_kernel_config = 2
    else:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            f"Do not support current sub kernel device type: {sub_operator_device_type}. \
                Should be AIC, AIV or MIX.",
        )

    super_kernel_early_start_config = (
        prev_sub_kernel_config << 2
    ) | cur_sub_kernel_config
    # sub_operator.elf.early_start_complement_wait_flag_block
    sub_operator.early_start_complement_wait_flag_block = (
        sub_operator.early_start_complement_wait_flag_block.replace(
            "__placehoder__earlay_config__", f"{super_kernel_early_start_config}"
        )
    )
    return f"g_super_kernel_early_start_config = {super_kernel_early_start_config};\n"


def gen_notify_wait_func():
    notify_func = """
template<bool aic_flag>
__aicore__ inline void NotifyFunc(GM_ADDR notify_lock_addr)
{
    if constexpr (aic_flag) {
        if (get_block_idx() == 0) {
            __gm__ uint64_t* notifyLock = reinterpret_cast<__gm__ uint64_t*>(notify_lock_addr);
            *notifyLock = 1;
            dcci(notifyLock, 0, 2);
        }
    } else {
        if (AscendC::GetBlockIdx() == 0) {
            __gm__ uint64_t* notifyLock = reinterpret_cast<__gm__ uint64_t*>(notify_lock_addr);
            *notifyLock = 1;
            dcci(notifyLock, 0, 2);
        }
    }
}\n
"""
    wait_func = """
template<bool aic_flag>
__aicore__ inline void WaitFunc(GM_ADDR wait_lock_addr)
{
    if constexpr (aic_flag) {
        __gm__ volatile uint64_t* waitLock = reinterpret_cast<__gm__ uint64_t*>(wait_lock_addr);
        if (get_block_idx() == 0) {
            dcci(waitLock, 0, 2);
            while(*waitLock != 1) {
                dcci(waitLock, 0, 2);
            }
        }
    } else {
        __gm__ volatile uint64_t* waitLock = reinterpret_cast<__gm__ uint64_t*>(wait_lock_addr);
        if (AscendC::GetBlockIdx() == 0) {
            dcci(waitLock, 0, 2);
            while(*waitLock != 1) {
                dcci(waitLock, 0, 2);
            }
        }
    }
}\n
"""
    return notify_func + wait_func


def get_sync_code_by_kernel_type(kernel_type):
    if kernel_type in [
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
    ]:
        return "AscendC::SyncAll<false>();\n\n"
    elif kernel_type in [
        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
    ]:
        return f"""
ffts_cross_core_sync(PIPE_FIX, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
{get_wait_flag_for_chip("AscendC::SYNC_AIC_FLAG")}
"""
    else:
        return f"""
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
{get_wait_flag_for_chip("AscendC::SYNC_AIV_ONLY_ALL")}
"""


def gen_inter_ops_barrier(
    super_operator: SuperOperatorInfos,
    pre_sub_operator: SubOperatorInfos,
    sub_operator: SubOperatorInfos,
):
    inter_ops_bar = "// begin inter ops barrier\n"
    if (
        super_operator.early_start_mode.value
        != SuperKernelEarlyStartMode.EarlyStartDisable.value
    ):
        inter_ops_bar += pre_sub_operator.early_start_complement_set_flag_block
        if (
            super_operator.early_start_mode.value
            == SuperKernelEarlyStartMode.EarlyStartEnableV2.value
            or super_operator.early_start_mode.value
            == SuperKernelEarlyStartMode.EarlyStartV2DisableSubKernel.value
        ):
            inter_ops_bar += gen_early_start_config(pre_sub_operator, sub_operator)
        inter_ops_bar += sub_operator.early_start_complement_wait_flag_block
    else:
        inter_ops_bar += "// reason2: inter op barrier when EarlyStartDisable \n"
        inter_ops_bar += get_sync_code_by_kernel_type(super_operator.kernel_type)

    return inter_ops_bar


def gen_op_end_debug_dcci_all(super_operator: SuperOperatorInfos):
    op_end_debug_dcci_all = ""
    if (
        super_operator.debug_dcci_all_mode.value
        == SuperKernelDebugDcciAllMode.DebugDcciAllEnable.value
    ):
        op_end_debug_dcci_all += "// op end debug dcci all.\n"
        op_end_debug_dcci_all += "pipe_barrier(PIPE_ALL);\n\
dcci((__gm__ uint64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);\n\n"
    return op_end_debug_dcci_all


def gen_op_end_debug_sync_all(super_operator: SuperOperatorInfos):
    op_end_debug_sync_all = ""
    if (
        super_operator.debug_sync_all_mode.value
        == SuperKernelDebugSyncAllMode.DebugSyncAllEnable.value
    ):
        op_end_debug_sync_all += "// op end debug sync all.\n"
        op_end_debug_sync_all += get_sync_code_by_kernel_type(
            super_operator.kernel_type
        )
    return op_end_debug_sync_all


def gen_2_real_stream_op_end_debug_sync_all_by_arch(
    super_operator: SuperOperatorInfos, arch
):
    op_end_debug_sync_all = ""
    if (
        super_operator.debug_sync_all_mode.value
        == SuperKernelDebugSyncAllMode.DebugSyncAllEnable.value
    ):
        op_end_debug_sync_all += "// op end debug sync all.\n"
        if arch == "aiv":
            op_end_debug_sync_all += f"pipe_barrier(PIPE_ALL);\n\
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));\n\
{get_wait_flag_for_chip('AscendC::SYNC_AIV_ONLY_ALL')}\n\n"
        elif arch == "aic":
            op_end_debug_sync_all += f"pipe_barrier(PIPE_ALL);\n\
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));\n\
{get_wait_flag_for_chip('AscendC::SYNC_AIC_FLAG')}\n\n"

    return op_end_debug_sync_all


def tpl_of_gen_switch_case_call(block_idx, dynamic_operator, super_operator):
    if super_operator.kernel_type in [
        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
    ]:
        condition_code = "get_block_idx"
        core_type = "ASCEND_IS_AIC"
    else:
        condition_code = "AscendC::GetBlockIdx"
        core_type = "ASCEND_IS_AIV"
    aiv_func_list = dynamic_operator._gen_preload_list_with_num("aiv_func_addr", 8)
    aic_func_list = dynamic_operator._gen_preload_list_with_num("aic_func_addr", 16)
    # need judge kernel type by tiling key
    if super_operator.kernel_type in [
        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
    ]:
        aiv_codes = indent_code_func(
            dynamic_operator.gen_call_func(
                aiv_func_list, "ASCEND_IS_AIV", "AscendC::GetBlockIdx"
            )
        )
        call_dynamic_switch_func = f"""
    {dynamic_operator.call_dynamic_switch_func}
{aiv_codes}
"""
    else:
        aiv_codes = indent_code_func(
            dynamic_operator.gen_call_func(
                aiv_func_list, "ASCEND_IS_AIV", "get_block_idx"
            )
        )
        aic_codes = indent_code_func(
            dynamic_operator.gen_call_func(
                aic_func_list, "ASCEND_IS_AIC", "get_block_idx"
            )
        )
        call_dynamic_switch_func = f"""
    {dynamic_operator.call_dynamic_switch_func}
"""
    return call_dynamic_switch_func


def gen_switch_case_call_block_of_dynamic_op(
    super_operator, next_sub_operator, sub_operator, pre_sub_operator
):
    switch_case_call_block = ""

    # if can not find free core before dynamic, wait for get tilingkey and block num
    if (
        sub_operator.sub_op_task_type.value == SubOperatorType.DYNAMIC_OP.value
        and sub_operator.switch_func_called_flag is False
    ):
        switch_case_call_block += tpl_of_gen_switch_case_call(
            sub_operator.start_block_idx, sub_operator, super_operator
        )
        if pre_sub_operator is None and not super_operator.enable_double_stream:
            switch_case_call_block += indent_code_func("pipe_barrier(PIPE_ALL);\n")
            switch_case_call_block += indent_code_func(
                "AscendC::SyncAll<false>(); // reason3: dynamic gen_switch_case_block when no pre op\n"
            )
    return switch_case_call_block


def print_params_addr(super_kernel_params):
    result = ""
    index = 0
    if not CommonUtility.is_c310():
        result += 'AscendC::printf("ffts_addr: %p\\n", ffts_addr); //para index: 0\n'
        index += 1
    for param in super_kernel_params:
        result += f'AscendC::printf("{param}: %p\\n", {param}); //para index: {index}\n'
        index += 1
    return result


def gen_clear_wait_sync_addr_code(super_operator):
    result = ""
    cnt = 0
    for op in super_operator.info_base:
        index = 0
        for recv_index in op.recv_event_list:
            if recv_index not in super_operator.inner_event_id_set:
                if op.kernel_type in [
                    SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                    SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                ]:
                    result += indent_code_func("if ASCEND_IS_AIC {\n")
                    result += indent_code_func("    if (get_block_idx() == 0) {\n")
                else:
                    result += indent_code_func("if ASCEND_IS_AIV {\n")
                    result += indent_code_func(
                        "    if (AscendC::GetBlockIdx() == 0) {\n"
                    )
                recv_wait_lock_offset = op.wait_param_offset + index
                result += indent_code_func(
                    f"\
        *(reinterpret_cast<__gm__ uint64_t*>(param_base[{recv_wait_lock_offset}])) = 0;\n"
                )
                cnt += 1
                result += indent_code_func("    }\n")
                result += indent_code_func("}\n")
            index += 1
    return result


def process_gen_stream_send_code(super_operator, op, arch, need_flag, code):
    code_str = ""
    if arch == "aic":
        if need_flag:
            code_str += code
        else:
            if len(op.send_info) == 0 and op != super_operator.cub_op_list[-1]:
                code_str += "// insert pipe all for ops\n"
                code_str += "   pipe_barrier(PIPE_ALL);\n"
    else:
        if need_flag:
            code_str += code
        else:
            if len(op.send_info) == 0 and op != super_operator.vec_op_list[-1]:
                code_str += "// insert pipe all for ops\n"
                code_str += "   pipe_barrier(PIPE_ALL);\n"
    return code_str


def gen_2_real_stream_send_code(super_operator, op, arch):
    super_kernel_file = ""
    need_sync_self = False
    need_sync_event_for_notify = (op.is_last_op is True) and (
        op.notify_block.get("aic", "") != "" or op.notify_block.get("aiv", "") != ""
    )
    if op.index == super_operator.info_base[-1].index:
        return super_kernel_file
    if arch == "aic":
        code = f"// Rule 1 : sync all {arch} must be insert behind each {arch} sub operator, when has real send info\n"
        code += (
            f"// sync all C->C kernel_name:{op.kernel_name}, send_info:{op.send_info}\n"
        )
        code += "ffts_cross_core_sync(PIPE_FIX, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));\n"
        code += f"{get_wait_flag_for_chip('AscendC::SYNC_AIC_FLAG')}\n\n"

        for single in op.send_info:
            info_pairs = op.send_info[single].split(";")
            if "cub:cub" in info_pairs or "vec:cub" in info_pairs:
                need_sync_self = True
            if "cub:vec" in info_pairs:
                code += (
                    f"// Rule 3.1 : sync all c2v must be insert when sendinfo has c2v, \
kernel_name:{op.kernel_name}, send_info:{op.send_info}\n"
                )
                code += "// send sync of C->V;\n"
                code += "ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));\n\n"
                need_sync_self = True

        super_kernel_file += process_gen_stream_send_code(
            super_operator, op, arch, need_sync_self or need_sync_event_for_notify, code
        )
    else:
        code = f"// Rule 1 : sync all {arch} must be insert behind each {arch} sub operator, when has real send info\n"
        code += (
            f"// sync all V->V kernel_name:{op.kernel_name}, send_info:{op.send_info}\n"
        )
        code += "ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));\n"
        code += f"{get_wait_flag_for_chip('AscendC::SYNC_AIV_ONLY_ALL')}\n\n"

        for single in op.send_info:
            info_pairs = op.send_info[single].split(";")
            if "vec:vec" in info_pairs or "cub:vec" in info_pairs:
                need_sync_self = True
            if "vec:cub" in info_pairs:
                code += (
                    f"// Rule 3.1 : sync all v2c must be insert when sendinfo has v2c, \
kernel_name:{op.kernel_name}, send_info:{op.send_info}\n"
                )
                code += "// send sync of V->C;\n"
                code += "ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));\n\n"
                need_sync_self = True

        super_kernel_file += process_gen_stream_send_code(
            super_operator, op, arch, need_sync_self or need_sync_event_for_notify, code
        )
    return super_kernel_file


def gen_2_real_stream_recv_code(op, arch):
    super_kernel_file = ""
    if arch == "aic":
        for single in op.recv_info:
            if "vec:cub" in op.recv_info[single].split(";"):
                super_kernel_file += (
                    f"// Rule 3.2 : sync all v2c must be insert when recvinfo has v2c, \
kernel_name:{op.kernel_name}, send_info:{op.recv_info}\n"
                )
                super_kernel_file += "// receive sync of V->C;\n"
                super_kernel_file += (
                    f"{get_wait_flag_for_chip('AscendC::SYNC_AIV_FLAG')}\n"
                )
    else:
        for single in op.recv_info:
            if "cub:vec" in op.recv_info[single].split(";"):
                super_kernel_file += (
                    f"// Rule 3.2 : sync all c2v must be insert when recvinfo has c2v, \
kernel_name:{op.kernel_name}, send_info:{op.recv_info}\n"
                )
                super_kernel_file += "// receive sync of C->V;\n"
                super_kernel_file += (
                    f"{get_wait_flag_for_chip('AscendC::SYNC_AIC_AIV_FLAG')}\n"
                )
    return super_kernel_file


def gen_2_real_stream_sync_code(super_operator, pre_op, cur_op, arch):
    super_kernel_file = ""
    if pre_op is not None:
        super_kernel_file += gen_2_real_stream_send_code(super_operator, pre_op, arch)
    if cur_op is not None:
        super_kernel_file += gen_2_real_stream_recv_code(cur_op, arch)

    return super_kernel_file


def gen_sync_and_event_code_for_two_stream(
    super_operator, pre_sub_operator, sub_operator, arch
):
    sync_and_event_code = ""
    if len(sub_operator.recv_event_list) != 0:
        # pre op send inter-core sync, cur op recv inter-core sync
        sync_and_event_code += indent_code_func(
            gen_2_real_stream_sync_code(
                super_operator, pre_sub_operator, sub_operator, arch
            )
        )
        # pre op send to outside
        if pre_sub_operator is not None:
            if len(pre_sub_operator.send_event_list) != 0:
                sync_and_event_code += indent_code_func(
                    pre_sub_operator.notify_block[arch]
                )
        # current op wait for outside
        if pre_sub_operator is not None:
            if len(sub_operator.wait_block) != 0:
                sync_and_event_code += indent_code_func(sub_operator.wait_block)

                # add sync after notify/wait event
                sync_and_event_code += "// two stream when has wait event, add sync by current operator kernel type\n"
                if sub_operator.kernel_type in [
                    SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
                    SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                ]:
                    # add sync with sub_operator and sub_operator
                    sync_and_event_code += indent_code_func(
                        "AscendC::SyncAll<false>(); // reason3: for continues notify/wait event \n\n"
                    )
                elif sub_operator.kernel_type in [
                    SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                    SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                ]:
                    sync_and_event_code += (
                        "// reason3: for continues notify/wait event\n"
                    )
                    sync_and_event_code += "ffts_cross_core_sync(PIPE_FIX, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));\n"
                    sync_and_event_code += (
                        f"{get_wait_flag_for_chip('AscendC::SYNC_AIC_FLAG')}\n\n"
                    )
                else:
                    sync_and_event_code += (
                        "// reason3: for continues notify/wait event\n"
                    )
                    sync_and_event_code += "ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));\n"
                    sync_and_event_code += (
                        f"{get_wait_flag_for_chip('AscendC::SYNC_AIV_ONLY_ALL')}\n\n"
                    )
    else:
        # pre op send inter-core sync, cur op recv inter-core sync
        sync_and_event_code += indent_code_func(
            gen_2_real_stream_sync_code(
                super_operator, pre_sub_operator, sub_operator, arch
            )
        )

        # pre op send to outside
        if pre_sub_operator is not None:
            if len(pre_sub_operator.send_event_list) != 0:
                sync_and_event_code += indent_code_func(
                    pre_sub_operator.notify_block[arch]
                )

    return sync_and_event_code


def gen_2_real_stream_code_by_arch(
    super_operator, arch, super_kernel_params_str, exits_dynamic_op, sub_ops
):
    super_kernel_file = f"__aicore__ inline __attribute__((always_inline)) void \
auto_gen_{super_operator.kernel_name}_kernel_{arch}(void) {{\n"
    super_kernel_file += "    GM_ADDR *param_base = (GM_ADDR *)get_para_base();\n"
    if exits_dynamic_op is True:
        super_kernel_file += "    uint64_t aiv_func_addr = 0;\n"
        super_kernel_file += "    uint64_t aic_func_addr = 0;\n"
        super_kernel_file += "    uint64_t dy_blockNum = 0;\n"
        if super_operator.split_mode > 1:
            for i in range(1, super_operator.split_mode):
                super_kernel_file += f"    uint64_t aiv_func_addr_split{i} = 0;\n"
                super_kernel_file += f"    uint64_t aic_func_addr_split{i} = 0;\n"

    if super_operator.preload_mode.value == SuperKernelPreLoadMode.PreLoadByWhole.value:
        super_kernel_file += indent_code_func("AscendC::PreLoad(8);\n")

    for pre_sub_operator, sub_operator, next_sub_operator in zip(
        [None] + sub_ops[:-1], sub_ops, sub_ops[1:] + [None]
    ):
        super_kernel_file += indent_code_func(
            f"//begin func call of sub operator {sub_operator.kernel_name}\n"
        )
        super_kernel_file += indent_code_func(
            sub_operator.get_notify_before_call_block(arch)
        )

        # generate switch case func of dynamic
        super_kernel_file += gen_switch_case_call_block_of_dynamic_op(
            super_operator, next_sub_operator, sub_operator, pre_sub_operator
        )

        # add preload of current func
        if (
            super_operator.preload_mode.value
            == SuperKernelPreLoadMode.PreLoadStepByStep.value
        ):
            super_kernel_file += indent_code_func(sub_operator.preload_call_block)

        # add preload of next func, when n+1 preload instr；
        if (
            super_operator.preload_mode.value
            == SuperKernelPreLoadMode.PreloadByAdanvanceStep.value
        ):
            if pre_sub_operator is None:
                super_kernel_file += indent_code_func(sub_operator.preload_call_block)
            if next_sub_operator is not None:
                super_kernel_file += indent_code_func(
                    next_sub_operator.preload_call_block
                )

        if (
            super_operator.datacache_mode.value
            == SuperKernelDataCacheMode.DataCacheLoadAdancanceStep.value
        ):
            if pre_sub_operator is None:
                super_kernel_file += indent_code_func(
                    sub_operator.data_cache_preload_call
                )
            if next_sub_operator is not None:
                super_kernel_file += indent_code_func(
                    next_sub_operator.data_cache_preload_call
                )
            super_kernel_file += "\n"

        if (
            pre_sub_operator is None
            and len(sub_operator.recv_event_list) != 0
            and sub_operator.index == 0
        ):
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"first op of super kernel must not have any recv event, op:{sub_operator.kernel_name}, \
event_list:{sub_operator.recv_event_list}",
            )

        super_kernel_file += gen_sync_and_event_code_for_two_stream(
            super_operator, pre_sub_operator, sub_operator, arch
        )

        tmp_code, enable_syncall_flag = gen_feed_syncall_var_init_code(
            super_operator, sub_operator
        )
        super_kernel_file += indent_code_func(tmp_code)
        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            super_kernel_file += indent_code_func(
                f"RecordProfiling({super_operator.info_base.index(sub_operator) + 1}, 0x8, true);\n"
            )
        if enable_syncall_flag is False:
            super_kernel_file += indent_code_func(sub_operator.kernel_call_block)
        else:
            super_kernel_file += indent_code_func(
                sub_operator.kernel_call_block_with_syncall
            )
        super_kernel_file += indent_code_func(gen_op_end_debug_dcci_all(super_operator))
        super_kernel_file += indent_code_func(
            gen_2_real_stream_op_end_debug_sync_all_by_arch(super_operator, arch)
        )

        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            super_kernel_file += indent_code_func(
                f"RecordProfiling({super_operator.info_base.index(sub_operator) + 1}, 0x8, false);\n"
            )

        if next_sub_operator is None:
            # last sub operator of que but not last sub operator in op list
            # allow send // lack : need check not last op dfx
            # last op send inter-core sync but not last op
            send_code = gen_2_real_stream_send_code(super_operator, sub_operator, arch)
            if (
                sub_operator.index == super_operator.info_base[-1].index
                and send_code != ""
            ):
                CommonUtility().ascendc_raise_python_err(
                    ERR_CODE,
                    f"last op of super kernel must not have any send info, op:{sub_operator.kernel_name}, \
event_list:{sub_operator.send_info}",
                )
            super_kernel_file += indent_code_func(send_code)
            if len(sub_operator.send_event_list) != 0:
                if sub_operator.index == super_operator.info_base[-1].index:
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        f"last op of super kernel must not have any send event, op:{sub_operator.kernel_name}, \
event_list:{sub_operator.send_event_list}",
                    )
                super_kernel_file += indent_code_func(sub_operator.notify_block[arch])
        pre_sub_operator = sub_operator
    super_kernel_file += "}\n\n"
    return super_kernel_file


def gen_profling_func_code(super_operator):
    profiling_code = ""
    if (
        super_operator.profiling_mode.value
        == SuperKernelProfilingMode.ProfilingEnable.value
    ):
        profiling_code = """
__BLOCK_LOCAL__ __inline__ uint32_t g_profiling_task_id;
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_profiling_base_addr;
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_profiling_working_addr;
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_profiling_max_addr;
__BLOCK_LOCAL__ __inline__ bool g_profiling_off;
__BLOCK_LOCAL__ __inline__ uint32_t g_percore_size;
constexpr uint64_t PROFILING_MAGIC_NUMBER = 0xbdca8756;
constexpr uint32_t PROFILING_WORKINF_PTR_OFFSET = 8;
constexpr uint32_t PROFILING_MAX_PTR_OFFSET = 16;
constexpr uint32_t ONE_PROFILING_HEAD_SIZE = 16;
constexpr uint32_t ONE_PROFILING_DATA_SIZE = 16;
__aicore__ inline bool ProfilingAreaIsValid()
{
    return (*((__gm__ uint64_t*)g_profiling_base_addr) == PROFILING_MAGIC_NUMBER) &&
        ((*((__gm__ uint64_t*)g_profiling_working_addr)) < (*((__gm__ uint64_t*)g_profiling_max_addr)));
}

__aicore__ inline uint8_t GetProfilingBlockIdx()
{
    if ASCEND_IS_AIV {
        return get_block_idx() * get_subblockdim() + get_subblockid();
    } else {
        return get_block_idx() + 50;
    }
}

__aicore__ inline void RecordProfiling()
{
    if (g_profiling_off) {
        return;
    }
    uint8_t blockIdx = GetProfilingBlockIdx();
    uint64_t workAddr = *((__gm__ uint64_t*)g_profiling_working_addr);
    *((__gm__ uint64_t*)workAddr) = ((uint64_t)g_profiling_task_id << 32) | (((uint64_t)blockIdx) << 8) | 0xff;
    *((__gm__ uint64_t*)workAddr + 1) = static_cast<uint64_t>(AscendC::GetSystemCycle());
    dcci((__gm__ uint64_t*)workAddr, 0, 2);
    *((__gm__ uint64_t*)g_profiling_working_addr) += ONE_PROFILING_DATA_SIZE;
    if (!ProfilingAreaIsValid()) {
        g_profiling_off = true;
    }
    dcci((__gm__ uint64_t*)g_profiling_working_addr, 0, 2);
}

__aicore__ inline void RecordProfiling(uint32_t index, uint8_t profilingType, bool startFlag)
{
    if (g_profiling_off) {
        return;
    }
    uint8_t blockIdx = GetProfilingBlockIdx();
    uint64_t workAddr = *((__gm__ uint64_t*)g_profiling_working_addr);
    if (startFlag) {
        *((__gm__ uint64_t*)workAddr) = ((uint64_t)index << 32) | (((uint64_t)profilingType & 0xf) << 8) | 0x0;
    } else {
        *((__gm__ uint64_t*)workAddr) =
            ((uint64_t)index << 32) | (1 << 12) | (((uint64_t)profilingType & 0xf) << 8) | 0x0;
    }
    *((__gm__ uint64_t*)workAddr + 1) = static_cast<uint64_t>(AscendC::GetSystemCycle());
    dcci((__gm__ uint64_t*)workAddr, 0, 2);
    *((__gm__ uint64_t*)g_profiling_working_addr) += ONE_PROFILING_DATA_SIZE;
    if (!ProfilingAreaIsValid()) {
        g_profiling_off = true;
    }
    dcci((__gm__ uint64_t*)g_profiling_working_addr, 0, 2);
}

__aicore__ inline void InitProfiling(uint32_t taskId, GM_ADDR profilingPtr)
{
    g_profiling_off = false;
    uint8_t blockIdx = GetProfilingBlockIdx();
    g_percore_size = *((__gm__ uint32_t*)(profilingPtr + 12));
    g_profiling_base_addr = profilingPtr + 64 + blockIdx * g_percore_size;
    g_profiling_working_addr = g_profiling_base_addr + PROFILING_WORKINF_PTR_OFFSET;
    g_profiling_max_addr = g_profiling_base_addr + PROFILING_MAX_PTR_OFFSET;
    if (!ProfilingAreaIsValid()) {
        g_profiling_off = true;
        return;
    }
    g_profiling_task_id = taskId;
    RecordProfiling();
}
"""
    return profiling_code


def gen_profiling_start_and_end_record(super_operator, is_start):
    code = ""
    if (
        super_operator.profiling_mode.value
        == SuperKernelProfilingMode.ProfilingEnable.value
    ):
        if is_start:
            code = "RecordProfiling(0, 0, true);\n"
        else:
            code = "RecordProfiling(0, 0, false);\n"
    return code


def gen_2_real_stream_super_kernel_file(super_operator):
    super_kernel_file = ""
    super_kernel_file += gen_file_header(
        super_operator.kernel_type, super_operator.split_mode
    )
    super_kernel_file += gen_profling_func_code(super_operator)
    super_kernel_file += gen_notify_wait_func()
    super_kernel_params = []
    sub_ops = super_operator.info_base
    exits_dynamic_op = False
    for _, sub_operator in enumerate(sub_ops):
        if super_operator.sub_decl_list.get(sub_operator.kernel_name) is None:
            super_kernel_file += sub_operator.kernel_declare
        super_kernel_params += sub_operator.params_before_kernel
        super_kernel_params += sub_operator.kernel_params
        if sub_operator.sub_op_task_type.value == SubOperatorType.DYNAMIC_OP.value:
            if super_operator.sub_decl_list.get(sub_operator.kernel_name) is None:
                super_kernel_file += sub_operator.dynamic_impl_func_block
            super_kernel_params += sub_operator.extra_kernel_params
            exits_dynamic_op = True
        elif sub_operator.sub_op_task_type.value == SubOperatorType.STATIC_OP.value:
            super_kernel_params += sub_operator.extra_kernel_params
        super_operator.sub_decl_list[sub_operator.kernel_name] = "1"

    super_kernel_params_str = ", ".join(
        [f"GM_ADDR {param}" for param in super_kernel_params]
    )
    for sub_ops, arch in zip(
        [super_operator.cub_op_list, super_operator.vec_op_list], ["aic", "aiv"]
    ):
        if len(sub_ops) == 0:
            continue
        super_kernel_file += gen_2_real_stream_code_by_arch(
            super_operator, arch, super_kernel_params_str, exits_dynamic_op, sub_ops
        )

    align_size = super_operator.op_options.get("func-align", 512)
    func_attribute = gen_func_align_attribute(align_size)

    super_kernel_file += f'extern "C"  __global__ {func_attribute} __aicore__ void \
auto_gen_{super_operator.kernel_name}_kernel(void) {{\n'
    super_kernel_file += "    GM_ADDR *param_base = (GM_ADDR *)get_para_base();\n"
    if (
        super_operator.timestamp_option
        or super_operator.feed_sync_all_mode.value
        == SuperKernelFeedSyncAllMode.FeedSyncAllEnable.value
    ):
        ws_offset = len(super_operator.super_kernel_params) + 1
        super_kernel_file += f"    GM_ADDR workspace = param_base[{ws_offset}];\n"
    if (
        super_operator.feed_sync_all_mode.value
        == SuperKernelFeedSyncAllMode.FeedSyncAllEnable.value
    ):
        super_kernel_file += (
            "    AscendC::g_superKernelAutoSyncAllConfigGmBaseAddr = workspace;\n"
        )
    if super_operator.timestamp_option:
        is_mix = super_operator.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
        ]
        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            profiling_offset = ws_offset + 1
            super_kernel_file += (
                f"    GM_ADDR profilingPtr = param_base[{profiling_offset}];\n"
            )
            super_kernel_file += f"    uint32_t taskId = *((__gm__ uint32_t*)(get_para_base() + 8 * {profiling_offset + 1}));\n"
            super_kernel_file += "    InitProfiling(taskId, profilingPtr);\n"
    else:
        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            profiling_offset = len(super_operator.super_kernel_params) + 1
            super_kernel_file += (
                f"    GM_ADDR profilingPtr = param_base[{profiling_offset}];\n"
            )
            super_kernel_file += f"    uint32_t taskId = *((__gm__ uint32_t*)(get_para_base() + 8 * {profiling_offset + 1}));\n"
            super_kernel_file += "    InitProfiling(taskId, profilingPtr);\n"

    super_kernel_file += "    GM_ADDR ffts_addr = param_base[0];\n"
    super_kernel_file += "    if (ffts_addr != nullptr) {\n"
    super_kernel_file += "        set_ffts_base_addr((uint64_t)ffts_addr);\n"
    super_kernel_file += "    }\n\n"
    super_kernel_file += indent_code_func(
        gen_profiling_start_and_end_record(super_operator, True)
    )
    super_kernel_file += indent_code_func(gen_clear_syncall_worskspace(super_operator))
    for sub_ops, arch in zip(
        [super_operator.cub_op_list, super_operator.vec_op_list], ["aic", "aiv"]
    ):
        if len(sub_ops) == 0:
            continue
        super_kernel_file += indent_code_func(f"if ASCEND_IS_{arch.upper()} {{\n")
        super_kernel_file += indent_code_func(
            f"    auto_gen_{super_operator.kernel_name}_kernel_{arch}();\n"
        )
        super_kernel_file += indent_code_func("}\n")

    super_kernel_file += gen_clear_wait_sync_addr_code(super_operator)
    super_kernel_file += indent_code_func(
        gen_profiling_start_and_end_record(super_operator, False)
    )
    super_kernel_file += "}\n\n"

    try:
        with os.fdopen(
            os.open(
                super_operator.kernel_file,
                os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(super_kernel_file)
    except Exception as err:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE, "gen super kernel func file failed, reason is:", err
        )


def judge_need_feed_sync_all(super_operator, sub_op):
    if sub_op.with_sync_all is False:
        return False
    if (
        super_operator.block_num == sub_op.block_num
        and super_operator.kernel_type == sub_op.kernel_type
    ):
        return False
    if super_operator.kernel_type in [
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
    ]:
        if sub_op.block_num < super_operator.block_num:
            return True
    else:
        if sub_op.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
            SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
        ]:
            if sub_op.block_num < super_operator.block_num:
                return True
        elif sub_op.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
        ]:
            if sub_op.block_num < super_operator.block_num * 2:
                return True
    return False


def gen_feed_syncall_var_init_code(super_operator, sub_op):
    code = ""
    if (
        super_operator.feed_sync_all_mode.value
        == SuperKernelFeedSyncAllMode.FeedSyncAllDisable.value
    ):
        return code, False
    sub_op_index = super_operator.info_base.index(sub_op)
    total_op_num = len(super_operator.info_base)
    sync_flag = judge_need_feed_sync_all(super_operator, sub_op)
    if sync_flag is False:
        code += "AscendC::g_superKernelAutoSyncAllEnable = false;\n"
        return code, False
    code += f"""
AscendC::g_superKernelAutoSyncAllSyncIdx = 0;
AscendC::g_superKernelAutoSyncAllEnable = true;
if ASCEND_IS_AIC {{
    AscendC::g_superKernelAutoSyncAllConfigGmAddr = \
AscendC::g_superKernelAutoSyncAllConfigGmBaseAddr + {sub_op_index} * 64;
}}
if ASCEND_IS_AIV {{
    AscendC::g_superKernelAutoSyncAllConfigGmAddr = \
AscendC::g_superKernelAutoSyncAllConfigGmBaseAddr + {total_op_num} * 64 + {sub_op_index} * 64;
}}
"""
    return code, True


def gen_clear_syncall_worskspace(super_operator):
    gen_code = ""
    if (
        super_operator.feed_sync_all_mode.value
        == SuperKernelFeedSyncAllMode.FeedSyncAllDisable.value
    ):
        return gen_code
    if super_operator.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0:
        gen_code += f"""
if ASCEND_IS_AIC {{
    uint32_t sizePerCore = {super_operator.workspace_size} / get_block_num();
    const uint32_t repeatTimes = sizePerCore / 512;
    __gm__ uint8_t* startAddr  = (__gm__ uint8_t*)(workspace + sizePerCore * AscendC::GetBlockIdxImpl());
    create_cbuf_matrix((__cbuf__ uint32_t*)(0), 0x10010, 0);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
    for (size_t i = 0; i < repeatTimes; i++) {{
        copy_cbuf_to_gm((__gm__ void*)(startAddr), (__cbuf__ void*)(0), 0, 1, 16, 1, 1);
        startAddr += 512;
    }}
    AscendC::PipeBarrier<PIPE_ALL>();
    ffts_cross_core_sync(PIPE_FIX, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
    {get_wait_flag_for_chip("AscendC::SYNC_AIC_FLAG")}
}}
"""
    elif super_operator.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0:
        gen_code += f"""
if ASCEND_IS_AIV {{
    uint32_t sizePerCore = {super_operator.workspace_size} / get_block_num();
    const uint32_t repeatTimes = sizePerCore / 512;
    __gm__ uint8_t* startAddr  = (__gm__ uint8_t*)(workspace + sizePerCore * AscendC::GetBlockIdxImpl());
    AscendC::DuplicateImpl((__ubuf__ uint32_t*)(0), (uint32_t)0, 128);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    for (size_t i = 0; i < repeatTimes; i++) {{
        copy_ubuf_to_gm((__gm__ void*)(startAddr), (__ubuf__ void*)(0), 0, 1, 16, 1, 1);
        startAddr += 512;
    }}
    AscendC::PipeBarrier<PIPE_ALL>();
    ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
    {get_wait_flag_for_chip("AscendC::SYNC_AIV_ONLY_ALL")}
}}
"""
    else:
        if super_operator.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1:
            workspace_size = int(super_operator.workspace_size)
        else:
            workspace_size = int(super_operator.workspace_size / 2)
        if CommonUtility.is_c310():
            gen_code += f"""
if ASCEND_IS_AIV {{
    uint32_t sizePerCore = {workspace_size} / get_block_num();
    const uint32_t repeatTimes = sizePerCore / 512;
    __gm__ uint8_t* startAddr  = (__gm__ uint8_t*)(workspace + sizePerCore * AscendC::GetBlockIdxImpl());
    AscendC::DuplicateImpl((__ubuf__ uint32_t*)(0), (uint32_t)0, 128);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    for (size_t i = 0; i < repeatTimes; i++) {{
        copy_ubuf_to_gm_align_v2((__gm__ void*)(startAddr), (__ubuf__ void*)(0), 0, 1, 512, 0, 512, 512);
        startAddr += 512;
    }}
    AscendC::PipeBarrier<PIPE_ALL>();
    ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
    {get_wait_flag_for_chip("AscendC::SYNC_AIV_ONLY_ALL")}
    ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
}}

if ASCEND_IS_AIC {{
    {get_wait_flag_for_chip("AscendC::SYNC_AIV_FLAG")}
}}
"""
        else:
            gen_code += f"""
if ASCEND_IS_AIV {{
    uint32_t sizePerCore = {workspace_size} / get_block_num();
    const uint32_t repeatTimes = sizePerCore / 512;
    __gm__ uint8_t* startAddr  = (__gm__ uint8_t*)(workspace + sizePerCore * AscendC::GetBlockIdxImpl());
    AscendC::DuplicateImpl((__ubuf__ uint32_t*)(0), (uint32_t)0, 128);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    for (size_t i = 0; i < repeatTimes; i++) {{
        copy_ubuf_to_gm((__gm__ void*)(startAddr), (__ubuf__ void*)(0), 0, 1, 16, 1, 1);
        startAddr += 512;
    }}
    AscendC::PipeBarrier<PIPE_ALL>();
    ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
    {get_wait_flag_for_chip("AscendC::SYNC_AIV_ONLY_ALL")}
    ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
}}

if ASCEND_IS_AIC {{
   {get_wait_flag_for_chip("AscendC::SYNC_AIV_FLAG")}
}}
"""
    return gen_code


def gen_wait_block_extra_sync(super_operator, pre_sub_operator, sub_operator):
    pre_sub_operator_device_type = kernel_meta_type_to_device_type(
        pre_sub_operator.kernel_type
    )
    sub_operator_device_type = kernel_meta_type_to_device_type(sub_operator.kernel_type)

    extra_sync = ""
    # When wait block runs on aiv block 0 and inter op barrier does not contain aiv only syncall,
    # extra aiv syncall will be needed to ensure next op runs after wait block finishes.
    extra_aiv_sync_pairs = {
        (
            SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIC.value,
            SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIV.value,
        ),
        (
            SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIC.value,
            SuperKernelDeviceType.KERNEL_DEVICE_TYPE_MIX.value,
        ),
    }

    # When wait block runs on aic block 0 and inter op barrier does not contain aic only syncall,
    # extra aic syncall will be needed to ensure next op runs after wait block finishes.
    extra_aic_sync_pairs = {
        (
            SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIV.value,
            SuperKernelDeviceType.KERNEL_DEVICE_TYPE_AIC.value,
        )
    }

    if (pre_sub_operator_device_type, sub_operator_device_type) in extra_aiv_sync_pairs:
        extra_sync += "// extra sync for wait event\n"
        extra_sync += "AscendC::SyncAll<true>();\n\n"
    elif (
        pre_sub_operator_device_type,
        sub_operator_device_type,
    ) in extra_aic_sync_pairs:
        extra_sync += f"""
// extra sync for wait event
ffts_cross_core_sync(PIPE_FIX, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
{get_wait_flag_for_chip("AscendC::SYNC_AIC_FLAG")}
"""

    return extra_sync


def gen_sync_and_event_code(super_operator, pre_sub_operator, sub_operator):
    sync_and_event_code = ""
    if (
        len(sub_operator.recv_event_list) != 0
        and len(pre_sub_operator.send_event_list) != 0
    ):
        sync_and_event_code += indent_code_func(
            gen_inter_ops_barrier(super_operator, pre_sub_operator, sub_operator)
        )
        sync_and_event_code += indent_code_func(pre_sub_operator.notify_block)
        if len(sub_operator.wait_block) != 0:
            sync_and_event_code += indent_code_func(sub_operator.wait_block)
            # add sync with sub_operator and sub_operator
            sync_and_event_code += "// reason3: for continues notify/wait event\n"
            sync_and_event_code += indent_code_func(
                get_sync_code_by_kernel_type(super_operator.kernel_type)
            )
    else:
        if len(sub_operator.recv_event_list) != 0:
            sync_and_event_code += indent_code_func(sub_operator.wait_block)
            sync_and_event_code += indent_code_func(
                gen_wait_block_extra_sync(
                    super_operator, pre_sub_operator, sub_operator
                )
            )
        sync_and_event_code += indent_code_func(
            gen_inter_ops_barrier(super_operator, pre_sub_operator, sub_operator)
        )
        if len(pre_sub_operator.send_event_list) != 0:
            sync_and_event_code += indent_code_func(pre_sub_operator.notify_block)
    return sync_and_event_code


def gen_super_kernel_file(super_operator):
    if super_operator.enable_double_stream:
        gen_2_real_stream_super_kernel_file(super_operator)
        return
    super_kernel_file = ""
    super_kernel_file += gen_file_header(
        super_operator.kernel_type, super_operator.split_mode
    )
    super_kernel_file += gen_profling_func_code(super_operator)
    super_kernel_file += gen_notify_wait_func()
    sub_ops = super_operator.info_base
    exits_dynamic_op = False
    for _, sub_operator in enumerate(sub_ops):
        if super_operator.sub_decl_list.get(sub_operator.kernel_name) is None:
            super_kernel_file += sub_operator.kernel_declare
        if sub_operator.sub_op_task_type.value == SubOperatorType.DYNAMIC_OP.value:
            if super_operator.sub_decl_list.get(sub_operator.kernel_name) is None:
                super_kernel_file += sub_operator.dynamic_impl_func_block
            exits_dynamic_op = True
        super_operator.sub_decl_list[sub_operator.kernel_name] = "1"

    align_size = super_operator.op_options.get("func-align", 512)
    func_attribute = gen_func_align_attribute(align_size)
    super_kernel_file += f'extern "C"  __global__ {func_attribute} __aicore__ void \
auto_gen_{super_operator.kernel_name}_kernel(void) {{\n'
    super_kernel_file += "    GM_ADDR *param_base = (GM_ADDR *)get_para_base();\n"
    if (
        super_operator.timestamp_option
        or super_operator.feed_sync_all_mode.value
        == SuperKernelFeedSyncAllMode.FeedSyncAllEnable.value
    ):
        if CommonUtility.is_c310():
            ws_offset = len(super_operator.super_kernel_params)
        else:
            ws_offset = len(super_operator.super_kernel_params) + 1
        super_kernel_file += f"    GM_ADDR workspace = param_base[{ws_offset}];\n"
    if (
        super_operator.feed_sync_all_mode.value
        == SuperKernelFeedSyncAllMode.FeedSyncAllEnable.value
    ):
        super_kernel_file += (
            "    AscendC::g_superKernelAutoSyncAllConfigGmBaseAddr = workspace;\n"
        )
    if super_operator.timestamp_option:
        is_mix = super_operator.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
        ]
        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            profiling_offset = ws_offset + 1
            super_kernel_file += (
                f"    GM_ADDR profilingPtr = param_base[{profiling_offset}];\n"
            )
            super_kernel_file += f"    uint32_t taskId = *((__gm__ uint32_t*)(get_para_base() + 8 * {profiling_offset + 1}));\n"
            super_kernel_file += "    InitProfiling(taskId, profilingPtr);\n"
    else:
        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            profiling_offset = len(super_operator.super_kernel_params) + 1
            super_kernel_file += (
                f"    GM_ADDR profilingPtr = param_base[{profiling_offset}];\n"
            )
            super_kernel_file += f"    uint32_t taskId = *((__gm__ uint32_t*)(get_para_base() + 8 * {profiling_offset + 1}));\n"
            super_kernel_file += "    InitProfiling(taskId, profilingPtr);\n"

    if not CommonUtility.is_c310():
        super_kernel_file += "    GM_ADDR ffts_addr = param_base[0];\n"
        super_kernel_file += "    if (ffts_addr != nullptr) {\n"
        super_kernel_file += "        set_ffts_base_addr((uint64_t)ffts_addr);\n"
        super_kernel_file += "    }\n\n"
    super_kernel_file += indent_code_func(gen_clear_syncall_worskspace(super_operator))
    if exits_dynamic_op is True:
        super_kernel_file += "    uint64_t aiv_func_addr = 0;\n"
        super_kernel_file += "    uint64_t aic_func_addr = 0;\n"
        super_kernel_file += "    uint64_t dy_blockNum = 0;\n"
        if super_operator.split_mode > 1:
            for i in range(1, super_operator.split_mode):
                super_kernel_file += f"    uint64_t aiv_func_addr_split{i} = 0;\n"
                super_kernel_file += f"    uint64_t aic_func_addr_split{i} = 0;\n"

    if super_operator.preload_mode.value == SuperKernelPreLoadMode.PreLoadByWhole.value:
        super_kernel_file += indent_code_func("AscendC::PreLoad(8);\n")
    super_kernel_file += indent_code_func(
        gen_profiling_start_and_end_record(super_operator, True)
    )
    for pre_sub_operator, sub_operator, next_sub_operator in zip(
        [None] + sub_ops[:-1], sub_ops, sub_ops[1:] + [None]
    ):
        super_kernel_file += indent_code_func(
            f"//begin func call of sub operator {sub_operator.kernel_name}\n"
        )
        super_kernel_file += indent_code_func(
            sub_operator.get_notify_before_call_block()
        )

        # generatre switch case func of dynamic
        super_kernel_file += gen_switch_case_call_block_of_dynamic_op(
            super_operator, next_sub_operator, sub_operator, pre_sub_operator
        )

        # add preload of current func
        if (
            super_operator.preload_mode.value
            == SuperKernelPreLoadMode.PreLoadStepByStep.value
        ):
            super_kernel_file += indent_code_func(sub_operator.preload_call_block)

        # add preload of next func, when n+1 preload instr
        if (
            super_operator.preload_mode.value
            == SuperKernelPreLoadMode.PreloadByAdanvanceStep.value
        ):
            if pre_sub_operator is None:
                super_kernel_file += indent_code_func(sub_operator.preload_call_block)
            if next_sub_operator is not None:
                super_kernel_file += indent_code_func(
                    next_sub_operator.preload_call_block
                )

        if (
            super_operator.datacache_mode.value
            == SuperKernelDataCacheMode.DataCacheLoadAdancanceStep.value
        ):
            if pre_sub_operator is None:
                super_kernel_file += indent_code_func(
                    sub_operator.data_cache_preload_call
                )
            if next_sub_operator is not None:
                super_kernel_file += indent_code_func(
                    next_sub_operator.data_cache_preload_call
                )
            super_kernel_file += "\n"

        if pre_sub_operator is None and len(sub_operator.recv_event_list) != 0:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"first op of super kernel must \
not have any recv event, op:{sub_operator.kernel_name}, event_list:{sub_operator.recv_event_list}",
            )

        # gen sync/notify/wait between operators
        if pre_sub_operator is not None:
            super_kernel_file += gen_sync_and_event_code(
                super_operator, pre_sub_operator, sub_operator
            )

        tmp_code, enable_syncall_flag = gen_feed_syncall_var_init_code(
            super_operator, sub_operator
        )
        super_kernel_file += indent_code_func(tmp_code)
        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            super_kernel_file += indent_code_func(
                f"RecordProfiling({super_operator.info_base.index(sub_operator) + 1}, 0x8, true);\n"
            )
        if enable_syncall_flag is False:
            super_kernel_file += indent_code_func(sub_operator.kernel_call_block)
        else:
            super_kernel_file += indent_code_func(
                sub_operator.kernel_call_block_with_syncall
            )
        super_kernel_file += indent_code_func(gen_op_end_debug_dcci_all(super_operator))
        super_kernel_file += indent_code_func(gen_op_end_debug_sync_all(super_operator))

        if (
            super_operator.profiling_mode.value
            == SuperKernelProfilingMode.ProfilingEnable.value
        ):
            super_kernel_file += indent_code_func(
                f"RecordProfiling({super_operator.info_base.index(sub_operator) + 1}, 0x8, false);\n"
            )

        if next_sub_operator is None and len(sub_operator.send_event_list) != 0:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"last op of super kernel must \
not have any send event, op:{sub_operator.kernel_name}, event_list:{sub_operator.send_event_list}",
            )
        pre_sub_operator = sub_operator

    super_kernel_file += gen_clear_wait_sync_addr_code(super_operator)

    super_kernel_file += indent_code_func(
        gen_profiling_start_and_end_record(super_operator, False)
    )

    super_kernel_file += "}\n\n"
    try:
        with os.fdopen(
            os.open(
                super_operator.kernel_file,
                os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(super_kernel_file)
    except Exception as err:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE, ("gen super kernel func file failed, reason is:", err)
        )


def compile(
    kernel_infos, called_kernel_name="ascendc_super_kernel_plus", compile_infos=None
):
    """entry of super kernel compile

    Args:
        kernel_infos: infos of sub kernel
            {
                "op_list":
                    [{"op1": {"bin_path": "", "json_path": ""}, "op2": {xxx}}],
                "super_kernel_options": compile_option
            }
        called_kernel_name: super kernel name
    """
    # global_var_storage must be reset before every entry of compile
    global_var_storage.global_storage_reset()
    if not CommonUtility.is_support_super_kernel():
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            f"current soc: {global_var_storage.get_variable('ascendc_short_soc_version')} "
            f"series do not support super kernel feature",
        )

    if compile_infos is not None:
        CommonUtility.print_compile_log(
            "[SuperKernel]", f"compile_infos: {compile_infos}", AscendCLogLevel.LOG_INFO
        )

    kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
    if os.path.exists(os.path.join(kernel_meta_dir, called_kernel_name + ".o")):
        return

    if kernel_infos.get("op_list", "") == "":
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE, ("super kernel compile must provide op lists")
        )
    super_operator = SuperOperatorInfos(kernel_infos, called_kernel_name)
    gen_super_kernel_file(super_operator)
    compile_super_kernel(super_operator.compile_info, super_operator.compile_log_path)
    return
