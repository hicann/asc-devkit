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
super kernel sub op infos
"""

import os
import json
import re
import subprocess
import math
import threading
from contextlib import contextmanager
from .super_kernel_utility import (
    get_soc_spec,
    CommonUtility,
    AscendCLogLevel,
    CompileStage,
)
from .super_kernel_constants import (
    SuperKernelEarlyStartMode,
    SubOperatorType,
    STR_TO_SUPER_TASK_TYPE,
    SuperKernelFeedSyncAllMode,
    SuperKernelProfilingMode,
    ERR_CODE,
    SuperKernelKernelType,
    STR_TO_SK_KERNEL_TYPE,
)


def indent_code_func(code: str, indent: str = "    "):
    """Indent non-empty lines."""
    # use `(?=)' for lookahead assertion, see re module document.
    return re.sub(r"^(?=.+)", indent, code, flags=re.MULTILINE)


@contextmanager
def change_dir(path):
    original_dir = os.getcwd()
    try:
        os.chdir(path)
        yield
    finally:
        os.chdir(original_dir)


class SubOperatorInfos:
    def __init__(
        self, index, info_dict, stream_index: int, op_options, compile_log_path=None
    ):
        keys_list = list(info_dict.keys())
        self.json_path: str = info_dict["json_path"]
        self.bin_path: list = info_dict["bin_path"]
        self.compile_log_path: str = compile_log_path
        self.start_block_idx = 0
        self.stream_index = stream_index  # true stream id
        self.sub_op_task_type: SubOperatorType = STR_TO_SUPER_TASK_TYPE[
            info_dict.get("task_type", "normal")
        ]
        self.index: int = index
        self.kernel_name: str = ""
        # stream fusion option will use kernel name for sync instr optiomization,
        # kernel name will be the same when fusing multi layer in one sk, separate them by index
        self.kernel_name_for_multi_stream: str = ""
        self.notify_before_call_event_list = info_dict.get(
            "notify_before_call_event_list", []
        )
        self.send_event_list = info_dict.get("send_event_list", [])
        self.recv_event_list = info_dict.get("recv_event_list", [])
        self.send_info: dict = {}
        self.recv_info: dict = {}
        self.called_kernel_name: dict = None
        self.origin_kernel_type_str: str = ""
        self.kernel_type: SuperKernelKernelType = ""
        self.block_num: int = 0
        self.timestamp_option: bool = False
        self.debug_size: int = 0
        self.debug_option: str = ""
        self.kernel_params: list = None
        self.kernel_declare: str = ""
        self.kernel_call_block: str = ""
        self.kernel_call_block_with_syncall: str = ""
        self.preload_call_block: str = ""
        self.early_start_complement_set_flag_block: str = ""
        self.early_start_complement_wait_flag_block: str = ""
        self.early_start_mode: SuperKernelEarlyStartMode = op_options.get(
            "early-start", SuperKernelEarlyStartMode.EarlyStartEnableV2
        )
        self.feed_sync_all_mode: SuperKernelFeedSyncAllMode = op_options.get(
            "feed-sync-all", SuperKernelFeedSyncAllMode.FeedSyncAllDisable
        )

        self.profiling_mode = op_options.get(
            "profiling", SuperKernelProfilingMode.ProfilingDisable
        )
        self.early_start_set_flag: bool = False
        self.early_start_wait_flag: bool = False
        self.aiv_bin: str = None
        self.aic_bin: str = None
        self.dynamic_bin: str = None
        self.split_mode_in_json: int = None
        self.aiv_text_len: int = 0
        self.aic_text_len: int = 0
        self.data_cache_preload_call: str = ""
        self.sub_kernel_names: list = []
        self.split_mode = op_options.get("split-mode", 4)
        self.call_dcci_before_kernel_start: bool = False
        self.call_dcci_after_kernel_end: bool = False
        self.call_dcci_disable_on_kernel: bool = False
        # For dynamic operators, decide whether to add DCCI options during the final superkernel compilation
        self.dcci_before_kernel_start_op_list = [
            part_op.strip()
            for part_op in op_options.get("dcci-before-kernel-start", "").split(",")
            if part_op.strip()
        ]
        self.dcci_after_kernel_end_op_list = [
            part_op.strip()
            for part_op in op_options.get("dcci-after-kernel-end", "").split(",")
            if part_op.strip()
        ]
        self.dcci_disable_on_kernel_op_list = [
            part_op.strip()
            for part_op in op_options.get("dcci-disable-on-kernel", "").split(",")
            if part_op.strip()
        ]
        # code_gen of dynamic op
        self._gen_code_for_dynamic_op()

    def _gen_code_for_dynamic_op(self):
        self.call_dynamic_switch_func: str = ""
        self.dynamic_impl_func_block: str = ""
        self.params_before_kernel: list = []
        self.extra_kernel_params: list = []
        self.switch_func_called_flag: bool = False
        self.wait_block = {}
        self.notify_before_call_block = ""
        self.notify_block = {}
        self.tmp_notify_block = {}
        self.is_last_op: bool = False
        self.param_offset = 0
        self.notify_param_offset = 0
        self.notify_before_call_param_offset = 0
        self.wait_param_offset = 0
        self.with_sync_all: bool = False

    @staticmethod
    def gen_dcci_all_block():
        return "dcci((__gm__ uint64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);\n\n"

    def gen_dcci_before_kernel_start_call_block(self):
        dcci_call_block = ""
        if not self.call_dcci_disable_on_kernel and self.call_dcci_before_kernel_start:
            dcci_call_block += "// option: dcci-before-kernel-start\n"
            dcci_call_block += self.gen_dcci_all_block()
        return dcci_call_block

    def gen_dcci_after_kernel_end_call_block(self):
        dcci_call_block = ""
        if not self.call_dcci_disable_on_kernel and self.call_dcci_after_kernel_end:
            dcci_call_block += "// option: dcci-after-kernel-end\n"
            dcci_call_block += self.gen_dcci_all_block()
        return dcci_call_block

    def gen_profiling_for_notify(self, index, end_flag):
        code = ""
        if self.profiling_mode.value == SuperKernelProfilingMode.ProfilingDisable.value:
            return code
        if end_flag is False:
            code = f"RecordProfiling({index}, 0x4, true);\n"
        else:
            code = f"RecordProfiling({index}, 0x4, false);\n"
        return code

    def gen_profiling_for_wait(self, index, end_flag):
        code = ""
        if self.profiling_mode.value == SuperKernelProfilingMode.ProfilingDisable.value:
            return code
        if end_flag is False:
            code = f"RecordProfiling({index}, 12, true);\n"
        else:
            code = f"RecordProfiling({index}, 12, false);\n"
        return code

    def _is_aic_kernel_type(self):
        return self.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
        ]

    def _gen_notify_block_for_core(
        self, inner_event_id_set, is_aic, event_list=None, notify_param_offset=None
    ):
        if event_list is None:
            event_list = self.send_event_list
        if notify_param_offset is None:
            notify_param_offset = self.notify_param_offset
        notify_block = "if ASCEND_IS_AIC {\n" if is_aic else "if ASCEND_IS_AIV {\n"
        found = False
        for index, send_index in enumerate(event_list):
            if send_index not in inner_event_id_set:
                CommonUtility.dump_compile_log(
                    ["###Notify:", self.kernel_name, event_list[index]],
                    CompileStage.SPLIT_SUB_OBJS,
                    self.compile_log_path,
                )
                param_offset = notify_param_offset + index
                notify_block += (
                    f"    // kernel={self.kernel_name}, ev={event_list[index]}, \
param_offset={param_offset}\n"
                )
                notify_block += self.gen_profiling_for_notify(send_index, False)
                notify_block += f"    NotifyFunc<{str(is_aic).lower()}>("
                notify_block += f"param_base[{param_offset}]);\n"
                notify_block += self.gen_profiling_for_notify(send_index, True)
                found = True
        notify_block += "}\n"
        return notify_block, found

    def gen_notify_before_call(self, enable_double_stream):
        if len(self.notify_before_call_event_list) == 0:
            self.notify_before_call_block = (
                {"aic": "", "aiv": ""} if enable_double_stream else ""
            )
            return

        notify_block_aic, found_aic = self._gen_notify_block_for_core(
            set(),
            True,
            self.notify_before_call_event_list,
            self.notify_before_call_param_offset,
        )
        notify_block_aiv, found_aiv = self._gen_notify_block_for_core(
            set(),
            False,
            self.notify_before_call_event_list,
            self.notify_before_call_param_offset,
        )
        if enable_double_stream:
            self.notify_before_call_block = {"aic": "", "aiv": ""}
            if self._is_aic_kernel_type():
                self.notify_before_call_block["aic"] = (
                    notify_block_aic if found_aic else ""
                )
            else:
                self.notify_before_call_block["aiv"] = (
                    notify_block_aiv if found_aiv else ""
                )
        elif self._is_aic_kernel_type():
            self.notify_before_call_block = notify_block_aic if found_aic else ""
        else:
            self.notify_before_call_block = notify_block_aiv if found_aiv else ""

    def get_notify_before_call_block(self, arch=None):
        if isinstance(self.notify_before_call_block, dict):
            return self.notify_before_call_block.get(arch, "")
        return self.notify_before_call_block

    def _set_notify_block_single_stream(
        self, notify_block_aic, notify_block_aiv, found_aic, found_aiv
    ):
        if self._is_aic_kernel_type():
            self.notify_block = notify_block_aic if found_aic else ""
        else:
            self.notify_block = notify_block_aiv if found_aiv else ""

    def _set_notify_block_double_stream(
        self, notify_block_aic, notify_block_aiv, found_aic, found_aiv
    ):
        if self._is_aic_kernel_type():
            self.notify_block["aic"] = notify_block_aic if found_aic else ""
            self.notify_block["aiv"] = ""
        else:
            self.notify_block["aiv"] = notify_block_aiv if found_aiv else ""
            self.notify_block["aic"] = ""
            self.tmp_notify_block["aiv"] = ""
            self.tmp_notify_block["aic"] = notify_block_aic if found_aic else ""

    def gen_notify_from_outside(self, inner_event_id_set, enable_double_stream):
        if len(self.send_event_list) == 0:
            return

        notify_block_aic, found_aic = self._gen_notify_block_for_core(
            inner_event_id_set, True
        )
        notify_block_aiv, found_aiv = self._gen_notify_block_for_core(
            inner_event_id_set, False
        )

        if enable_double_stream:
            self._set_notify_block_double_stream(
                notify_block_aic, notify_block_aiv, found_aic, found_aiv
            )
        else:
            self._set_notify_block_single_stream(
                notify_block_aic, notify_block_aiv, found_aic, found_aiv
            )

    def gen_wait_from_outside(self, inner_event_id_set, enable_double_stream):
        if len(self.recv_event_list) != 0:
            found = False
            if self.kernel_type in [
                SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
            ]:
                wait_block = "if ASCEND_IS_AIC {\n"
                index = 0
                for recv_index in self.recv_event_list:
                    if recv_index not in inner_event_id_set:
                        CommonUtility.dump_compile_log(
                            ["###Wait:", self.kernel_name, self.recv_event_list[index]],
                            CompileStage.SPLIT_SUB_OBJS,
                            self.compile_log_path,
                        )
                        wait_block += f"    // kernel={self.kernel_name}, ev={self.recv_event_list[index]}, \
param_offset={self.wait_param_offset + index}\n"
                        wait_block += self.gen_profiling_for_wait(recv_index, False)
                        wait_block += f"    WaitFunc<true>(param_base[{self.wait_param_offset + index}]);\n"
                        wait_block += self.gen_profiling_for_wait(recv_index, True)
                        found = True
                    index += 1
                wait_block += "}\n"
                self.wait_block = wait_block if found else ""
            else:
                wait_block = "if ASCEND_IS_AIV {\n"
                index = 0
                for recv_index in self.recv_event_list:
                    if recv_index not in inner_event_id_set:
                        CommonUtility.dump_compile_log(
                            ["###Wait:", self.kernel_name, self.recv_event_list[index]],
                            CompileStage.SPLIT_SUB_OBJS,
                            self.compile_log_path,
                        )
                        wait_block += f"    // kernel={self.kernel_name}, ev={self.recv_event_list[index]}, \
param_offset={self.wait_param_offset + index}\n"
                        wait_block += self.gen_profiling_for_wait(recv_index, False)
                        wait_block += f"    WaitFunc<false>(param_base[{self.wait_param_offset + index}]);\n"
                        wait_block += self.gen_profiling_for_wait(recv_index, True)
                        found = True
                    index += 1
                wait_block += "}\n"
                self.wait_block = wait_block if found else ""

    def gen_notify_wait_from_outside(self, inner_event_id_set, enable_double_stream):
        notify_before_call_event_num = len(self.notify_before_call_event_list)
        send_event_num = len(self.send_event_list)
        notify_before_call_params = [
            f"__ac_notify_lock_{self.index}_{index}"
            for index in range(notify_before_call_event_num)
        ]
        notify_param_end = notify_before_call_event_num + send_event_num
        notify_params = [
            f"__ac_notify_lock_{self.index}_{index}"
            for index in range(notify_before_call_event_num, notify_param_end)
        ]
        if notify_before_call_event_num:
            self.notify_before_call_param_offset = (
                self.param_offset - notify_before_call_event_num
            )
        self.params_before_kernel += notify_before_call_params
        self.notify_param_offset = (
            self.param_offset + len(self.kernel_params) + len(self.extra_kernel_params)
        )
        self.wait_param_offset = self.notify_param_offset + send_event_num
        self.extra_kernel_params += notify_params
        self.extra_kernel_params += [
            f"__ac_wait_lock_{self.index}_{index}"
            for index in range(len(self.recv_event_list))
        ]
        self.gen_notify_before_call(enable_double_stream)
        self.gen_notify_from_outside(inner_event_id_set, enable_double_stream)
        self.gen_wait_from_outside(inner_event_id_set, enable_double_stream)

    def code_gen(self, inner_event_id_set, enable_double_stream):
        if self.sub_op_task_type.value == SubOperatorType.DYNAMIC_OP.value:
            self.process_of_dynamic_op(enable_double_stream)
        else:
            self.extract_sub_op_bin_files()
            self.gen_sub_kernel_declare_and_call_func()
        self.gen_notify_wait_from_outside(inner_event_id_set, enable_double_stream)

    def adjust_dynamic_op(self, spk_block_num):
        if self.sub_op_task_type.value == SubOperatorType.DYNAMIC_OP.value:
            self.dynamic_impl_func_block = self.dynamic_impl_func_block.replace(
                "__placehoder__spk_block_num__", f"{spk_block_num}"
            )

    def update_dynamic_op_dcci_options(self):
        if self.sub_op_task_type.value != SubOperatorType.DYNAMIC_OP.value:
            return

        if self.sub_operator_op_type in self.dcci_disable_on_kernel_op_list:
            self.call_dcci_before_kernel_start = False
            self.call_dcci_after_kernel_end = False
            self.call_dcci_disable_on_kernel = True
        else:
            self.call_dcci_before_kernel_start = (
                self.sub_operator_op_type in self.dcci_before_kernel_start_op_list
            )
            self.call_dcci_after_kernel_end = (
                self.sub_operator_op_type in self.dcci_after_kernel_end_op_list
            )

    def init_of_sub_operator_info(self):
        try:
            with open(self.json_path, "r") as fd:
                sub_operater_infos = json.load(fd)
                self.kernel_name: str = sub_operater_infos["kernelName"]
                self.kernel_name_for_multi_stream: str = (
                    self.kernel_name + "_" + str(self.index)
                )
                self.called_kernel_name: dict = sub_operater_infos[
                    "sub_operator_kernel_name"
                ]
                self.split_mode_in_json = sub_operater_infos.get("split_mode")
                self.origin_kernel_type_str: str = sub_operater_infos[
                    "sub_operator_kernel_type"
                ]
                self.kernel_type: SuperKernelKernelType = STR_TO_SK_KERNEL_TYPE[
                    self.origin_kernel_type_str
                ]
                self.block_num: int = sub_operater_infos["blockDim"]
                self.timestamp_option: bool = (
                    "timestamp" in sub_operater_infos.get("debugOptions", "")
                    or "printf" in sub_operater_infos.get("debugOptions", "")
                    or "assert" in sub_operater_infos.get("debugOptions", "")
                )
                self.with_sync_all: bool = sub_operater_infos.get(
                    "sub_op_with_sync_all", False
                )
                # disable timestamp until GE support printf assert
                if self.timestamp_option == True:
                    self.debug_option: str = sub_operater_infos["debugOptions"]
                    self.debug_size: int = sub_operater_infos["debugBufSize"]
                self.sub_operator_op_type = sub_operater_infos.get(
                    "sub_operator_op_type", ""
                )
                self.kernel_params: list = [
                    param + f"_{self.index}"
                    for param in sub_operater_infos["sub_operator_params"]
                ]
                self.early_start_set_flag = sub_operater_infos[
                    "sub_operator_early_start_set_flag"
                ]
                self.early_start_wait_flag = sub_operater_infos[
                    "sub_operator_early_start_wait_flag"
                ]
                self.call_dcci_before_kernel_start = sub_operater_infos.get(
                    "sub_operator_call_dcci_before_kernel_start", False
                )
                self.call_dcci_after_kernel_end = sub_operater_infos.get(
                    "sub_operator_call_dcci_after_kernel_end", False
                )
                self.call_dcci_disable_on_kernel = sub_operater_infos.get(
                    "sub_operator_call_dcci_disable_on_kernel", False
                )
                self.update_dynamic_op_dcci_options()
                if (
                    self.early_start_mode.value
                    == SuperKernelEarlyStartMode.EarlyStartDisable.value
                    and (self.early_start_set_flag or self.early_start_wait_flag)
                ):
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        (
                            f"sub operator {self.kernel_name} early-start mode set:{self.early_start_set_flag}, \
wait:{self.early_start_wait_flag}, donot match with super kernel early-start mode: False, please check whether sub\
operator inherits super kernel opton."
                        ),
                    )
                if (
                    self.split_mode_in_json is not None
                    and self.split_mode_in_json != self.split_mode
                ):
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        (
                            f"sub operator {self.kernel_name} split_mode: {self.split_mode_in_json}, \
donot match with super kernel split_mode: {self.split_mode} please check whether sub\
operator inherits super kernel opton."
                        ),
                    )
        except Exception as err:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                (
                    f"read sub op json file failed, json name {self.json_path}, reason is:",
                    err,
                ),
            )

    def gen_select_addr_code(self, func_addr_str, aicore_kernel_name):
        result = f"{func_addr_str} = (uint64_t)({aicore_kernel_name});"
        if self.split_mode > 1:
            for i in range(1, self.split_mode):
                result += f"\n    {func_addr_str}_split{i} = (uint64_t)({aicore_kernel_name}_split{i});"
        return result

    def gen_switch_case_block_of_dynamic_op(
        self, kernel_info_of_tiling_key, tiling_key, kernel_type
    ):
        chip_version = CommonUtility.get_chip_version()
        params_with_type = ", ".join(
            [f"GM_ADDR {param}" for param in self.kernel_params]
        )
        if kernel_type is SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY:
            aicore_kernel_name = kernel_info_of_tiling_key["AiCore"]
            case_block = f"""
{self.gen_select_addr_code("aiv_func_addr", aicore_kernel_name)}
dy_block_num = ((uint64_t){kernel_type.value}) << 32 | (*blockNumAddr);
"""
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
        elif kernel_type is SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY:
            aicore_kernel_name = kernel_info_of_tiling_key["AiCore"]
            case_block = f"""
{self.gen_select_addr_code("aic_func_addr", aicore_kernel_name)}
dy_block_num = ((uint64_t){kernel_type.value}) << 32 | (*blockNumAddr);
"""
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
        elif kernel_type is SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0:
            aicore_kernel_name = kernel_info_of_tiling_key[f"dav-{chip_version}-vec"]
            case_block = f"""
{self.gen_select_addr_code("aiv_func_addr", aicore_kernel_name)}
dy_block_num = ((uint64_t){kernel_type.value}) << 32 | (*blockNumAddr);
"""
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
        elif kernel_type is SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0:
            aicore_kernel_name = kernel_info_of_tiling_key[f"dav-{chip_version}-cube"]
            case_block = f"""
{self.gen_select_addr_code("aic_func_addr", aicore_kernel_name)}
dy_block_num = ((uint64_t){kernel_type.value}) << 32 | (*blockNumAddr);
"""
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
        elif kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
        ]:
            aiv_kernel_name = kernel_info_of_tiling_key[f"dav-{chip_version}-vec"]
            aic_kernel_name = kernel_info_of_tiling_key[f"dav-{chip_version}-cube"]
            case_block = f"""
{self.gen_select_addr_code("aiv_func_addr", aiv_kernel_name)}
{self.gen_select_addr_code("aic_func_addr", aic_kernel_name)}
dy_block_num = ((uint64_t){kernel_type.value}) << 32 | (*blockNumAddr);
"""
            self.sub_kernel_names.append(aiv_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aiv_kernel_name, params_with_type
            )
            self.sub_kernel_names.append(aic_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aic_kernel_name, params_with_type
            )
        else:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE, (f"kernel type {kernel_type} do not support!")
            )
        return case_block

    def gen_param_code(self, param_str):
        result = param_str
        if self.split_mode > 1:
            for i in range(1, self.split_mode):
                result += f", {param_str}_split{i}"
        return result

    def gen_binary_search_block(self, input_blocks):
        if len(input_blocks) == 1:
            block_str = f"""if (*tilingKeyAddr == {input_blocks[0][1]}) {{
{indent_code_func(input_blocks[0][0])}
}}"""
            return block_str
        elif len(input_blocks) == 2:
            block_str = f"""if (*tilingKeyAddr == {input_blocks[0][1]}) {{
{indent_code_func(input_blocks[0][0])}
}} else {{
{indent_code_func(input_blocks[1][0])}
}}"""
            return block_str
        else:
            total_length = len(input_blocks)
            left_block_str = self.gen_binary_search_block(
                input_blocks[0 : total_length // 2]
            )
            right_block_str = self.gen_binary_search_block(
                input_blocks[total_length // 2 :]
            )
            block_str = f"""if (*tilingKeyAddr < {input_blocks[total_length // 2][1]}) {{
{indent_code_func(left_block_str)}
}} else {{
{indent_code_func(right_block_str)}
}}"""
            return block_str

    def gen_switch_code_of_dynamic_op(self):
        dynamic_func_names = self.called_kernel_name["dynamic_func_names"]
        param_types = ", ".join(["GM_ADDR " for param in self.kernel_params])
        origin_switch_block = []
        for tiling_key in dynamic_func_names:
            kernel_info_of_tiling_key = dynamic_func_names[tiling_key]
            kernel_type = STR_TO_SK_KERNEL_TYPE[
                kernel_info_of_tiling_key["kernel_type"]
            ]
            case_block = self.gen_switch_case_block_of_dynamic_op(
                kernel_info_of_tiling_key, tiling_key, kernel_type
            )
            origin_switch_block.append([case_block, tiling_key])
        origin_switch_block.sort(key=lambda x: int(x[1]))
        switch_code = self.gen_binary_search_block(origin_switch_block)
        aiv_func_addr_str = self.gen_param_code("uint64_t& aiv_func_addr")
        aic_func_addr_str = self.gen_param_code("uint64_t& aic_func_addr")
        self.dynamic_impl_func_block = f"""
// begin implement of dynamic op {self.kernel_name}
static __aicore__ void switch_func_of_{self.kernel_name}(GM_ADDR __ac_dynamic_tiling_key_{self.index}, \
GM_ADDR __ac_dynamic_block_num_{self.index}, GM_ADDR __ac_wait_lock_{self.index}, \
{aiv_func_addr_str}, {aic_func_addr_str}, uint64_t& dy_block_num) {{
    __gm__ uint64_t* tilingKeyAddr = reinterpret_cast<__gm__ uint64_t*>(__ac_dynamic_tiling_key_{self.index});
    __gm__ uint64_t* blockNumAddr = reinterpret_cast<__gm__ uint64_t*>(__ac_dynamic_block_num_{self.index});
    __gm__ volatile uint64_t* lockAddr = reinterpret_cast<__gm__ uint64_t*>(__ac_wait_lock_{self.index});
    dcci(lockAddr, 0, 2);
    while(*lockAddr != 1) {{
        dcci(lockAddr, 0, 2);
    }}

    {switch_code}
    return;
}}
"""

    def dynamic_gen_split_call_code(self, func_name, params):
        result = ""
        if self.split_mode > 1:
            result += f"""if ((coreid % {self.split_mode}) == 0) {{

                {func_name}({params});"""
            for i in range(1, self.split_mode - 1):
                result += f"""\n            }} else if ((coreid % {self.split_mode}) == {i}) {{
            {func_name}_split{i}({params});"""
            result += f"""\n            }} else {{


                {func_name}_split{self.split_mode - 1}({params});
            }}"""
        else:
            result += f"{func_name}({params});"
        return result

    def gen_kernel_call_block(self, enable_double_stream: bool):
        self.kernel_call_block = ""
        if enable_double_stream:
            self.kernel_call_block += indent_code_func(
                "AscendC::SyncAll<false>(); // reason: double stream need syncall to wait switch func\n"
            )
        aiv_func_addr = self.gen_param_code("aiv_func_addr")
        aic_func_addr = self.gen_param_code("aic_func_addr")
        self.kernel_call_block += self.gen_dcci_before_kernel_start_call_block()
        self.kernel_call_block += f"call_func_of_{self.kernel_name}({self.param_offset}, {aiv_func_addr}, {aic_func_addr}, dy_blockNum);\n"
        self.kernel_call_block += self.gen_dcci_after_kernel_end_call_block()
        if self.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
        ]:
            self.kernel_call_block += "if ASCEND_IS_AIV {\n"
        else:
            self.kernel_call_block += "if ASCEND_IS_AIC {\n"
        wait_lock_offset = (
            self.param_offset
            + len(self.kernel_params)
            + self.extra_kernel_params.index(f"__ac_wait_lock_{self.index}")
        )
        self.kernel_call_block += f"""
    if (AscendC::GetBlockIdx() == 0) {{
        __gm__ volatile uint64_t* lockAddr = reinterpret_cast<__gm__ uint64_t*>(param_base[{wait_lock_offset}]);
        *lockAddr = 0;
        dcci(lockAddr, 0, 2);
    }}
}}
"""

    def gen_dynamic_op_call_func(self, enable_double_stream: bool):
        func_type = "using FuncType = void (*)(uint64_t args_offset);"
        self.gen_kernel_call_block(enable_double_stream)

        dy_aiv_func_ptr = self.gen_param_code("const uint64_t dy_aiv_func_ptr")
        dy_aic_func_ptr = self.gen_param_code("const uint64_t dy_aic_func_ptr")
        dynamic_impl_func_block = ""
        dynamic_impl_func_block += f"""
