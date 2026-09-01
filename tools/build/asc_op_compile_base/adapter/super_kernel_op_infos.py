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
super kernel op infos
"""

import os
import json
import subprocess
import math
import shutil

from .super_kernel_utility import (
    AscendCLogLevel,
    CompileStage,
    CommonUtility,
    get_op_debug_config,
    get_soc_spec,
)
from .super_kernel_option_parse import parse_super_kernel_options
from .super_kernel_constants import (
    SuperKernelLinkMode,
    SuperKernelPreLoadMode,
    SuperKernelDataCacheMode,
    SuperKernelEarlyStartMode,
    SubOperatorType,
    SuperKernelStreamFusionMode,
    SuperKernelDebugDcciAllMode,
    SuperKernelDebugSyncAllMode,
    SuperKernelFeedSyncAllMode,
    SuperKernelProfilingMode,
    AI_CORE_STR,
    ERR_CODE,
    SuperKernelKernelType,
    STR_TO_SUPER_TASK_TYPE,
)
from .super_kernel_sub_op_infos import SubOperatorInfos


def gen_symbol_rename_file(dynamic_func_names, rename_file_path_list, split_mode):
    lines_list = []
    new_kernel_names_list = []
    chip_version = CommonUtility.get_chip_version()
    for _ in range(1, split_mode):
        lines_list.append([])
        new_kernel_names_list.append([])

    for tiling_key in dynamic_func_names:
        kernel_info_of_tiling_key = dynamic_func_names[tiling_key]
        for arch_name in [
            AI_CORE_STR,
            f"dav-{chip_version}-cube",
            f"dav-{chip_version}-vec",
        ]:
            if arch_name in kernel_info_of_tiling_key:
                kernel_name = kernel_info_of_tiling_key[arch_name]
                for i in range(1, split_mode):
                    new_kernel_name = f"{kernel_name}_split{i}"
                    lines_list[i - 1].append(f"{kernel_name} {new_kernel_name}")
                    new_kernel_names_list[i - 1].append(new_kernel_name)
    for i in range(1, split_mode):
        with open(rename_file_path_list[i - 1], "w", encoding="utf-8") as file:
            for line in lines_list[i - 1]:
                file.write(line + "\n")
    return new_kernel_names_list


def split_dynamic_o_in_super_kernel(
    orign_bin_path, rename_file_path, i, compile_log_path
):
    filename = os.path.basename(orign_bin_path)
    kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
    new_bin_path = os.path.join(kernel_meta_dir, filename[:-2] + f"_split{i}.o")
    if os.path.exists(new_bin_path):
        str_lst = f"WARNING: ALLREADY EXISTS split .o path: {new_bin_path}"
        CommonUtility.dump_compile_log(
            [str_lst], CompileStage.SPLIT_SUB_OBJS, compile_log_path
        )
    cmds = ["cp"] + ["-rfL"] + [f"{orign_bin_path}"] + [f"{new_bin_path}"]
    try:
        CommonUtility.dump_compile_log(
            cmds, CompileStage.SPLIT_SUB_OBJS, compile_log_path
        )
        subprocess.run(cmds)
    except Exception as err:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE, (f"{' '.join(cmds)} failed", err)
        )
    cmds = ["llvm-objcopy", f"--redefine-syms={rename_file_path}", f"{new_bin_path}"]
    try:
        CommonUtility.dump_compile_log(
            cmds, CompileStage.SPLIT_SUB_OBJS, compile_log_path
        )
        subprocess.run(cmds)
    except Exception as err:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE, (f"{' '.join(cmds)} failed", err)
        )
    return new_bin_path


def get_sub_op_streamid(op_info):
    streamid = op_info.get("stream_id")
    if streamid is not None:
        return streamid
    return -1


def normalize_nop_ops(op_list):
    """Remove NOP tasks and transfer their send events to same-stream real ops."""
    if not any(op.get("task_type") == "nop" for op in op_list):
        return op_list

    normalized_ops = []
    previous_op_by_stream = {}
    pending_events_by_stream = {}
    for index, op_info in enumerate(op_list):
        task_type = STR_TO_SUPER_TASK_TYPE[op_info.get("task_type", "normal")]
        if task_type != SubOperatorType.NOP_OP:
            normalized_op = dict(op_info)
            stream_id = get_sub_op_streamid(normalized_op)
            pending_events = pending_events_by_stream.pop(stream_id, [])
            if pending_events:
                notify_before_call_event_list = list(
                    normalized_op.get("notify_before_call_event_list", [])
                )
                normalized_op["notify_before_call_event_list"] = (
                    notify_before_call_event_list + pending_events
                )
            normalized_ops.append(normalized_op)
            previous_op_by_stream[stream_id] = normalized_op
            continue

        recv_event_list = op_info.get("recv_event_list", [])
        if recv_event_list:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"NOP op at index {index} must not have recv events: {recv_event_list}",
            )
        if index == len(op_list) - 1:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE, f"NOP op at index {index} must not be the last op"
            )

        send_event_list = list(op_info.get("send_event_list", []))
        if not send_event_list:
            continue
        stream_id = get_sub_op_streamid(op_info)
        previous_op = previous_op_by_stream.get(stream_id)
        if previous_op is not None:
            previous_op["send_event_list"] = (
                list(previous_op.get("send_event_list", [])) + send_event_list
            )
        else:
            pending_events_by_stream.setdefault(stream_id, []).extend(send_event_list)

    if pending_events_by_stream:
        stream_id, event_list = next(iter(pending_events_by_stream.items()))
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            f"NOP send events {event_list} have no following real op on stream "
            f"{stream_id}",
        )
    return normalized_ops


class SuperOperatorInfos:
    def __init__(self, kernel_infos, super_kernel_name):
        self.sub_decl_list = {}
        self.op_list = normalize_nop_ops(kernel_infos["op_list"])
        self.kernel_name: str = super_kernel_name
        self.compile_log_path = None
        self.creat_compile_log()
        self.info_base = []
        self.super_kernel_params = []
        self.enable_double_stream: bool = False
        self.op_options = parse_super_kernel_options(
            kernel_infos.get("super_kernel_options", "")
        )
        self.split_mode = self.op_options.get("split-mode", 4)
        self.profiling_mode = self.op_options.get(
            "profiling", SuperKernelProfilingMode.ProfilingDisable
        )
        self.stream_fusin_mode = self.op_options.get(
            "stream-fusion", SuperKernelStreamFusionMode.StreamFusionDisable
        )
        self.feed_sync_all_mode = self.op_options.get(
            "feed-sync-all", SuperKernelFeedSyncAllMode.FeedSyncAllDisable
        )
        self.debug_aic_num: int = self.op_options.get("debug-aic-num", 0)
        self.debug_aiv_num: int = self.op_options.get("debug-aiv-num", 0)
        self.inner_event_id_set = set()
        for index, op_info in enumerate(self.op_list):
            if "json_path" not in op_info:
                continue
            stream_id = get_sub_op_streamid(op_info)
            self.info_base.append(
                SubOperatorInfos(
                    index, op_info, stream_id, self.op_options, self.compile_log_path
                )
            )
        self.init_sub_operators()
        self.kernel_type: SuperKernelKernelType = SuperKernelKernelType.KERNEL_TYPE_MAX
        self.timestamp_option: bool = False
        self.debug_size: int = 0
        self.debug_option: str = ""
        self.block_num: int = 0
        self.workspace_size = 0
        # superkernel block num should be greater equal than max of block num of subops
        self.get_summary_type_and_options()
        self.adjust_dynamic_op_block_num()
        self.compile_info: json = None
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        file_name_tag = CommonUtility.get_distinct_filename_tag() + "_kernel.cpp"
        self.kernel_file = os.path.realpath(
            os.path.join(kernel_meta_dir, self.kernel_name + file_name_tag)
        )
        self.gen_op_options()
        self.gen_super_kernel_params()
        self.cub_op_list: list = []
        self.vec_op_list: list = []
        self.gen_compile_info()
        if self.enable_double_stream is True:
            self.split_op_by_kernel_type()
            self.insert_sync_by_stream_idx()
            self.print_send_recv_info("[Sync by stream idx]")
            self.insert_sync_by_event()
            self.print_send_recv_info("[Sync by evnet]")
            self.insert_sync_for_notify()
            self.print_send_recv_info("[Sync by notify]")
            self.optimize_sync_pass()
            self.print_send_recv_info("[After Optimize]")

    def gen_op_options(self):
        self.link_mode: SuperKernelLinkMode = self.op_options.get(
            "link-mode", SuperKernelLinkMode.PerCubeHerVecWithSuper
        )
        self.preload_mode: SuperKernelPreLoadMode = self.op_options.get(
            "preload-code", SuperKernelPreLoadMode.PreloadByAdanvanceStep
        )
        if self.enable_double_stream:
            self.early_start_mode: SuperKernelEarlyStartMode = (
                SuperKernelEarlyStartMode.EarlyStartDisable
            )
        else:
            self.early_start_mode: SuperKernelEarlyStartMode = self.op_options.get(
                "early-start", SuperKernelEarlyStartMode.EarlyStartEnableV2
            )
        self.datacache_mode: SuperKernelDataCacheMode = self.op_options.get(
            "preload-data", SuperKernelDataCacheMode.DataCacheLoadNA
        )
        self.debug_dcci_all_mode: SuperKernelDebugDcciAllMode = self.op_options.get(
            "debug-dcci-all", SuperKernelDebugDcciAllMode.DebugDcciAllDisable
        )
        self.debug_sync_all_mode: SuperKernelDebugSyncAllMode = self.op_options.get(
            "debug-sync-all", SuperKernelDebugSyncAllMode.DebugSyncAllDisable
        )

        self.check_dcci_before_after_op_options()

    def print_send_recv_info(self, stage):
        CommonUtility.dump_compile_log(
            [stage], CompileStage.SPLIT_SUB_OBJS, self.compile_log_path
        )
        for sub_op in self.info_base:
            CommonUtility.dump_compile_log(
                [
                    f"op_name: {sub_op.kernel_name_for_multi_stream}, send_info: {sub_op.send_info}, \
                recv_info: {sub_op.recv_info}"
                ],
                CompileStage.SPLIT_SUB_OBJS,
                self.compile_log_path,
            )

    def split_op_by_kernel_type(self):
        """
        cub_op_list save all cube ops and mix ops
        vec_op_list save all vec ops and mix ops
        """
        for sub_op in self.info_base:
            if sub_op.kernel_type in [
                SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
            ]:
                self.cub_op_list.append(sub_op)
            if sub_op.kernel_type in [
                SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
                SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
            ]:
                self.vec_op_list.append(sub_op)

    def get_task_type(self, op):
        if op.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
        ]:
            return "cub"
        elif op.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
        ]:
            return "vec"
        else:
            return "mix"

    def gen_sync_name(self, pre_op, current_op):
        pre_type = self.get_task_type(pre_op)
        current_type = self.get_task_type(current_op)
        if pre_type == "mix" and current_type == "mix":
            return "cub:vec;vec:cub"
        elif pre_type == "mix" and current_type != "mix":
            if current_type == "cub":
                return "vec:cub"
            else:
                return "cub:vec"
        elif pre_type != "mix" and current_type == "mix":
            if pre_type == "cub":
                return "cub:vec"
            else:
                return "vec:cub"
        else:
            return f"{pre_type}:{current_type}"

    def insert_sync_event(self, pre_op, current_op):
        """
        insert sync event for pre_op to current_op
        for pre_op, insert to send_info
        for current_op, insert to recv_info
        """
        sync_event = self.gen_sync_name(pre_op, current_op)
        pre_type = self.get_task_type(pre_op)
        if pre_type == "mix" or pre_type == "vec":
            idx = self.vec_op_list.index(pre_op)
            self.vec_op_list[idx].send_info[current_op.kernel_name_for_multi_stream] = (
                sync_event
            )
        if pre_type == "mix" or pre_type == "cub":
            idx = self.cub_op_list.index(pre_op)
            self.cub_op_list[idx].send_info[current_op.kernel_name_for_multi_stream] = (
                sync_event
            )

        current_type = self.get_task_type(current_op)
        if current_type == "mix" or current_type == "vec":
            idx = self.vec_op_list.index(current_op)
            self.vec_op_list[idx].recv_info[pre_op.kernel_name_for_multi_stream] = (
                sync_event
            )
        if current_type == "mix" or current_type == "cub":
            idx = self.cub_op_list.index(current_op)
            self.cub_op_list[idx].recv_info[pre_op.kernel_name_for_multi_stream] = (
                sync_event
            )

    def insert_sync_by_stream_idx(self):
        """
        insert sync event if ops in same stream
        """
        same_stream_ops = {}
        for sub_op in self.info_base:
            stream_idx = sub_op.stream_index
            if same_stream_ops.get(stream_idx) is None:
                same_stream_ops[stream_idx] = []
            same_stream_ops[stream_idx].append(sub_op)
        for stream_idx in same_stream_ops.keys():
            pre_op = None
            same_stream_ops[stream_idx][-1].is_last_op = True
            for current_op in same_stream_ops[stream_idx]:
                if pre_op is not None:
                    self.insert_sync_event(pre_op, current_op)
                pre_op = current_op

    def insert_sync_by_event(self):
        """
        insert sync event according to send_event_list and recv_event_list
        e.g.
        op1: send_event_list [100, 101]
        op2: recv_event_list [100, 101]
        then insert sync: op1->op2
        """
        event_send = {}
        event_recv = {}

        for sub_op in self.info_base:
            for event_id in sub_op.send_event_list:
                event_send[event_id] = sub_op
            for event_id in sub_op.recv_event_list:
                event_recv[event_id] = sub_op

        for send_id in event_send.keys():
            if event_recv.get(send_id) is not None:
                if event_send[send_id] == event_recv[send_id]:
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        (
                            f"send op {event_send[send_id].kernel_name_for_multi_stream} can not same with recv op \
{event_recv[send_id].kernel_name_for_multi_stream}"
                        ),
                    )
                self.insert_sync_event(event_send[send_id], event_recv[send_id])

    def insert_sync_for_notify(self):
        for sub_op in self.info_base[:-1]:
            op_type = self.get_task_type(sub_op)
            if (op_type == "mix") and (
                sub_op.notify_block.get("aic", "") != ""
                or sub_op.notify_block.get("aiv", "") != ""
            ):
                sub_op_index = self.info_base.index(sub_op)
                next_op = self.info_base[sub_op_index + 1]
                next_op_type = self.get_task_type(next_op)
                if next_op_type == "cub":
                    sub_op.notify_block["aic"] = sub_op.tmp_notify_block.get("aic", "")
                    sub_op.notify_block["aiv"] = sub_op.tmp_notify_block.get("aiv", "")
                if next_op.stream_index != sub_op.stream_index:
                    flag = False
                    for sub_send_info in sub_op.send_info:
                        if sub_send_info == next_op.kernel_name_for_multi_stream:
                            flag = True
                    if flag is False:
                        self.insert_sync_event(sub_op, next_op)

    def remove_info_by_name(
        self, send_op_name, recv_op_name, is_delete_recv_info, update_content=""
    ):
        """delete sync event
        Args:
            send_op_name (str): sent op name
            recv_op_name (str): recv op name
            is_delete_recv_info (bool): process recv_info or send_info
            update_content (res): delete directly if update_content is "" else replace dict by update_content
        """
        for sub_op in self.info_base:
            if sub_op.kernel_name_for_multi_stream == send_op_name:
                if update_content == "":
                    if is_delete_recv_info is True:
                        sub_op.recv_info.pop(recv_op_name, "unknown")
                    else:
                        sub_op.send_info.pop(recv_op_name, "unknown")
                else:
                    if is_delete_recv_info is True:
                        sub_op.recv_info[recv_op_name] = update_content
                    else:
                        sub_op.send_info[recv_op_name] = update_content

    def get_remain_events(self, origin_events, delete_event):
        split_events = origin_events.split(";")
        remain_events = []

        for sub_event in split_events:
            if sub_event != delete_event:
                remain_events.append(sub_event)

        return ";".join(remain_events)

    def get_idx(self, op_name, is_vec_list):
        """
        get idx in vec_op_list or cub_op_list
        op_name: op kernel name
        is_vec_list: get idx in vec_op_list if True else get idx in cub_op_list
        """
        if is_vec_list:
            for sub_op in self.vec_op_list:
                if sub_op.kernel_name_for_multi_stream == op_name:
                    return self.vec_op_list.index(sub_op)
        else:
            for sub_op in self.cub_op_list:
                if sub_op.kernel_name_for_multi_stream == op_name:
                    return self.cub_op_list.index(sub_op)
        return 0

    def judge_remove(self, send_name, recv_name, is_cub_to_vec):
        if is_cub_to_vec:
            send_idx = self.get_idx(send_name, False)
            recv_idx = self.get_idx(recv_name, True)
            for sub_op in self.vec_op_list[0:recv_idx][::-1]:
                for key, value in sub_op.recv_info.items():
                    if "cub:vec" in value:
                        send_idx1 = self.get_idx(key, False)
                        recv_idx1 = self.get_idx(
                            sub_op.kernel_name_for_multi_stream, True
                        )
                        if recv_idx1 < recv_idx and send_idx1 > send_idx:
                            return True
            return False
        else:
            send_idx = self.get_idx(send_name, True)
            recv_idx = self.get_idx(recv_name, False)
            for sub_op in self.cub_op_list[0:recv_idx][::-1]:
                for key, value in sub_op.recv_info.items():
                    if "vec:cub" in value:
                        send_idx1 = self.get_idx(key, True)
                        recv_idx1 = self.get_idx(
                            sub_op.kernel_name_for_multi_stream, False
                        )
                        if recv_idx1 < recv_idx and send_idx1 > send_idx:
                            return True
            return False

    def remove_crossed_line_sync(self):
        delete_event = []
        for sub_op in self.cub_op_list:
            for key, value in sub_op.send_info.items():
                value_list = value.split(";")
                for sub_value in value_list:
                    if sub_value in "cub:vec":
                        flag = self.judge_remove(
                            sub_op.kernel_name_for_multi_stream, key, True
                        )
                        if flag is True:
                            delete_event.append(
                                [
                                    sub_op.kernel_name_for_multi_stream,
                                    key,
                                    False,
                                    self.get_remain_events(value, "cub:vec"),
                                ]
                            )
                            delete_event.append(
                                [
                                    key,
                                    sub_op.kernel_name_for_multi_stream,
                                    True,
                                    self.get_remain_events(value, "cub:vec"),
                                ]
                            )

        for sub_op in self.vec_op_list:
            for key, value in sub_op.send_info.items():
                value_list = value.split(";")
                for sub_value in value_list:
                    if sub_value in "vec:cub":
                        flag = self.judge_remove(
                            sub_op.kernel_name_for_multi_stream, key, False
                        )
                        if flag is True:
                            delete_event.append(
                                [
                                    sub_op.kernel_name_for_multi_stream,
                                    key,
                                    False,
                                    self.get_remain_events(value, "vec:cub"),
                                ]
                            )
                            delete_event.append(
                                [
                                    key,
                                    sub_op.kernel_name_for_multi_stream,
                                    True,
                                    self.get_remain_events(value, "vec:cub"),
                                ]
                            )

        for item in delete_event:
            self.remove_info_by_name(item[0], item[1], item[2], item[3])

    def remove_multi_send_info(self):
        delete_event = []
        for op in self.vec_op_list:
            if len(op.send_info) > 1:
                send_info_list = []
                for key, value in op.send_info.items():
                    if "vec:cub" in value:
                        send_info_list.append([key, value, self.get_idx(key, False)])
                if len(send_info_list) > 1:
                    send_info_list.sort(key=lambda x: x[2])
                    for sub_info in send_info_list[1:]:
                        delete_event.append(
                            [
                                op.kernel_name_for_multi_stream,
                                sub_info[0],
                                False,
                                self.get_remain_events(sub_info[1], "vec:cub"),
                            ]
                        )
                        delete_event.append(
                            [
                                sub_info[0],
                                op.kernel_name_for_multi_stream,
                                True,
                                self.get_remain_events(sub_info[1], "vec:cub"),
                            ]
                        )

        for item in delete_event:
            self.remove_info_by_name(item[0], item[1], item[2], item[3])

        delete_event = []
        for op in self.cub_op_list:
            if len(op.send_info) > 1:
                send_info_list = []
                for key, value in op.send_info.items():
                    if "cub:vec" in value:
                        send_info_list.append([key, value, self.get_idx(key, True)])
                if len(send_info_list) > 1:
                    send_info_list.sort(key=lambda x: x[2])
                    for sub_info in send_info_list[1:]:
                        delete_event.append(
                            [
                                op.kernel_name_for_multi_stream,
                                sub_info[0],
                                False,
                                self.get_remain_events(sub_info[1], "cub:vec"),
                            ]
                        )
                        delete_event.append(
                            [
                                sub_info[0],
                                op.kernel_name_for_multi_stream,
                                True,
                                self.get_remain_events(sub_info[1], "cub:vec"),
                            ]
                        )

        for item in delete_event:
            self.remove_info_by_name(item[0], item[1], item[2], item[3])

    def remove_multi_recv_info(self):
        delete_event = []
        for op in self.vec_op_list:
            if len(op.recv_info) > 1:
                recv_info_list = []
                for key, value in op.recv_info.items():
                    if "cub:vec" in value:
                        recv_info_list.append([key, value, self.get_idx(key, False)])
                if len(recv_info_list) > 1:
                    recv_info_list.sort(key=lambda x: x[2], reverse=True)
                    for sub_info in recv_info_list[1:]:
                        delete_event.append(
                            [
                                op.kernel_name_for_multi_stream,
                                sub_info[0],
                                True,
                                self.get_remain_events(sub_info[1], "cub:vec"),
                            ]
                        )
                        delete_event.append(
                            [
                                sub_info[0],
                                op.kernel_name_for_multi_stream,
                                False,
                                self.get_remain_events(sub_info[1], "cub:vec"),
                            ]
                        )

        for item in delete_event:
            self.remove_info_by_name(item[0], item[1], item[2], item[3])

        delete_event = []
        for op in self.cub_op_list:
            if len(op.recv_info) > 1:
                recv_info_list = []
                for key, value in op.recv_info.items():
                    if "vec:cub" in value:
                        recv_info_list.append([key, value, self.get_idx(key, True)])
                if len(recv_info_list) > 1:
                    recv_info_list.sort(key=lambda x: x[2], reverse=True)
                    for sub_info in recv_info_list[1:]:
                        delete_event.append(
                            [
                                op.kernel_name_for_multi_stream,
                                sub_info[0],
                                True,
                                self.get_remain_events(sub_info[1], "vec:cub"),
                            ]
                        )
                        delete_event.append(
                            [
                                sub_info[0],
                                op.kernel_name_for_multi_stream,
                                False,
                                self.get_remain_events(sub_info[1], "vec:cub"),
                            ]
                        )

        for item in delete_event:
            self.remove_info_by_name(item[0], item[1], item[2], item[3])

    def optimize_sync_pass(self):
        CommonUtility.print_compile_log("", "[INIT STATE]:", AscendCLogLevel.LOG_DEBUG)
        self.print_vec_cub_list_info()
        self.remove_crossed_line_sync()
        CommonUtility.print_compile_log(
            "", "[AFTER REMOVE CORESS LINE SYNC]:", AscendCLogLevel.LOG_DEBUG
        )
        self.print_vec_cub_list_info()
        self.remove_multi_send_info()
        self.remove_multi_recv_info()
        CommonUtility.print_compile_log(
            "", "[AFTER REMOVE MULTI EVENT SYNC]:", AscendCLogLevel.LOG_DEBUG
        )
        self.print_vec_cub_list_info()

    def print_vec_cub_list_info(self):
        CommonUtility.print_compile_log("", "[VEC LIST OP]:", AscendCLogLevel.LOG_DEBUG)
        for sub_op in self.vec_op_list:
            CommonUtility.print_compile_log(
                "",
                f"op_name: {sub_op.kernel_name_for_multi_stream}, \
                stream_idx: {sub_op.stream_index}, send_info: {sub_op.send_info}, \
                recv_info: {sub_op.recv_info}",
                AscendCLogLevel.LOG_DEBUG,
            )
        CommonUtility.print_compile_log("", "[CUB LIST OP]:", AscendCLogLevel.LOG_DEBUG)
        for sub_op in self.cub_op_list:
            CommonUtility.print_compile_log(
                "",
                f"op_name: {sub_op.kernel_name_for_multi_stream}, \
                stream_idx: {sub_op.stream_index}, send_info: {sub_op.send_info}, \
                recv_info: {sub_op.recv_info}",
                AscendCLogLevel.LOG_DEBUG,
            )

    def creat_compile_log(self):
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        self.compile_log_path = os.path.join(
            kernel_meta_dir, self.kernel_name + distinct_tag + ".log"
        )

    def sub_op_connect_set(self, former_op, op):
        former_send_list = former_op.send_event_list
        recv_list = op.recv_event_list
        former_send_set = set(former_send_list)
        recv_set = set(recv_list)
        union_set = former_send_set & recv_set
        return union_set

    def find_all_inner_event_id_set(self):
        sub_num = len(self.info_base)
        if sub_num <= 1:
            return
        for i in range(0, sub_num - 1):
            for j in range(i + 1, sub_num):
                connect_set = self.sub_op_connect_set(
                    self.info_base[i], self.info_base[j]
                )
                if (
                    self.info_base[i].stream_index == self.info_base[j].stream_index
                    and connect_set
                ):
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        (
                            f"ERROR: super kernel do not support self send/receive pair within 1 real stream: oplist: {self.op_list} "
                        ),
                    )
                elif connect_set:
                    self.inner_event_id_set.update(connect_set)

    def check_sp_has_two_real_stream(self):
        former_op = None
        for _, op in enumerate(self.info_base):
            if former_op is not None:
                self_connet_set = self.sub_op_connect_set(op, op)
                if self.sub_op_connect_set(op, op):
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        (
                            f"ERROR: exists send-recv event pair within 1 op:"
                            f" {op.kernel_name}, event id: {self_connet_set}, oplist:{self.op_list}"
                        ),
                    )
                connect_set = self.sub_op_connect_set(former_op, op)
                if former_op.stream_index == op.stream_index and connect_set:
                    CommonUtility().ascendc_raise_python_err(
                        ERR_CODE,
                        (
                            f"ERROR: super kernel do not support self send/receive pair within 1 real stream: oplist: {self.op_list} "
                        ),
                    )
                elif former_op.stream_index != op.stream_index and not connect_set:
                    if (
                        self.stream_fusin_mode.value
                        == SuperKernelStreamFusionMode.StreamFusionEnable.value
                    ):
                        CommonUtility.print_compile_log(
                            "",
                            f"enter into 2 real stream mode, oplist: {self.op_list} ",
                            AscendCLogLevel.LOG_DEBUG,
                        )
                        self.enable_double_stream = True
                        break
                    else:
                        CommonUtility().ascendc_raise_python_err(
                            ERR_CODE,
                            (
                                f"ERROR: super kernel do not support more than 2 real stream, use "
                                f"'options=\"stream-fusion=1\"' to enable operators fusion on multi-stream, "
                                f"oplist: {self.op_list} "
                            ),
                        )
                if connect_set:
                    self.inner_event_id_set.update(connect_set)
            former_op = op
        self.find_all_inner_event_id_set()

    def init_sub_operators(self):
        for sub_op in self.info_base:
            sub_op.init_of_sub_operator_info()
        self.check_sp_has_two_real_stream()
        CommonUtility.dump_compile_log(
            ["###INNER_ID:"] + list(self.inner_event_id_set),
            CompileStage.SPLIT_SUB_OBJS,
            self.compile_log_path,
        )

        param_offset = 0
        # c310 do not have ffts_addr
        if CommonUtility.is_has_ffts_mode():
            param_offset += 1
        for sub_op in self.info_base:
            if sub_op.notify_before_call_event_list:
                param_offset += len(sub_op.notify_before_call_event_list)
            sub_op.param_offset = param_offset
            sub_op.code_gen(self.inner_event_id_set, self.enable_double_stream)
            param_offset += len(sub_op.kernel_params) + len(sub_op.extra_kernel_params)

    def warn_op_sequence_with_no_dcci_option(self, op_sequence_with_no_dcci_option):
        if len(op_sequence_with_no_dcci_option) == 0:
            return

        CommonUtility.print_compile_log(
            "",
            "[Super Kernel] There are more than 2 consecutive sub-operators with option dcci-disable-on-kernel, "
            "may lead to data cache consistency issue.",
            AscendCLogLevel.LOG_WARNING,
        )

        for seq_id, seq in enumerate(op_sequence_with_no_dcci_option):
            if len(seq) > 0:
                for op_id, op_kernel_name in enumerate(seq):
                    CommonUtility.print_compile_log(
                        "",
                        f"[Super Kernel] Operator sequence {seq_id}, op_kernel_name {op_id}: {op_kernel_name}",
                        AscendCLogLevel.LOG_WARNING,
                    )

    def check_dcci_before_after_op_options(self):
        op_sequence_with_no_dcci_option = []
        current_sequence = []

        for sub_op in self.info_base:
            if sub_op.call_dcci_disable_on_kernel:
                current_sequence.append(sub_op.kernel_name)
            else:
                if len(current_sequence) >= 2:
                    op_sequence_with_no_dcci_option.append(current_sequence)
                current_sequence = []

        if len(current_sequence) >= 2:
            op_sequence_with_no_dcci_option.append(current_sequence)

        self.warn_op_sequence_with_no_dcci_option(op_sequence_with_no_dcci_option)

    def check_debug_aic_aiv_num_ratio(self):
        # aic:aiv ratio should be 1:0 or 0:1 or 1:1 or 1:2
        if self.debug_aic_num == 0 or self.debug_aiv_num == 0:
            return
        if self.debug_aic_num == self.debug_aiv_num:
            return
        if self.debug_aic_num * 2 == self.debug_aiv_num:
            return
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            f"[Super Kernel][ERROR]: ratio of super kernel options debug-aic-num {self.debug_aic_num} "
            f"to debug-aiv-num {self.debug_aiv_num} is invalid. Should be 1:0 or 0:1 or 1:1 or 1:2.",
        )

    def check_debug_aic_aiv_num_exceed_platform_num_blocks(self):
        max_aic_num = int(get_soc_spec("ai_core_cnt"))
        max_aiv_num = int(get_soc_spec("vector_core_cnt"))
        if self.debug_aic_num > max_aic_num:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel][ERROR]: super kernel option debug-aic-num {self.debug_aic_num} "
                f"exceeds current platform max aic num {max_aic_num}.",
            )

        if self.debug_aiv_num > max_aiv_num:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel][ERROR]: super kernel option debug-aiv-num {self.debug_aiv_num} "
                f"exceeds current platform max aiv num {max_aiv_num}.",
            )

    def raise_exceed_sub_op_aic_aiv_num_error(
        self, case_str, aic_or_aiv, debug_block_num, sub_op_block_num
    ):
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE,
            f"[Super Kernel][ERROR]: In super kernel {case_str} case, "
            f"option debug-{aic_or_aiv}-num {debug_block_num} should not "
            f"be less than max sub op {aic_or_aiv} num {sub_op_block_num}.",
        )

    def check_debug_aic_aiv_num_exceed_sub_op_aic_aiv_num(self):
        if self.debug_aic_num == 0 and self.debug_aiv_num == 0:
            return

        if self.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
        ]:
            if self.debug_aic_num < self.block_num:
                self.raise_exceed_sub_op_aic_aiv_num_error(
                    "aic", "aic", self.debug_aic_num, self.block_num
                )

        if self.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
        ]:
            if self.debug_aiv_num < self.block_num:
                self.raise_exceed_sub_op_aic_aiv_num_error(
                    "aiv", "aiv", self.debug_aiv_num, self.block_num
                )

        if self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1:
            if self.debug_aic_num < self.block_num:
                self.raise_exceed_sub_op_aic_aiv_num_error(
                    "mix 1:1", "aic", self.debug_aic_num, self.block_num
                )
            if self.debug_aiv_num < self.block_num:
                self.raise_exceed_sub_op_aic_aiv_num_error(
                    "mix 1:1", "aiv", self.debug_aiv_num, self.block_num
                )

        if self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2:
            if self.debug_aic_num < self.block_num:
                self.raise_exceed_sub_op_aic_aiv_num_error(
                    "mix 1:2", "aic", self.debug_aic_num, self.block_num
                )
            if self.debug_aiv_num < self.block_num * 2:
                self.raise_exceed_sub_op_aic_aiv_num_error(
                    "mix 1:2", "aiv", self.debug_aiv_num, self.block_num * 2
                )

    def update_superkernel_blocknum_by_debug_options(self):
        self.check_debug_aic_aiv_num_ratio()
        self.check_debug_aic_aiv_num_exceed_platform_num_blocks()
        self.check_debug_aic_aiv_num_exceed_sub_op_aic_aiv_num()

        if self.debug_aic_num == 0 and self.debug_aiv_num == 0:
            return
        if self.debug_aic_num > 0 and self.debug_aiv_num == 0:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
            self.block_num = self.debug_aic_num
        elif self.debug_aic_num == 0 and self.debug_aiv_num > 0:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
            self.block_num = self.debug_aiv_num
        elif self.debug_aic_num == self.debug_aiv_num:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
            self.block_num = self.debug_aic_num
        elif self.debug_aiv_num == 2 * self.debug_aic_num:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
            self.block_num = self.debug_aic_num
        else:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"ERROR: ratio of super kernel debug-aic-num {self.debug_aic_num} to "
                f"debug-aiv-num {self.debug_aiv_num} is invalid.",
            )

    def get_finale_type_and_block_num(
        self, final_kernel_type, max_aic_num, max_aiv_num
    ):
        # get kernel type of super kernel
        if final_kernel_type == 0b1:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
            self.block_num = max_aiv_num
        elif final_kernel_type == 0b10:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
            self.block_num = max_aic_num
        elif final_kernel_type == 0b100 or final_kernel_type == 0b101:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
            self.block_num = max_aiv_num
        elif final_kernel_type == 0b1000 or final_kernel_type == 0b1010:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
            self.block_num = max_aic_num
        elif final_kernel_type == 0b10000:
            self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
            self.block_num = max_aic_num
        else:
            if max_aiv_num <= max_aic_num:
                self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                self.block_num = max_aic_num
            else:
                self.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                max_1_2_aiv_block_num = math.ceil(max_aiv_num / 2)
                self.block_num = (
                    max_aic_num
                    if max_aic_num >= max_1_2_aiv_block_num
                    else max_1_2_aiv_block_num
                )

        self.update_superkernel_blocknum_by_debug_options()

    def get_summary_type_and_options(self):
        """set superkernel kernel type and block num."""
        final_kernel_type = 0
        max_aic_num = 0
        max_aiv_num = 0
        for sub_operator in self.info_base:
            sub_aiv_num = 0
            sub_aic_num = 0
            # summarize sub kernels kernel type infos
            if sub_operator.kernel_type == SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY:
                sub_aiv_num = sub_operator.block_num
                final_kernel_type = final_kernel_type | 0b1
            elif sub_operator.kernel_type == SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY:
                sub_aic_num = sub_operator.block_num
                final_kernel_type = final_kernel_type | 0b10
            elif (
                sub_operator.kernel_type
                == SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
            ):
                sub_aiv_num = sub_operator.block_num
                final_kernel_type = final_kernel_type | 0b100
            elif (
                sub_operator.kernel_type
                == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
            ):
                sub_aic_num = sub_operator.block_num
                final_kernel_type = final_kernel_type | 0b1000
            elif (
                sub_operator.kernel_type
                == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
            ):
                sub_aic_num = sub_operator.block_num
                sub_aiv_num = sub_operator.block_num
                final_kernel_type = final_kernel_type | 0b10000
            elif (
                sub_operator.kernel_type
                == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
            ):
                sub_aic_num = sub_operator.block_num
                sub_aiv_num = sub_operator.block_num * 2
                final_kernel_type = final_kernel_type | 0b100000

            # judge super kernel enable options
            if sub_operator.timestamp_option:
                self.timestamp_option = True
                if sub_operator.debug_size > self.debug_size:
                    self.debug_size = sub_operator.debug_size
                if self.debug_option != "":
                    option_list = self.debug_option.split(",")
                    sub_option_list = sub_operator.debug_option.split(",")
                    for option in sub_option_list:
                        if option not in option_list:
                            self.debug_option += ","
                            self.debug_option += f"{option}"
                else:
                    self.debug_option += f"{sub_operator.debug_option}"
            max_aic_num = sub_aic_num if sub_aic_num > max_aic_num else max_aic_num
            max_aiv_num = sub_aiv_num if sub_aiv_num > max_aiv_num else max_aiv_num

        self.get_finale_type_and_block_num(final_kernel_type, max_aic_num, max_aiv_num)

    def find_sub_kernel_name(self, origin_sub_kernel_names):
        aiv_kernel_name = origin_sub_kernel_names[0]
        aic_kernel_name = origin_sub_kernel_names[0]
        for sub_kernel_name in origin_sub_kernel_names:
            if "_mix_aiv_" in sub_kernel_name:
                aiv_kernel_name = sub_kernel_name
            elif "_mix_aic_" in sub_kernel_name:
                aic_kernel_name = sub_kernel_name
        return aiv_kernel_name, aic_kernel_name

    def adjust_dynamic_op_block_num(self):
        for sub_op in self.info_base:
            sub_op.adjust_dynamic_op(self.block_num)

    def split_o_in_super_kernel(self, orign_bin_path, origin_kernel_name, i):
        filename = os.path.basename(orign_bin_path)
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        new_bin_path = os.path.join(kernel_meta_dir, filename[:-2] + f"_split{i}.o")
        if os.path.exists(new_bin_path):
            str_lst = f"WARNING: ALLREADY EXISTS split .o path: {new_bin_path}"
            CommonUtility.dump_compile_log(
                [str_lst], CompileStage.SPLIT_SUB_OBJS, self.compile_log_path
            )
        cmds = ["cp"] + ["-rfL"] + [f"{orign_bin_path}"] + [f"{new_bin_path}"]
        try:
            CommonUtility.dump_compile_log(
                cmds, CompileStage.SPLIT_SUB_OBJS, self.compile_log_path
            )
            subprocess.run(cmds)
        except Exception as err:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE, (f"{' '.join(cmds)} failed", err)
            )
        new_kernel_name = f"{origin_kernel_name}_split{i}"
        cmds = [
            "llvm-objcopy",
            f"--redefine-sym={origin_kernel_name}={new_kernel_name}",
            f"{new_bin_path}",
        ]
        try:
            CommonUtility.dump_compile_log(
                cmds, CompileStage.SPLIT_SUB_OBJS, self.compile_log_path
            )
            subprocess.run(cmds)
        except Exception as err:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE, (f"{' '.join(cmds)} failed", err)
            )
        return new_bin_path, new_kernel_name

    def gen_super_kernel_params(self):
        for sub_operator in self.info_base:
            self.super_kernel_params += sub_operator.params_before_kernel
            self.super_kernel_params += sub_operator.kernel_params
            if sub_operator.sub_op_task_type.value == SubOperatorType.DYNAMIC_OP.value:
                self.super_kernel_params += sub_operator.extra_kernel_params
            elif sub_operator.sub_op_task_type.value == SubOperatorType.STATIC_OP.value:
                self.super_kernel_params += sub_operator.extra_kernel_params
        CommonUtility.dump_compile_log(
            ["### SK Arg: FFTS", ",".join(self.super_kernel_params)],
            CompileStage.SPLIT_SUB_OBJS,
            self.compile_log_path,
        )

    def get_ws_size(self, block_num):
        base_size = 512
        total_need_size = len(self.info_base) * 128
        while block_num * base_size <= total_need_size:
            base_size *= 2
        self.workspace_size = block_num * base_size

    def calc_workspace_size(self):
        if (
            self.feed_sync_all_mode.value
            == SuperKernelFeedSyncAllMode.FeedSyncAllDisable.value
        ):
            self.workspace_size = 0
            return
        if self.kernel_type in [
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
            SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
        ]:
            self.get_ws_size(self.block_num)
        else:
            self.get_ws_size(self.block_num * 2)

    def add_define_options(self, exist_dynamic_sub_ops, options: list):
        if (
            self.kernel_type == SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
            and CommonUtility.is_c310()
        ):
            options.append("-D__ASCENDC_DAVID_SPLIT_CORE__")
        if exist_dynamic_sub_ops:
            options.append("-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__")

        if (
            self.early_start_mode.value
            != SuperKernelEarlyStartMode.EarlyStartDisable.value
        ):
            options.append("-D__ASCENDC_ENABLE_SET_NEXT_TASK_START")
            options.append("-D__ASCENDC_ENABLE_WAIT_PRE_TASK_END")
            if (
                self.early_start_mode.value
                == SuperKernelEarlyStartMode.EarlyStartEnableV1.value
            ):
                options.append("-D__ASCENDC_SUPERKERNEL_EARLY_START_V1")
            else:
                options.append("-D__ASCENDC_SUPERKERNEL_EARLY_START_V2")

        if (
            self.feed_sync_all_mode.value
            == SuperKernelFeedSyncAllMode.FeedSyncAllEnable.value
        ):
            options.append("-D__ASCENDC_SUPERKERNEL_AUTO_SYNC_ALL__")

        if self.timestamp_option:
            options.append("-DASCENDC_DUMP")
        else:
            options.append("-DASCENDC_DUMP=0")

        external_option = [
            part_option.strip()
            for part_option in self.op_options.get("compile-options", "").split(",")
            if part_option.strip()
        ]
        for sub_external_option in external_option:
            options.append(sub_external_option)

    def gen_compile_info(self):
        options = ["-x", "cce"]
        ascend_home_path = os.environ.get("ASCEND_HOME_PATH")
        import platform

        archlinux = platform.machine()
        if ascend_home_path is None or ascend_home_path == "":
            asc_opc_path = shutil.which("asc_opc")
            if asc_opc_path is not None:
                asc_opc_path_link = os.path.dirname(asc_opc_path)
                asc_opc_real_path = os.path.realpath(asc_opc_path_link)
                ascend_home_path = os.path.realpath(
                    os.path.join(asc_opc_real_path, "..", "..")
                )
            else:
                ascend_home_path = "/usr/local/Ascend/cann"

        if "x86" in archlinux:
            asc_path = os.path.realpath(
                os.path.join(ascend_home_path, "x86_64-linux", "asc")
            )
        else:
            asc_path = os.path.realpath(
                os.path.join(ascend_home_path, "aarch64-linux", "asc")
            )
        if asc_path is None:
            asc_path = os.path.realpath(
                os.path.join(ascend_home_path, "compiler", "asc")
            )

        options.append("-I" + os.path.join(asc_path, "impl", "adv_api"))
        options.append("-I" + os.path.join(asc_path, "impl", "basic_api"))
        options.append("-I" + os.path.join(asc_path, "impl", "c_api"))
        options.append(
            "-I" + os.path.join(asc_path, "impl", "basic_api", "reg_compute")
        )
        options.append("-I" + os.path.join(asc_path, "impl", "simt_api"))
        options.append("-I" + os.path.join(asc_path, "impl", "utils"))
        options.append("-I" + os.path.join(asc_path, "include"))
        options.append("-I" + os.path.join(asc_path, "include", "adv_api"))
        options.append("-I" + os.path.join(asc_path, "include", "basic_api"))
        options.append("-I" + os.path.join(asc_path, "include", "aicpu_api"))
        options.append("-I" + os.path.join(asc_path, "include", "c_api"))
        options.append(
            "-I" + os.path.join(asc_path, "include", "basic_api", "reg_compute")
        )
        options.append("-I" + os.path.join(asc_path, "include", "simt_api"))
        options.append("-I" + os.path.join(asc_path, "include", "utils"))
        options.append("-I" + os.path.join(asc_path, "..", "ascendc", "act"))
        options.append("-I" + os.path.join(asc_path, "impl"))
        options.append("-I" + asc_path)
        options.append("-I" + os.path.join(asc_path, "..", "tikcpp"))
        options.append("-I" + os.path.join(asc_path, "..", "..", "include"))
        options.append("-I" + os.path.join(asc_path, "..", "..", "include", "ascendc"))
        options.append("-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw"))
        options.append("-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw", "impl"))
        options.append(
            "-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw", "interface")
        )
        exist_dynamic_sub_ops = False

        param_offset = []
        for sub_operator in self.info_base:
            param_offset.append(sub_operator.param_offset)

        notify_param_offset = []
        for sub_operator in self.info_base:
            notify_param_offset.append(sub_operator.notify_param_offset)

        notify_before_call_param_offset = []
        for sub_operator in self.info_base:
            notify_before_call_param_offset.append(
                sub_operator.notify_before_call_param_offset
            )

        wait_param_offset = []
        for sub_operator in self.info_base:
            wait_param_offset.append(sub_operator.wait_param_offset)

        recv_event_list = []
        for sub_operator in self.info_base:
            recv_event_list.append(sub_operator.recv_event_list)

        send_event_list = []
        for sub_operator in self.info_base:
            send_event_list.append(sub_operator.send_event_list)

        notify_before_call_event_list = []
        for sub_operator in self.info_base:
            notify_before_call_event_list.append(
                sub_operator.notify_before_call_event_list
            )

        sub_operator_info = []
        for sub_operator in self.info_base:
            operator_info = {}
            if sub_operator.aiv_bin is not None:
                operator_info["aiv_bin"] = sub_operator.aiv_bin
            if sub_operator.aic_bin is not None:
                operator_info["aic_bin"] = sub_operator.aic_bin
            if sub_operator.dynamic_bin is not None:
                operator_info["dynamic_bin"] = sub_operator.dynamic_bin
                exist_dynamic_sub_ops = True
            operator_info["sub_kernel_names"] = sub_operator.sub_kernel_names
            origin_aiv_kernel_name, origin_aic_kernel_name = self.find_sub_kernel_name(
                sub_operator.sub_kernel_names
            )
            sub_operator_info.append(operator_info)
            if sub_operator.dynamic_bin is None and sub_operator.split_mode > 1:
                for i in range(1, sub_operator.split_mode):
                    cur_operator_info = {}
                    new_sub_op_sub_kernel_names = []
                    if sub_operator.aiv_bin is not None:
                        if sub_operator.split_mode_in_json is None:
                            split_o_path, new_kernel_name = (
                                self.split_o_in_super_kernel(
                                    sub_operator.aiv_bin, origin_aiv_kernel_name, i
                                )
                            )
                        else:
                            split_o_path = sub_operator.aiv_bin[:-2] + f"_split{i}.o"
                            new_kernel_name = f"{origin_aiv_kernel_name}_split{i}"
                        cur_operator_info["aiv_bin"] = split_o_path
                        new_sub_op_sub_kernel_names.append(f"{new_kernel_name}")
                    if sub_operator.aic_bin is not None:
                        if sub_operator.split_mode_in_json is None:
                            split_o_path, new_kernel_name = (
                                self.split_o_in_super_kernel(
                                    sub_operator.aic_bin, origin_aic_kernel_name, i
                                )
                            )
                        else:
                            split_o_path = sub_operator.aic_bin[:-2] + f"_split{i}.o"
                            new_kernel_name = f"{origin_aic_kernel_name}_split{i}"
                        cur_operator_info["aic_bin"] = split_o_path
                        new_sub_op_sub_kernel_names.append(f"{new_kernel_name}")
                    cur_operator_info["sub_kernel_names"] = new_sub_op_sub_kernel_names
                    sub_operator_info.append(cur_operator_info)
            elif sub_operator.split_mode > 1:
                dynamic_func_names = sub_operator.called_kernel_name[
                    "dynamic_func_names"
                ]
                kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
                rename_file_path_list = []
                for i in range(1, sub_operator.split_mode):
                    rename_file_name = f"{sub_operator.kernel_name}_rename_file_{i}.txt"
                    rename_file_path_list.append(
                        os.path.join(kernel_meta_dir, rename_file_name)
                    )
                new_kernel_names_list = gen_symbol_rename_file(
                    dynamic_func_names, rename_file_path_list, sub_operator.split_mode
                )
                orign_bin_path = operator_info["dynamic_bin"]
                for i in range(1, sub_operator.split_mode):
                    cur_operator_info = {}
                    split_o_path = split_dynamic_o_in_super_kernel(
                        orign_bin_path,
                        rename_file_path_list[i - 1],
                        i,
                        self.compile_log_path,
                    )
                    cur_operator_info["dynamic_bin"] = split_o_path
                    cur_operator_info["sub_kernel_names"] = new_kernel_names_list[i - 1]
                    sub_operator_info.append(cur_operator_info)
                if "dump_cce" in get_op_debug_config():
                    for rename_file in rename_file_path_list:
                        os.remove(rename_file)

        self.add_define_options(exist_dynamic_sub_ops, options)
        self.calc_workspace_size()
        self.compile_info = {
            "block_num": self.block_num,
            "kernel_type": self.kernel_type.value,
            "sub_operator": sub_operator_info,
            "kernel_file": self.kernel_file,
            "compile_option": options,
            "kernel_name": self.kernel_name,
            "link_mode": self.link_mode,
            "timestamp_option": self.timestamp_option,
            "debug_option": self.debug_option,
            "debug_size": self.debug_size,
            "split_mode": self.split_mode,
            "op_list": self.op_list,
            "sp_options": self.op_options,
            "workspace_size": self.workspace_size,
            "param_offset": param_offset,
            "notify_param_offset": notify_param_offset,
            "wait_param_offset": wait_param_offset,
            "send_event_list": send_event_list,
            "recv_event_list": recv_event_list,
        }
        if any(notify_before_call_event_list):
            self.compile_info["notify_before_call_param_offset"] = (
                notify_before_call_param_offset
            )
            self.compile_info["notify_before_call_event_list"] = (
                notify_before_call_event_list
            )
