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
super kernel utility
"""

# import all func or types for sk, if need delete grep super kernel code in 4.1.2
import enum
import re
from tbe.common.platform.platform_info import get_soc_spec
from tbe.common.buildcfg import get_current_build_config
from tbe.common.buildcfg.buildcfg_mapping import op_debug_config
from .ascendc_constants import KernelMetaType, STR_TO_KERNEL_TYPE_V220
from .ascendc_common_utility import CommonUtility, gen_func_align_attribute
from .ascendc_compile_base import search_in_line, extract_file_path
from .log_utils import AscendCLogLevel, CompileStage
from .super_kernel_constants import ERR_CODE


def get_op_debug_config():
    return get_current_build_config(op_debug_config)


def run_local_cmd(cmds, compile_log_path=None):
    output, ret_code = CommonUtility.run_cmd_ascendc(cmds, "compile")
    CommonUtility.dump_build_log(
        output.decode(), cmds, CompileStage.SPLIT_SUB_OBJS, ret_code
    )
    CommonUtility.dump_compile_log(cmds, CompileStage.SPLIT_SUB_OBJS, compile_log_path)


def get_wait_flag_for_chip(flag_id):
    if CommonUtility.is_c310():
        return f"wait_flag_dev(PIPE_S, {flag_id});"
    return f"wait_flag_dev({flag_id});"


def check_exist_forbidden_symbols(dst_i_file, forbidden_symbols, allow_path):
    need_check: bool = True
    block_file_path = ""
    result_symbol_list = []
    path_list = []
    line_result = []
    forbidden_pattern = re.compile(
        r"\b(" + "|".join(re.escape(symbol) for symbol in forbidden_symbols) + r")\b"
    )
    allow_path_pattern = re.compile(
        r"\b(" + "|".join(re.escape(path) for path in allow_path) + r")\b"
    )
    with open(dst_i_file, "r") as f:
        for line in f:
            if line.startswith("#"):
                # Track the current preprocessed source context for subsequent lines.
                need_check = True
                if allow_path_pattern.search(line):
                    need_check = False
                block_file_path = line
                continue
            if not need_check:
                continue
            if not any(symbol in line for symbol in forbidden_symbols):
                continue
            matches = forbidden_pattern.findall(line)
            if matches:
                result_symbol_list.append(", ".join(matches))
                path_list.append(extract_file_path(block_file_path))
                line_result.append(line)
    return result_symbol_list, path_list, line_result


_FORBIDDEN_API_REPLACEMENT = {
    "get_block_idx": "AscendC::GetBlockIdx()",
    "get_block_num": "AscendC::GetBlockNum()",
    "get_task_ration": "AscendC::GetTaskRatio()",
    "block_idx": "AscendC::GetBlockIdx()",
}


def check_exist_instrinsic_when_super_kernel(dst_i_file):
    forbidden_apis = list(_FORBIDDEN_API_REPLACEMENT.keys())
    allow_path_mark = [
        "bisheng_compiler",
        "ccec_compiler",
        "tikcpp/tikcfw",
        "asc/impl",
        "impl/basic_api",
        "impl/c_api",
    ]
    result_symbol_list, path_list, line_result = check_exist_forbidden_symbols(
        dst_i_file, forbidden_apis, allow_path_mark
    )
    result_str = ""
    len_result_symbol_list = len(result_symbol_list)
    if len_result_symbol_list != 0:
        result_str += f"ERROR({len_result_symbol_list}): "
        for i, result in enumerate(result_symbol_list):
            apis = list(dict.fromkeys([a.strip() for a in result.split(",")]))
            replacements = list(
                dict.fromkeys(
                    [
                        _FORBIDDEN_API_REPLACEMENT.get(api, "")
                        for api in apis
                        if _FORBIDDEN_API_REPLACEMENT.get(api, "")
                    ]
                )
            )
            apis_str = " or ".join(apis)
            replacements_str = " and ".join(replacements)
            result_str += (
                f"Use of {apis_str} {'are' if len(apis) > 1 else 'is'} not allowed in SuperKernel"
                f", please use {replacements_str} instead (op: {path_list[i]}"
                f", line: {line_result[i]})"
            )
            if i != len_result_symbol_list - 1:
                result_str += "\n"
        CommonUtility().ascendc_raise_python_err(ERR_CODE, result_str)