__aicore__ inline void call_func_of_{self.kernel_name}(uint64_t args_offset, \
{dy_aiv_func_ptr}, {dy_aic_func_ptr}, const uint64_t dy_block_num) {{
    uint64_t kernelType = dy_block_num  >> 32;
    uint64_t numBlocks = __placehoder__spk_block_num__;
    g_super_kernel_dynamic_block_num = dy_block_num & 0xFFFFFFFF;
    {func_type}
    """
        dynamic_impl_func_block += """
    FuncType aiv_ptr = (FuncType)(dy_aiv_func_ptr);
    FuncType aic_ptr = (FuncType)(dy_aic_func_ptr);
    """
        if self.split_mode > 1:
            for i in range(1, self.split_mode):
                dynamic_impl_func_block += f"""
    FuncType aiv_ptr_split{i} = (FuncType)(dy_aiv_func_ptr_split{i});
    FuncType aic_ptr_split{i} = (FuncType)(dy_aic_func_ptr_split{i});
    """
        dynamic_impl_func_block += f"""
    if (kernelType == {SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1.value} || \
kernelType == {SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2.value}) {{
        if (get_block_idx() < numBlocks) {{
            uint8_t coreid = get_coreid();
            if ASCEND_IS_AIC {{
                {self.dynamic_gen_split_call_code("aic_ptr", "args_offset")}
            }} else {{
                {self.dynamic_gen_split_call_code("aiv_ptr", "args_offset")}
            }}
        }}
    }} else if(kernelType == {SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY.value} || \
kernelType == {SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0.value}) {{
        if (AscendC::GetBlockIdx() < numBlocks) {{
            uint8_t coreid = get_coreid();
            if ASCEND_IS_AIV{{
                {self.dynamic_gen_split_call_code("aiv_ptr", "args_offset")}
            }}
        }}
    }} else if (kernelType == {SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY.value} || \
kernelType == {SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0.value}) {{
        if (get_block_idx() < numBlocks) {{
            uint8_t coreid = get_coreid();
            if ASCEND_IS_AIC {{
                {self.dynamic_gen_split_call_code("aic_ptr", "args_offset")}
            }}
        }}
    }}
}}

