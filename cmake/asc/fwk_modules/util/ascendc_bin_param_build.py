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

import argparse
import sys
import os
import shlex
from collections import defaultdict

import opdesc_parser
from bin_param_builder import BinParamBuilder, ParamInfo
from opc_config import (
    OpDebugConfig,
    get_tiling_keys,
    trans_soc_version,
    _extract_option_value,
    _parse_soc_list,
    _is_soc_match,
    _process_tiling_key_option,
    _process_op_debug_config_option,
    _process_kernel_json_file_option,
    _process_input_param_file_option,
    _process_kernel_template_input_option,
    _process_simplified_key_mode_option,
    _process_opc_options,
    parse_op_debug_config,
    gen_option_config,
)
from ascendc_build_kernel_spec import task_directory

PYF_PATH = os.path.dirname(os.path.realpath(__file__))


def _apply_op_compile_options(op_desc, options, debug_config, super_config):
    for scope in (op_desc.op_type, "ALL"):
        if scope in debug_config:
            op_desc.set_op_debug_config(debug_config[scope])
    for scope in (op_desc.op_type, "ALL"):
        if scope in super_config:
            op_desc.set_op_super_config(super_config[scope])
    for scope in (op_desc.op_type, "ALL"):
        if scope in options.tiling_key_info:
            op_desc.set_tiling_key(options.tiling_key_info[scope])
    simplified_key_mode = None
    for scope in ("ALL", op_desc.op_type):
        if scope in options.kernel_json_file:
            op_desc.json_file = options.kernel_json_file[scope]
        if scope in options.simplified_key_mode_info:
            simplified_key_mode = options.simplified_key_mode_info[scope]
    if simplified_key_mode is not None:
        op_desc.simplified_key_mode = int(simplified_key_mode)
    for scope in ("ALL", op_desc.op_type):
        if scope in options.kernel_template_input_info:
            op_desc.set_kernel_template_input(options.kernel_template_input_info[scope])


def _set_kernel_spec_options(op_desc, kernel_spec_mode, kernel_spec_dir, soc):
    if kernel_spec_mode not in {"None", "Normal", "SK"}:
        raise ValueError("kernel-spec must be None, Normal or SK")
    op_desc.kernel_spec_options = ""
    if kernel_spec_mode != "None":
        directory = task_directory(kernel_spec_dir, op_desc.op_type, soc)
        directory.mkdir(parents=True, exist_ok=True)
        op_desc.kernel_spec_directory = directory
        op_desc.kernel_spec_options = " " + " ".join(
            shlex.quote(arg) for arg in ["--kernel-spec=" + kernel_spec_mode, "--kernel-spec-dir=" + str(directory)]
        )


def gen_bin_param_file(
    cfgfile: str,
    out_dir: str,
    soc: str,
    opc_config_file: str = "",
    ops: list = None,
    kernel_spec_mode: str = "None",
    kernel_spec_dir: str = None,
):
    if not os.path.exists(cfgfile):
        print(f"INFO: {cfgfile} does not exists in this project, skip generating compile commands.")
        return

    debug_config = defaultdict(set)
    super_config = defaultdict(set)
    op_descs = opdesc_parser.get_op_desc(cfgfile, [], [], BinParamBuilder, ops)
    options = parse_op_debug_config(opc_config_file, soc)
    gen_option_config(debug_config, super_config, options.op_debug_config)

    auto_gen_path_dir = os.path.dirname(cfgfile)
    for op_desc in op_descs:
        op_desc.set_soc_version(soc)
        op_desc.set_out_path(out_dir)
        _set_kernel_spec_options(op_desc, kernel_spec_mode, kernel_spec_dir, soc)
        _apply_op_compile_options(op_desc, options, debug_config, super_config)

        key_params = ""
        if op_desc.op_type in options.input_param_file:
            key_params = options.input_param_file[op_desc.op_type]

        if key_params:
            op_desc.gen_input_json_based_on_input_json(key_params, out_dir)
        elif op_desc.json_file == "":
            op_desc.gen_input_json(auto_gen_path_dir)
        else:
            op_desc.gen_input_json_based_on_specified_json(op_desc.json_file, out_dir)


def parse_args(argv):
    """Command line parameter parsing"""
    parser = argparse.ArgumentParser()
    parser.add_argument("argv", nargs="+")
    parser.add_argument("--opc-config-file", nargs="?", const="", default="")
    parser.add_argument("--kernel-spec-mode", "--kernel-spec", choices=("None", "Normal", "SK"), default="None")
    parser.add_argument("--kernel-spec-dir")
    return parser.parse_args(argv)


if __name__ == "__main__":
    args = parse_args(sys.argv)
    if len(args.argv) <= 3:
        raise RuntimeError("arguments must greater than 3")
    gen_bin_param_file(
        args.argv[1],
        args.argv[2],
        args.argv[3],
        opc_config_file=args.opc_config_file,
        kernel_spec_mode=args.kernel_spec_mode,
        kernel_spec_dir=args.kernel_spec_dir,
    )
