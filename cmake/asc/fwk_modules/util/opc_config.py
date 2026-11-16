#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

"""Read OPC configuration and resolve target-specific compilation options."""

import os
import re
from collections import defaultdict
from typing import NamedTuple, Set

import opdesc_parser


class OpDebugConfig(NamedTuple):
    tiling_key_info: dict
    op_debug_config: dict
    kernel_json_file: dict
    input_param_file: dict
    kernel_template_input_info: dict
    simplified_key_mode_info: dict


def get_tiling_keys(tiling_keys: str) -> Set:
    all_tiling_keys = set()
    if not tiling_keys:
        return all_tiling_keys

    tiling_key_list = tiling_keys.split(";")
    for tiling_key_value in tiling_key_list:
        pattern = r"(?<![^\s])(\d+)-(\d+)(?![^\s])"
        results = re.findall(pattern, tiling_key_value)
        if results:
            start, end = results[0]
            if int(start) > int(end):
                continue
            for i in range(int(start), int(end) + 1):
                all_tiling_keys.add(i)
        elif tiling_key_value.isdigit():
            all_tiling_keys.add(int(tiling_key_value))
    return all_tiling_keys


def trans_soc_version(soc_ver: str):
    low_soc_ver = soc_ver.lower()
    if low_soc_ver not in opdesc_parser.SOC_TO_SHORT_SOC_MAP:
        return low_soc_ver
    return opdesc_parser.SOC_TO_SHORT_SOC_MAP[low_soc_ver]


def _extract_option_value(option_str):
    first_index = option_str.find("=")
    if first_index != -1:
        return option_str[first_index + 1 :]
    return ""


def _parse_soc_list(compute_unit_str):
    compute_unit_list = compute_unit_str.split(";")
    soc_lists = []
    for soc_ver in compute_unit_list:
        short_soc_ver = trans_soc_version(soc_ver)
        soc_lists.append(short_soc_ver)
    return soc_lists


def _is_soc_match(compute_unit, target_soc):
    if not compute_unit or compute_unit == "ALL":
        return True
    soc_lists = _parse_soc_list(compute_unit)
    return target_soc in soc_lists


def _process_tiling_key_option(op_type, options, tiling_key_info):
    name, separator, value = options.partition("=")
    if name != "--tiling_key":
        return
    error = (
        f"Invalid --tiling_key for operator {op_type}: {options!r}; "
        "expected non-negative integers or ascending ranges separated by ';'"
    )
    if not separator or not re.fullmatch(r"[0-9]+(?:-[0-9]+)?(?:;[0-9]+(?:-[0-9]+)?)*", value):
        raise ValueError(error)
    for item in value.split(";"):
        start, is_range, end = item.partition("-")
        if is_range and int(start) > int(end):
            raise ValueError(error)
    tiling_key_info[op_type].update(get_tiling_keys(value))


def _process_op_debug_config_option(op_type, options, op_debug_config):
    if "--op_debug_config" in options:
        debug_config = _extract_option_value(options)
        format_debug_config = set(debug_config.split(";"))
        for _config in format_debug_config:
            op_debug_config[op_type].add(_config)


def _process_kernel_json_file_option(op_type, options, kernel_json_file):
    if "--kernel-json-file" in options:
        json_file = _extract_option_value(options)
        kernel_json_file[op_type] = json_file


def _process_input_param_file_option(op_type, options, input_param_file):
    if "--input-param-file" in options:
        json_file = _extract_option_value(options)
        input_param_file[op_type] = json_file


def _process_kernel_template_input_option(op_type, options, kernel_template_input_info):
    if "--kernel-template-input" in options:
        kernel_template_input = _extract_option_value(options)
        kernel_template_input_info[op_type] = kernel_template_input


def _process_simplified_key_mode_option(op_type, options, simplified_key_mode_info):
    if "--simplified_key_mode" in options:
        simplified_key_mode_info[op_type] = _extract_option_value(options)


def _process_opc_options(
    op_type,
    opc_configs,
    soc,
    tiling_key_info,
    op_debug_config,
    kernel_json_file,
    input_param_file,
    kernel_template_input_info,
    simplified_key_mode_info,
):
    compute_unit = opc_configs[1]
    if not _is_soc_match(compute_unit, soc):
        return

    for options in opc_configs[2:]:
        _process_tiling_key_option(op_type, options, tiling_key_info)
        _process_op_debug_config_option(op_type, options, op_debug_config)
        _process_kernel_json_file_option(op_type, options, kernel_json_file)
        _process_input_param_file_option(op_type, options, input_param_file)
        _process_kernel_template_input_option(op_type, options, kernel_template_input_info)
        _process_simplified_key_mode_option(op_type, options, simplified_key_mode_info)


def parse_op_debug_config(opc_config_file: str, soc: str) -> OpDebugConfig:
    tiling_key_info = defaultdict(set)
    op_debug_config = defaultdict(set)
    kernel_json_file = defaultdict(dict)
    input_param_file = defaultdict(dict)
    kernel_template_input_info = defaultdict(dict)
    simplified_key_mode_info = defaultdict(dict)

    result = OpDebugConfig(
        tiling_key_info,
        op_debug_config,
        kernel_json_file,
        input_param_file,
        kernel_template_input_info,
        simplified_key_mode_info,
    )

    if not opc_config_file or not os.path.exists(opc_config_file):
        return result

    with open(opc_config_file, "r") as file:
        contents = file.readlines()

    for _content in contents:
        content = _content.strip()
        opc_configs = content.split("@")
        if len(opc_configs) < 3:
            continue

        op_type = opc_configs[0]
        if not op_type:
            continue

        _process_opc_options(
            op_type,
            opc_configs,
            soc,
            tiling_key_info,
            op_debug_config,
            kernel_json_file,
            input_param_file,
            kernel_template_input_info,
            simplified_key_mode_info,
        )

    return result


def gen_option_config(debug_config, super_config, op_debug_config):
    for _op_type, _op_option in op_debug_config.items():
        for _option in _op_option:
            if _option.startswith("--op_relocatable_kernel_binary") and ("false" in _option or "False" in _option):
                continue
            elif _option.startswith("--op_relocatable_kernel_binary") or _option.startswith(
                "--op_super_kernel_options"
            ):
                super_config[_op_type].add(_option)
            else:
                debug_config[_op_type].add(_option)