"""

        self.dynamic_impl_func_block += dynamic_impl_func_block
        if self.early_start_set_flag or self.early_start_wait_flag:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                (f"{self.kernel_name} is dynamic op, do not support early start"),
            )
        # for hard sync of dynamic op
        self.set_early_start_complement_blocks("ASCEND_IS_AIV", "true")
        self.set_early_start_complement_blocks("ASCEND_IS_AIC", "true")

    def process_of_dynamic_op(self, enable_double_stream: bool):
        # set sub_op_task_type to append extra params: block_num, tiling_key, lock
        self.sub_op_task_type = SubOperatorType.DYNAMIC_OP

        # add bin path to dynamic_bin for link
        self.dynamic_bin = self.bin_path
        # set block_num to max
        if self.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
        ]:
            self.block_num = int(get_soc_spec("vector_core_cnt"))
        else:
            self.block_num = int(get_soc_spec("ai_core_cnt"))
        self.extra_kernel_params = [
            f"__ac_dynamic_tiling_key_{self.index}",
            f"__ac_dynamic_block_num_{self.index}",
            f"__ac_wait_lock_{self.index}",
        ]
        aiv_func_addr_str = self.gen_param_code("aiv_func_addr")
        aic_func_addr_str = self.gen_param_code("aic_func_addr")
        dynamic_extra_param_offset = self.param_offset + len(self.kernel_params)
        self.call_dynamic_switch_func = f"switch_func_of_{self.kernel_name}(param_base[{dynamic_extra_param_offset}], \
param_base[{dynamic_extra_param_offset + 1}], param_base[{dynamic_extra_param_offset + 2}], \
{aiv_func_addr_str}, {aic_func_addr_str}, dy_blockNum);\n"

        self.gen_switch_code_of_dynamic_op()

        self.gen_dynamic_op_call_func(enable_double_stream)

    def get_text_section_size(self, binary_file):
        command = ["llvm-objdump", "-h", binary_file]
        result = subprocess.run(
            command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )

        if result.returncode != 0:
            CommonUtility.print_compile_log(
                "",
                f"The binary {binary_file} of {self.kernel_name} decode .text Error: {result.stderr}",
                AscendCLogLevel.LOG_WARNING,
            )
            return 0

        for line in result.stdout.splitlines():
            if ".text" in line:
                parts = line.split()
                if len(parts) >= 4:
                    size = int(parts[2], 16)
                    return min(size, 2048 * 8)

        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            (
                f"The binary {binary_file} of {self.kernel_name} do not found .text section"
            ),
        )
        return 0

    def extract_sub_bin_file(self, kernel_meta_dir, bin_file_name):
        with change_dir(kernel_meta_dir):
            try:
                if os.path.exists(os.path.join(kernel_meta_dir, bin_file_name)):
                    return
                CommonUtility.dump_compile_log(
                    ["cd", f"{kernel_meta_dir};", "ar", "x", self.bin_path],
                    CompileStage.UNPACK,
                    self.compile_log_path,
                )
                subprocess.run(["ar", "x", self.bin_path])
            except Exception as err:
                CommonUtility().ascendc_raise_python_err(
                    ERR_CODE, ("ar extract files or mv files failed", err)
                )

    def extract_sub_bin_file_of_mix_kernel(
        self, kernel_meta_dir, aiv_bin_file_name, aic_bin_file_name
    ):
        with change_dir(kernel_meta_dir):
            try:
                if os.path.exists(
                    os.path.join(kernel_meta_dir, aiv_bin_file_name)
                ) and os.path.exists(os.path.join(kernel_meta_dir, aic_bin_file_name)):
                    return
                CommonUtility.dump_compile_log(
                    ["cd", f"{kernel_meta_dir};", "ar", "x", self.bin_path],
                    CompileStage.UNPACK,
                    self.compile_log_path,
                )
                subprocess.run(["ar", "x", self.bin_path])
            except Exception as err:
                CommonUtility().ascendc_raise_python_err(
                    ERR_CODE, ("ar extract files or mv files failed", err)
                )

    def extract_sub_op_bin_files(self):
        chip_version = CommonUtility.get_chip_version()
        kernel_meta_dir_with_thread_id = os.path.join(
            CommonUtility.get_kernel_meta_dir(), str(threading.get_ident())
        )
        if not os.path.exists(kernel_meta_dir_with_thread_id):
            os.makedirs(kernel_meta_dir_with_thread_id)
        if self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY:
            bin_file_name = os.path.basename(
                self.called_kernel_name["AiCore"]["obj_files"]
            )
            if self.split_mode_in_json is None:
                self.aiv_bin = self.bin_path
            else:
                self.extract_sub_bin_file(kernel_meta_dir_with_thread_id, bin_file_name)
                self.aiv_bin = os.path.join(
                    kernel_meta_dir_with_thread_id, bin_file_name
                )
            self.aiv_text_len = self.get_text_section_size(self.aiv_bin)
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY:
            bin_file_name = os.path.basename(
                self.called_kernel_name["AiCore"]["obj_files"]
            )
            if self.split_mode_in_json is None:
                self.aic_bin = self.bin_path
            else:
                self.extract_sub_bin_file(kernel_meta_dir_with_thread_id, bin_file_name)
                self.aic_bin = os.path.join(
                    kernel_meta_dir_with_thread_id, bin_file_name
                )
            self.aic_text_len = self.get_text_section_size(self.aic_bin)
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0:
            aiv_bin_file_name = os.path.basename(
                self.called_kernel_name[f"dav-{chip_version}-vec"]["obj_files"]
            )
            if self.split_mode_in_json is None:
                self.aiv_bin = self.bin_path
            else:
                self.extract_sub_bin_file(
                    kernel_meta_dir_with_thread_id, aiv_bin_file_name
                )
                self.aiv_bin = os.path.join(
                    kernel_meta_dir_with_thread_id, aiv_bin_file_name
                )
            self.aiv_text_len = self.get_text_section_size(self.aiv_bin)
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0:
            aic_bin_file_name = os.path.basename(
                self.called_kernel_name[f"dav-{chip_version}-cube"]["obj_files"]
            )
            if self.split_mode_in_json is None:
                self.aic_bin = self.bin_path
            else:
                self.extract_sub_bin_file(
                    kernel_meta_dir_with_thread_id, aic_bin_file_name
                )
                self.aic_bin = os.path.join(
                    kernel_meta_dir_with_thread_id, aic_bin_file_name
                )
            self.aic_text_len = self.get_text_section_size(self.aic_bin)
        elif (
            self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
            or self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
        ):
            aiv_bin_file_name = os.path.basename(
                self.called_kernel_name[f"dav-{chip_version}-vec"]["obj_files"]
            )
            aic_bin_file_name = os.path.basename(
                self.called_kernel_name[f"dav-{chip_version}-cube"]["obj_files"]
            )
            self.extract_sub_bin_file_of_mix_kernel(
                kernel_meta_dir_with_thread_id, aiv_bin_file_name, aic_bin_file_name
            )
            self.aiv_bin = os.path.join(
                kernel_meta_dir_with_thread_id, aiv_bin_file_name
            )
            self.aiv_text_len = self.get_text_section_size(self.aiv_bin)
            self.aic_bin = os.path.join(
                kernel_meta_dir_with_thread_id, aic_bin_file_name
            )
            self.aic_text_len = self.get_text_section_size(self.aic_bin)

    def sub_op_gen_feed_sync_all_code(self, end_flag):
        code = ""
        if (
            self.feed_sync_all_mode.value
            == SuperKernelFeedSyncAllMode.FeedSyncAllDisable.value
        ):
            return code
        if end_flag is False:
            code += "AscendC::SuperKernelAutoSyncAllEndImpl();\n"
        else:
            code += """else {
    AscendC::SuperKernelAutoSyncAllComplementImpl();
  }
