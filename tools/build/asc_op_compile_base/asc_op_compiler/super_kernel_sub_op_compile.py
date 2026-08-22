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
super kernel sub op compile
"""

import os
import re
import subprocess
from asc_op_compile_base.common.context import get_context
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)
from .global_storage import global_var_storage
from .ascendc_common_utility import CommonUtility
from asc_op_compile_base.common.utils.log_utils import CompileStage, AscendCLogLevel
from .ascendc_constants import KernelMetaType, STR_TO_KERNEL_TYPE_V220
from .super_kernel_utility import run_local_cmd
from .super_kernel_constants import (
    FUNC_STR,
    OBJ_FILES_STR,
    AI_CORE_STR,
    SuperKernelStreamFusionMode,
    SuperKernelEarlyStartMode,
    SuperKernelFeedSyncAllMode,
)


def match_kernel_name_patern(op_type, op_list, use_regex=False):
    """Match op_type against op_list. If use_regex is True, treat list items as regex patterns."""
    if use_regex:
        return any(re.fullmatch(pattern, op_type) for pattern in op_list)
    return op_type in op_list


def gen_gm_get_set_value_dcci_compile_options(
    compile_option_tuple, compile_info, is_static_shape
):
    if not is_static_shape:
        # dynamic op runs offline compilation, dcci-before-kernel-start can not modify offline compilation
        # so call dcci before and after kernel by default, remove dcci in gm Get/SetValue
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_call_dcci_before_kernel_start", True
        )
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_call_dcci_after_kernel_end", True
        )
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_call_dcci_disable_on_kernel", False
        )
        return

    dcci_before_kernel_start_op_list = [
        part_op.strip()
        for part_op in compile_info.super_kernel_info["sp_options"]
        .get("dcci-before-kernel-start", "")
        .split(",")
        if part_op.strip()
    ]
    dcci_after_kernel_end_op_list = [
        part_op.strip()
        for part_op in compile_info.super_kernel_info["sp_options"]
        .get("dcci-after-kernel-end", "")
        .split(",")
        if part_op.strip()
    ]
    dcci_disable_on_kernel_op_list = [
        part_op.strip()
        for part_op in compile_info.super_kernel_info["sp_options"]
        .get("dcci-disable-on-kernel", "")
        .split(",")
        if part_op.strip()
    ]

    is_sub_combine = get_context().get_addition("super_kernel_sub_combine") is True
    match_key = compile_info.kernel_name if is_sub_combine else compile_info.op_type
    if match_kernel_name_patern(
        match_key, dcci_before_kernel_start_op_list, use_regex=is_sub_combine
    ):
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_call_dcci_before_kernel_start", True
        )
    if match_kernel_name_patern(
        match_key, dcci_after_kernel_end_op_list, use_regex=is_sub_combine
    ):
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_call_dcci_after_kernel_end", True
        )
    if match_kernel_name_patern(
        match_key, dcci_disable_on_kernel_op_list, use_regex=is_sub_combine
    ):
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_call_dcci_disable_on_kernel", True
        )

    if is_sub_combine:
        # For aclgraph+sk workflow (super_kernel_sub_combine), dcci_disable_on_kernel will not disable sub kernel
        # gm set get value dcci
        if match_kernel_name_patern(
            compile_info.kernel_name, dcci_before_kernel_start_op_list, use_regex=True
        ) or match_kernel_name_patern(
            compile_info.kernel_name, dcci_after_kernel_end_op_list, use_regex=True
        ):
            # do not call dcci in global tensor get set value, call dcci-all before or after sub kernel
            return
    else:
        if (
            compile_info.op_type in dcci_disable_on_kernel_op_list
            or compile_info.op_type in dcci_before_kernel_start_op_list
            or compile_info.op_type in dcci_after_kernel_end_op_list
        ):
            # do not call dcci in global tensor get set value, call dcci-all before or after sub kernel
            return

    # dcci-all before sub op end has been removed in sk, so call dcci in global tensor get set value by default
    # in case of performance degradation, try dcci-before-kernel-start or dcci-after-kernel-end or
    # dcci-disable-on-kernel options
    if (
        is_sub_combine
        and compile_info.super_kernel_info["sp_options"].get(
            "dcci-disable-on-kernel", ""
        )
        == ""
    ):
        return
    compile_option_tuple.compile_options.append(
        "-D__ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__"
    )


def gen_sub_super_kernel_early_start_compile_options(
    compile_option_tuple, compile_info
):
    if get_context().get_addition("super_kernel_sub_combine") is True:
        sp_options = compile_info.super_kernel_info.get("sp_options", {})
        if (
            sp_options.get("early-start", "0") == "1"
            and sp_options.get("debug-sync-all", "0") != "1"
        ):
            if (
                compile_info.super_kernel_early_start_set_flag
                or compile_info.super_kernel_early_start_wait_flag
            ):
                compile_option_tuple.compile_options.append(
                    "-D__ASCENDC_SUPERKERNEL_EARLY_START_V3"
                )
            if compile_info.super_kernel_early_start_set_flag:
                compile_option_tuple.compile_options.append(
                    "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START"
                )
                global_var_storage.set_variable(
                    "ascendc_sub_super_kernel_early_start_set_flag", True
                )
            if compile_info.super_kernel_early_start_wait_flag:
                compile_option_tuple.compile_options.append(
                    "-D__ASCENDC_ENABLE_WAIT_PRE_TASK_END"
                )
                global_var_storage.set_variable(
                    "ascendc_sub_super_kernel_early_start_wait_flag", True
                )
        return
    early_start_mode = compile_info.super_kernel_info["sp_options"].get(
        "early-start", SuperKernelEarlyStartMode.EarlyStartEnableV2
    )
    if (
        early_start_mode.value == SuperKernelEarlyStartMode.EarlyStartDisable.value
        or early_start_mode.value
        == SuperKernelEarlyStartMode.EarlyStartV2DisableSubKernel.value
    ):
        return
    sp_info = get_context().get_addition("super_kernel_sub_info")
    if len(sp_info) != 0 and "super_kernel_sub_loc" in sp_info:
        super_kernel_sub_loc = sp_info["super_kernel_sub_loc"]
        if early_start_mode.value == SuperKernelEarlyStartMode.EarlyStartEnableV1.value:
            compile_option_tuple.compile_options.append(
                "-D__ASCENDC_SUPERKERNEL_EARLY_START_V1"
            )
        elif (
            early_start_mode.value == SuperKernelEarlyStartMode.EarlyStartEnableV2.value
        ):
            compile_option_tuple.compile_options.append(
                "-D__ASCENDC_SUPERKERNEL_EARLY_START_V2"
            )

        if compile_info.super_kernel_early_start_set_flag and (
            super_kernel_sub_loc != "end"
        ):
            compile_option_tuple.compile_options.append(
                "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START"
            )
            global_var_storage.set_variable(
                "ascendc_sub_super_kernel_early_start_set_flag", True
            )
        if compile_info.super_kernel_early_start_wait_flag and (
            super_kernel_sub_loc != "start"
        ):
            compile_option_tuple.compile_options.append(
                "-D__ASCENDC_ENABLE_WAIT_PRE_TASK_END"
            )
            global_var_storage.set_variable(
                "ascendc_sub_super_kernel_early_start_wait_flag", True
            )
    else:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            ("sub super kernel compile must provide super_kernel_sub_info"),
        )


def sp_add_sub_op_block_num_macro(compile_option_tuple, tiling_info):
    if tiling_info.block_num != -1:
        # static cases
        compile_option_tuple.compile_options.append(
            f"-D__SUPER_KERNEL_STATIC_BLOCK_NUM__={tiling_info.block_num}"
        )
    else:
        # dynamic cases
        compile_option_tuple.compile_options.append(
            "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__"
        )


def sp_add_sub_op_feed_sync_all_macro(compile_info, compile_option_tuple):
    feed_sync_all_mode = compile_info.super_kernel_info["sp_options"].get(
        "feed-sync-all", SuperKernelFeedSyncAllMode.FeedSyncAllDisable
    )
    if feed_sync_all_mode.value == SuperKernelFeedSyncAllMode.FeedSyncAllEnable.value:
        compile_option_tuple.compile_options.append(
            "-D__ASCENDC_SUPERKERNEL_AUTO_SYNC_ALL__"
        )


def gen_sub_super_kernel_compile_options(
    compile_option_tuple, tiling_info, compile_info
):
    compile_option_tuple.compile_options.append("-D__ASCENDC_ENABLE_SUPER_KERNEL__")
    sp_add_sub_op_block_num_macro(compile_option_tuple, tiling_info)
    sp_add_sub_op_feed_sync_all_macro(compile_info, compile_option_tuple)
    if (
        get_context().get_addition("super_kernel_sub_combine") is True
        and compile_info.super_kernel_info["sp_options"].get(
            "debug-per-op-max-core-num", "0"
        )
        == "1"
    ):
        compile_option_tuple.compile_options.append(
            "-D__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__"
        )
    stream_fusion_mode = compile_info.super_kernel_info["sp_options"].get(
        "stream-fusion", SuperKernelStreamFusionMode.StreamFusionDisable
    )

    # dynamic can not open early start, because do not now id in graph
    if (
        stream_fusion_mode.value != SuperKernelStreamFusionMode.StreamFusionEnable.value
        and tiling_info.static_shape_flag
    ):
        gen_sub_super_kernel_early_start_compile_options(
            compile_option_tuple, compile_info
        )

    gen_gm_get_set_value_dcci_compile_options(
        compile_option_tuple, compile_info, tiling_info.static_shape_flag
    )
    return


def split_kernel(sub_kernels_dict, func_name, obj_path, split_mode, compile_log_path):
    kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
    for i in range(1, split_mode):
        filename = os.path.basename(obj_path)
        new_bin_path = os.path.join(kernel_meta_dir, filename[:-2] + f"_split{i}.o")
        if os.path.exists(new_bin_path):
            str_lst = f"ERROR: ALLREADY EXISTS split .o path: {new_bin_path}"
            CommonUtility.dump_compile_log(
                [str_lst], CompileStage.SPLIT_SUB_OBJS, compile_log_path
            )
        cmds = ["cp"] + ["-rfL"] + [f"{obj_path}"] + [f"{new_bin_path}"]
        run_local_cmd(cmds, compile_log_path)
        new_kernel_name = f"{func_name}_split{i}"
        cmds = [
            "llvm-objcopy",
            f"--redefine-sym={func_name}={new_kernel_name}",
            f"{new_bin_path}",
        ]
        run_local_cmd(cmds, compile_log_path)
        sub_kernels_dict[f"{OBJ_FILES_STR}_split{i}"] = new_bin_path


def split_kernel_arch_str(
    sub_operator_kernel_name, arch_str, split_mode, compile_log_path
):
    if arch_str in sub_operator_kernel_name:
        func_name = sub_operator_kernel_name[arch_str][FUNC_STR]
        obj_path = sub_operator_kernel_name[arch_str][OBJ_FILES_STR]
        split_kernel(
            sub_operator_kernel_name[arch_str],
            func_name,
            obj_path,
            split_mode,
            compile_log_path,
        )


def split_sub_kernel_objs(dst_file: str, tiling_info, compile_info):
    if (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and tiling_info.static_shape_flag
    ):
        compile_log_path = compile_info.compile_log_path
        chip_version = CommonUtility.get_chip_version()
        tmp_kernel_info_list = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_fun_names"
        )
        kernel_type_str = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_type"
        )
        split_mode = compile_info.super_kernel_info["sp_options"].get("split-mode", 4)
        CommonUtility.print_compile_log(
            "", f"SuperKernel split mode is {split_mode}", AscendCLogLevel.LOG_DEBUG
        )
        if split_mode is not None and split_mode > 1:
            sub_operator_kernel_type = STR_TO_KERNEL_TYPE_V220[kernel_type_str]
            if sub_operator_kernel_type in [
                KernelMetaType.KERNEL_TYPE_AIV_ONLY,
                KernelMetaType.KERNEL_TYPE_AIC_ONLY,
            ]:
                split_kernel_arch_str(
                    tmp_kernel_info_list, AI_CORE_STR, split_mode, compile_log_path
                )
            else:
                split_kernel_arch_str(
                    tmp_kernel_info_list,
                    f"dav-{chip_version}-cube",
                    split_mode,
                    compile_log_path,
                )
                split_kernel_arch_str(
                    tmp_kernel_info_list,
                    f"dav-{chip_version}-vec",
                    split_mode,
                    compile_log_path,
                )
            # to edit json
            global_var_storage.set_variable(
                "ascendc_sub_super_kernel_fun_names", tmp_kernel_info_list
            )
        tmp_obj_files = []
        temp_target_o = dst_file[:-2] + "_target.o"
        for arch in tmp_kernel_info_list:
            origin_o = tmp_kernel_info_list[arch][OBJ_FILES_STR]
            tmp_obj_files.append(origin_o)
            if split_mode is not None and split_mode > 1:
                for i in range(1, split_mode):
                    tmp_obj_files.append(
                        tmp_kernel_info_list[arch][f"{OBJ_FILES_STR}_split{i}"]
                    )
        try:
            CommonUtility.dump_compile_log(
                ["ar", "rcs", temp_target_o] + tmp_obj_files,
                CompileStage.PACK,
                compile_log_path,
            )
            subprocess.run(["ar", "rcs", temp_target_o] + tmp_obj_files)
            CommonUtility.dump_compile_log(
                ["mv", temp_target_o, dst_file], CompileStage.PACK, compile_log_path
            )
            subprocess.run(["mv", temp_target_o, dst_file])
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                ("ar sub super kernel op files failed", err),
            )
        return


def add_sub_super_kernel_info(js, static_shape_flag, compile_info):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        js["sub_operator_op_type"] = compile_info.op_type
        js["sub_operator_params"] = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_params"
        )
        js["sub_operator_kernel_type"] = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_type"
        )
        js["sub_operator_kernel_name"] = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_fun_names"
        )
        js["sub_operator_early_start_set_flag"] = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_early_start_set_flag"
        )
        js["sub_operator_early_start_wait_flag"] = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_early_start_wait_flag"
        )
        js["sub_operator_call_dcci_before_kernel_start"] = (
            global_var_storage.get_variable(
                "ascendc_sub_super_kernel_call_dcci_before_kernel_start"
            )
        )
        js["sub_operator_call_dcci_after_kernel_end"] = global_var_storage.get_variable(
            "ascendc_sub_super_kernel_call_dcci_after_kernel_end"
        )
        js["sub_operator_call_dcci_disable_on_kernel"] = (
            global_var_storage.get_variable(
                "ascendc_sub_super_kernel_call_dcci_disable_on_kernel"
            )
        )
        js["sub_op_with_sync_all"] = global_var_storage.get_variable(
            "ascendc_op_with_syncall"
        )
        if static_shape_flag:
            split_mode = compile_info.super_kernel_info["sp_options"].get(
                "split-mode", 4
            )
            if split_mode is not None:
                js["split_mode"] = split_mode
    return js


def save_kernel_type(kernel_type: str):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        global_var_storage.set_variable("ascendc_sub_super_kernel_type", kernel_type)


def gen_sub_kernel_name(
    current_kernel_name: str, arch: str, kernel_type: str, obj_files: str
):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        sp_info = get_context().get_addition("super_kernel_sub_info")
        save_kernel_type(kernel_type)
        if len(sp_info) != 0 and "super_kernel_sub_loc" in sp_info:
            super_kernel_sub_loc = sp_info["super_kernel_sub_loc"]
            new_kernel_name = current_kernel_name + "_" + str(super_kernel_sub_loc)
            tmp_kernel_info_list = global_var_storage.get_variable(
                "ascendc_sub_super_kernel_fun_names"
            )
            tmp_kernel_info_list[arch] = {
                "func_name": new_kernel_name,
                "obj_files": obj_files,
            }
            global_var_storage.set_variable(
                "ascendc_sub_super_kernel_fun_names", tmp_kernel_info_list
            )
            return new_kernel_name
        else:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                ("sub super kernel compile must provide super_kernel_sub_info"),
            )
    return current_kernel_name
