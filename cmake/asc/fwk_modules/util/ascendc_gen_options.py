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

import sys
import stat
import os

from opdesc_parser import _trans_short_soc_to_soc_version


def write_options_to_file(
    file_name: str, options_str: str, op_type: str, compute_unit: str, split_char: str
):
    flags = os.O_WRONLY | os.O_CREAT
    modes = stat.S_IWUSR | stat.S_IRUSR
    try:
        with os.fdopen(os.open(file_name, flags, modes), "a") as fd:
            fd.write(
                op_type + split_char + compute_unit + split_char + options_str + "\n"
            )
    except Exception as err:
        print("write compile options config file failed")
        raise (err)


def _handle_oom_option(opts, opc_debug_config):
    if opts == "--oom":
        opc_debug_config.append("oom")
    else:
        raise RuntimeError(f"Unknown oom option format {opts}")


def _handle_input_param_file(opts):
    """Handle input param file option."""
    return "--input-param-file=" + opts.strip().split("=")[1]


def _handle_tiling_key(opts):
    keys = opts.strip().split("=")[1].split(",")
    return ";".join([key for key in keys])


def _handle_kernel_template_input(opts):
    if "=" not in opts:
        raise RuntimeError("Invalid --kernel-template-input option format!")
    input_value = opts.split("=", 1)
    if not input_value[1]:
        raise RuntimeError("No value given for --kernel-template-input option!")
    return input_value[1]


def _parse_single_option(opts, opc_debug_config, opc_kernel_config):
    if "oom" in opts:
        _handle_oom_option(opts, opc_debug_config)
        return "debug_config", None
    elif "--save-temp-files" in opts:
        opc_debug_config.append("dump_cce")
        return "debug_config", None
    elif "--input-param-file" in opts:
        return "input_param", _handle_input_param_file(opts)
    elif opts.startswith("--op_relocatable_kernel_binary"):
        opc_debug_config.append(opts)
        return "debug_config", None
    elif opts.startswith("--op_super_kernel_options"):
        opc_debug_config.append(opts)
        return "debug_config", None
    elif opts.startswith("--kernel-json-file"):
        opc_kernel_config.append(opts)
        return "kernel_config", None
    elif opts.startswith("--simplified_key_mode"):
        opc_kernel_config.append(opts)
        return "kernel_config", None
    elif "--tiling_key" in opts:
        return "tiling_key", _handle_tiling_key(opts)
    elif "--kernel-template-input" in opts:
        return "template_input", _handle_kernel_template_input(opts)
    else:
        return "compile_opt", opts


def _parse_compile_options(compile_options):
    compile_opt = []
    opc_debug_config = []
    opc_kernel_config = []
    opc_tiling_keys = ""
    input_param_file = ""
    opc_template_kernel_str = ""

    for opts in compile_options:
        category, value = _parse_single_option(
            opts, opc_debug_config, opc_kernel_config
        )
        if category == "compile_opt":
            compile_opt.append(opts)
        elif category == "input_param":
            input_param_file = value
        elif category == "tiling_key":
            opc_tiling_keys = value
        elif category == "template_input":
            opc_template_kernel_str = value

    return (
        compile_opt,
        opc_debug_config,
        opc_kernel_config,
        opc_tiling_keys,
        input_param_file,
        opc_template_kernel_str,
    )


def _build_opc_config_str(
    opc_debug_config, opc_tiling_keys, opc_template_kernel_str, opc_kernel_config
):
    opc_config_str = ""
    if opc_debug_config:
        opc_config_str = "--op_debug_config=" + ";".join(
            [opt for opt in opc_debug_config]
        )
    if len(opc_tiling_keys) > 0:
        if opc_config_str != "":
            opc_config_str += "@"
        opc_config_str += "--tiling_key=" + opc_tiling_keys
    if len(opc_template_kernel_str) > 0:
        if opc_config_str != "":
            opc_config_str += "@"
        opc_config_str += "--kernel-template-input=" + opc_template_kernel_str
    if opc_kernel_config:
        if opc_config_str != "":
            opc_config_str += "@"
        opc_config_str += "@".join(opc_kernel_config)
    return opc_config_str