"""
        return code

    def gen_call_func(self, indent_code, core_type, block_type, is_preload=False):
        vector_call_func_block = f"if {core_type} {{\n"
        split_times = len(indent_code)
        block_num = self.block_num
        if not is_preload:
            vector_call_func_block += indent_code_func(
                self.gen_dcci_before_kernel_start_call_block(), "  "
            )
        vector_call_func_block += f"  if ({block_type}() < {block_num}) {{\n"

        if split_times > 1:
            vector_call_func_block += "    uint8_t coreid = (uint8_t)get_coreid();\n"
            vector_call_func_block += f"    if ((coreid % {split_times}) == 0) {{\n"
            vector_call_func_block += indent_code_func(indent_code[0], "      ")
            for i in range(1, split_times - 1):
                vector_call_func_block += (
                    "    } " + f"else if ((coreid % {split_times}) == {i}) {{\n"
                )
                vector_call_func_block += indent_code_func(indent_code[i], "      ")
            vector_call_func_block += "    } " + "else {\n"
            vector_call_func_block += indent_code_func(
                indent_code[split_times - 1], "      "
            )
            vector_call_func_block += "    }\n\n"
        else:
            vector_call_func_block += indent_code_func(indent_code[0], "      ")

        vector_call_func_block += "  }\n\n"
        if not is_preload:
            vector_call_func_block += indent_code_func(
                self.gen_dcci_after_kernel_end_call_block(), "  "
            )
        vector_call_func_block += "}\n\n"
        return vector_call_func_block

    def gen_call_func_with_syncall(self, indent_code, core_type, block_type):
        vector_call_func_block = f"if {core_type} {{\n"
        split_times = len(indent_code)
        block_num = self.block_num
        vector_call_func_block += indent_code_func(
            self.gen_dcci_before_kernel_start_call_block(), "  "
        )
        vector_call_func_block += f"  if ({block_type}() < {block_num}) {{\n"

        if split_times > 1:
            vector_call_func_block += "    uint8_t coreid = (uint8_t)get_coreid();\n"
            vector_call_func_block += f"    if ((coreid % {split_times}) == 0) {{\n"
            vector_call_func_block += indent_code_func(indent_code[0], "      ")
            vector_call_func_block += indent_code_func(
                self.sub_op_gen_feed_sync_all_code(False), "      "
            )
            for i in range(1, split_times - 1):
                vector_call_func_block += (
                    "    } " + f"else if ((coreid % {split_times}) == {i}) {{\n"
                )
                vector_call_func_block += indent_code_func(indent_code[i], "      ")
                vector_call_func_block += indent_code_func(
                    self.sub_op_gen_feed_sync_all_code(False), "      "
                )
            vector_call_func_block += "    } " + "else {\n"
            vector_call_func_block += indent_code_func(
                indent_code[split_times - 1], "      "
            )
            vector_call_func_block += indent_code_func(
                self.sub_op_gen_feed_sync_all_code(False), "      "
            )
            vector_call_func_block += "    }\n\n"
        else:
            vector_call_func_block += indent_code_func(indent_code[0], "      ")

        vector_call_func_block += "  } "
        vector_call_func_block += indent_code_func(
            self.gen_dcci_after_kernel_end_call_block(), "  "
        )
        vector_call_func_block += self.sub_op_gen_feed_sync_all_code(True)
        vector_call_func_block += "}\n\n"
        return vector_call_func_block

    def gen_early_start_complement_func(
        self, core_type, condition_code, gen_set_flag: bool, sub_block: bool = True
    ):
        vector_call_func_block = f"if {core_type} {{\n"
        vector_call_func_block += f"    if ({condition_code}) {{\n"
        if gen_set_flag:
            if (
                self.early_start_mode.value
                == SuperKernelEarlyStartMode.EarlyStartEnableV1.value
            ):
                vector_call_func_block += "        AscendC::SetNextTaskStart();\n"
            else:
                if (
                    self.kernel_type
                    in [
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
                    ]
                    and core_type == "ASCEND_IS_AIC"
                ):
                    vector_call_func_block += (
                        "        // AIV only, no complement early start set flag.\n"
                    )
                elif (
                    self.kernel_type
                    in [
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                    ]
                    and core_type == "ASCEND_IS_AIV"
                ):
                    vector_call_func_block += (
                        "        // AIC only, no complement early start set flag.\n"
                    )
                else:
                    vector_call_func_block += "        AscendC::SetNextTaskStart();\n"
        else:
            vector_call_func_block += "        AscendC::WaitPreTaskEndImpl<__placehoder__earlay_config__>();\n"
        vector_call_func_block += "    }\n"
        vector_call_func_block += "}\n\n"
        return vector_call_func_block

    def set_early_start_complement_blocks(
        self, core_type, condition_code, sub_block: bool = True
    ):
        if self.early_start_set_flag:
            self.early_start_complement_set_flag_block += (
                self.gen_early_start_complement_func(
                    core_type, condition_code, True, sub_block
                )
            )
        else:
            self.early_start_complement_set_flag_block += (
                self.gen_early_start_complement_func(core_type, "true", True, sub_block)
            )

        if self.early_start_wait_flag:
            self.early_start_complement_wait_flag_block += (
                self.gen_early_start_complement_func(
                    core_type, condition_code, False, sub_block
                )
            )
        else:
            self.early_start_complement_wait_flag_block += (
                self.gen_early_start_complement_func(
                    core_type, "true", False, sub_block
                )
            )
        return

    def _gen_sub_kernel_decare_once(self, aicore_kernel_name, params_with_type):
        kernel_declare = f'extern "C"  __aicore__ void {aicore_kernel_name}(uint64_t args_offset);\n\n'
        if self.split_mode > 1:
            for j in range(1, self.split_mode):
                kernel_declare += f'extern "C"  __aicore__ void {aicore_kernel_name}_split{j}(uint64_t args_offset);\n\n'
        return kernel_declare

    def _gen_func_call_list(self, aicore_kernel_name, params):
        fun_call_list = [f"{aicore_kernel_name}({self.param_offset});\n"]
        if self.split_mode > 1:
            for j in range(1, self.split_mode):
                fun_call_list.append(
                    f"{aicore_kernel_name}_split{j}({self.param_offset});\n"
                )
        return fun_call_list

    def _gen_preload_list_with_num(self, aicore_kernel_name, num):
        preload_list = [f"preload((const void *){aicore_kernel_name}, {num});\n"]
        if self.split_mode > 1:
            for j in range(1, self.split_mode):
                preload_list.append(
                    f"preload((const void *){aicore_kernel_name}_split{j}, {num});\n"
                )
        return preload_list

    def _gen_preload_list(self, aicore_kernel_name, text_len):
        return self._gen_preload_list_with_num(
            aicore_kernel_name, math.ceil(text_len / 2048)
        )

    def gen_sub_kernel_declare_and_call_func(self):
        params_with_type = ", ".join(
            [f"GM_ADDR {param}" for param in self.kernel_params]
        )
        chip_version = CommonUtility.get_chip_version()

        # generate date cache preload for sub operator
        self.data_cache_preload_call += (
            f"// begin add dc preload of sub_operator: {self.kernel_name}\n"
        )
        len_of_param = len(self.kernel_params)
        if self.index == 0 and not CommonUtility.is_c310():
            len_of_param += 1
        for index in range(0, len_of_param, 8):
            self.data_cache_preload_call += (
                "dc_preload((__gm__ uint64_t *)(param_base), 0); \n"
            )
            self.data_cache_preload_call += (
                f"param_base += {min(8, len_of_param - index)}; \n"
            )

        if self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY:
            aicore_kernel_name = self.called_kernel_name["AiCore"]["func_name"]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare = self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block = self.gen_call_func(
                func_call, "ASCEND_IS_AIV", "AscendC::GetBlockIdx"
            )
            self.kernel_call_block_with_syncall = self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIV", "AscendC::GetBlockIdx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aiv_text_len
            )
            self.preload_call_block = self.gen_call_func(
                preload_call_block,
                "ASCEND_IS_AIV",
                "AscendC::GetBlockIdx",
                is_preload=True,
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIV", f"AscendC::GetBlockIdx() >= {self.block_num}"
            )
            self.set_early_start_complement_blocks("ASCEND_IS_AIC", "true")
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY:
            aicore_kernel_name = self.called_kernel_name["AiCore"]["func_name"]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare = self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block = self.gen_call_func(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            self.kernel_call_block_with_syncall = self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aic_text_len
            )
            self.preload_call_block = self.gen_call_func(
                preload_call_block, "ASCEND_IS_AIC", "get_block_idx", is_preload=True
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIC", f"get_block_idx() >= {self.block_num}"
            )
            self.set_early_start_complement_blocks("ASCEND_IS_AIV", "true")
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0:
            aicore_kernel_name = self.called_kernel_name[f"dav-{chip_version}-vec"][
                "func_name"
            ]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare = self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block = self.gen_call_func(
                func_call, "ASCEND_IS_AIV", "AscendC::GetBlockIdx"
            )
            self.kernel_call_block_with_syncall = self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIV", "AscendC::GetBlockIdx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aiv_text_len
            )
            self.preload_call_block = self.gen_call_func(
                preload_call_block,
                "ASCEND_IS_AIV",
                "AscendC::GetBlockIdx",
                is_preload=True,
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIV", f"AscendC::GetBlockIdx() >= {self.block_num}"
            )
            self.set_early_start_complement_blocks("ASCEND_IS_AIC", "true")
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0:
            aicore_kernel_name = self.called_kernel_name[f"dav-{chip_version}-cube"][
                "func_name"
            ]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare = self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block = self.gen_call_func(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            self.kernel_call_block_with_syncall = self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aic_text_len
            )
            self.preload_call_block = self.gen_call_func(
                preload_call_block, "ASCEND_IS_AIC", "get_block_idx", is_preload=True
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIC", f"get_block_idx() >= {self.block_num}"
            )
            self.set_early_start_complement_blocks("ASCEND_IS_AIV", "true")
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1:
            # need check of sub block id
            aicore_kernel_name = self.called_kernel_name[f"dav-{chip_version}-cube"][
                "func_name"
            ]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare = self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block = self.gen_call_func(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            self.kernel_call_block_with_syncall = self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aic_text_len
            )
            self.preload_call_block = self.gen_call_func(
                preload_call_block, "ASCEND_IS_AIC", "get_block_idx", is_preload=True
            )
            aicore_kernel_name = self.called_kernel_name[f"dav-{chip_version}-vec"][
                "func_name"
            ]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block += self.gen_call_func(
                func_call, "ASCEND_IS_AIV", "get_block_idx"
            )
            self.kernel_call_block_with_syncall += self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIV", "get_block_idx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aiv_text_len
            )
            self.preload_call_block += self.gen_call_func(
                preload_call_block, "ASCEND_IS_AIV", "get_block_idx", is_preload=True
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIC", f"get_block_idx() >= {self.block_num}"
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIV", f"get_block_idx() >= {self.block_num}"
            )
        elif self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2:
            aicore_kernel_name = self.called_kernel_name[f"dav-{chip_version}-cube"][
                "func_name"
            ]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare = self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block = self.gen_call_func(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            self.kernel_call_block_with_syncall = self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIC", "get_block_idx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aic_text_len
            )
            self.preload_call_block = self.gen_call_func(
                preload_call_block, "ASCEND_IS_AIC", "get_block_idx", is_preload=True
            )
            aicore_kernel_name = self.called_kernel_name[f"dav-{chip_version}-vec"][
                "func_name"
            ]
            self.sub_kernel_names.append(aicore_kernel_name)
            self.kernel_declare += self._gen_sub_kernel_decare_once(
                aicore_kernel_name, params_with_type
            )
            params = ", ".join([f"{param}" for param in self.kernel_params])
            func_call = self._gen_func_call_list(aicore_kernel_name, params)
            self.kernel_call_block += self.gen_call_func(
                func_call, "ASCEND_IS_AIV", "get_block_idx"
            )
            self.kernel_call_block_with_syncall += self.gen_call_func_with_syncall(
                func_call, "ASCEND_IS_AIV", "get_block_idx"
            )
            preload_call_block = self._gen_preload_list(
                aicore_kernel_name, self.aiv_text_len
            )
            self.preload_call_block += self.gen_call_func(
                preload_call_block, "ASCEND_IS_AIV", "get_block_idx", is_preload=True
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIC", f"get_block_idx() >= {self.block_num}"
            )
            self.set_early_start_complement_blocks(
                "ASCEND_IS_AIV", f"get_block_idx() >= {self.block_num}"
            )
        else:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE, (f"kernel type {self.kernel_type} do not support!")
            )