def _write_compile_options(compile_options_file, compile_opt, op_type, compute_unit):
    if len(compile_opt) > 0:
        options_str = ";".join([opt for opt in compile_opt])
        write_options_to_file(
            compile_options_file, options_str, op_type, compute_unit, ","
        )


def _write_opc_config(opc_config_file, opc_config_str, op_type, compute_unit):
    if opc_config_str != "":
        write_options_to_file(
            opc_config_file, opc_config_str, op_type, compute_unit, "@"
        )


def _write_input_param_file(opc_config_file, input_param_file, op_type, compute_unit):
    if input_param_file != "":
        if op_type == "ALL" or compute_unit == "":
            raise RuntimeError(
                "--input-param-file must be used with a COMPUTE_UNIT, and OP_TYPE cannot be ALL."
            )
        write_options_to_file(
            opc_config_file, input_param_file, op_type, compute_unit, "@"
        )


def gen_compile_options(
    compile_options_file: str, op_type: str, compute_unit: str, compile_options: list
):
    base_dir = os.path.dirname(compile_options_file)
    opc_config_file = os.path.join(base_dir, "custom_opc_options.ini")

    (
        compile_opt,
        opc_debug_config,
        opc_kernel_config,
        opc_tiling_keys,
        input_param_file,
        opc_template_kernel_str,
    ) = _parse_compile_options(compile_options)

    _write_compile_options(compile_options_file, compile_opt, op_type, compute_unit)

    opc_config_str = _build_opc_config_str(
        opc_debug_config, opc_tiling_keys, opc_template_kernel_str, opc_kernel_config
    )
    _write_opc_config(opc_config_file, opc_config_str, op_type, compute_unit)
    _write_input_param_file(opc_config_file, input_param_file, op_type, compute_unit)


def parse_options(args):
    result = []
    current = ""
    in_quotes = False
    quote_char = None

    for arg in args:
        for ch in arg:
            if ch in ['"', "'"]:
                if in_quotes and ch == quote_char:
                    in_quotes = False
                    quote_char = None
                else:
                    in_quotes = True
                    quote_char = ch
            if ch == ";" and not in_quotes:
                result.append(current)
                current = ""
            else:
                current += ch
        if not in_quotes:
            result.append(current)
            current = ""
        else:
            pass
    if current:
        result.append(current)
    return result


if __name__ == "__main__":
    if len(sys.argv) < 4:
        raise RuntimeError("arguments must greater than 4")
    compute_soc = ""
    soc_series = ""
    comp_options = []
    for i in range(len(sys.argv) - 3):
        arg = sys.argv[i + 3]
        if arg.startswith("--soc-series="):
            soc_series = arg.split("=", 1)[1].replace(",", ";")
            if not soc_series:
                raise RuntimeError("No value given for --soc-series")
        elif arg.upper().startswith("ASCEND"):
            compute_soc += arg + ";"
        elif arg.upper().startswith("KIRIN"):
            compute_soc += arg + ";"
        elif arg.upper().startswith("MC62"):
            compute_soc += arg + ";"
        else:
            comp_options.append(arg)
    if soc_series and compute_soc:
        raise RuntimeError(
            "SOC_SERIES and COMPUTE_UNIT cannot be used at the same time"
        )
    if soc_series:
        compute_soc = ";".join(
            _trans_short_soc_to_soc_version(soc_ver)
            for soc_ver in soc_series.split(";")
        )
    elif compute_soc != "":
        compute_soc = compute_soc[0:-1]
    final_options = []
    for opt in comp_options:
        final_options.extend(parse_options([opt]))
    gen_compile_options(sys.argv[1], sys.argv[2], compute_soc, final_options)
