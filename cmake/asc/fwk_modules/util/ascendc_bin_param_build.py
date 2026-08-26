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
import json
import hashlib
import re
import copy
from collections import defaultdict
from typing import Dict, Set, NamedTuple

import const_var
import opdesc_parser
from split_opc_json_with_op_list import split_json_files

PYF_PATH = os.path.dirname(os.path.realpath(__file__))


class ParamInfo(NamedTuple):
    dtype_list: list
    format_list: list
    dtype_for_bin_list: dict
    format_for_bin_list: dict


class BinParamBuilder(opdesc_parser.OpDesc):
    def __init__(self: any, op_type: str):
        super().__init__(op_type)
        self.soc = ""
        self.out_path = ""
        self.tiling_keys = set()
        self.op_debug_config = ""
        self.op_super_config = []
        self.kernel_template_input = ""

    def set_soc_version(self: any, soc: str):
        self.soc = soc

    def set_out_path(self: any, out_path: str):
        self.out_path = out_path

    def set_tiling_key(self: any, tiling_key_info: Set):
        if tiling_key_info:
            self.tiling_keys.update(tiling_key_info)

    def set_kernel_template_input(self: any, kernel_template_input_info: str):
        if kernel_template_input_info:
            self.kernel_template_input = kernel_template_input_info

    def set_op_debug_config(self: any, op_debug_config: str):
        if op_debug_config:
            self.op_debug_config = op_debug_config

    def set_op_super_config(self: any, op_super_config: str):
        if op_super_config:
            self.op_super_config = op_super_config

    def get_full_list(self: any):
        dtype_list = []
        for dtype_in in self.input_dtype:
            dtype_list.append(dtype_in.split(","))
        for dtype_out in self.output_dtype:
            dtype_list.append(dtype_out.split(","))

        format_list = []
        for fmt_in in self.input_fmt:
            format_list.append(fmt_in.split(","))
        for fmt_out in self.output_fmt:
            format_list.append(fmt_out.split(","))

        dtype_for_bin_list = [
            [] for _ in range(len(self.input_dtype) + len(self.output_dtype))
        ]
        format_for_bin_list = copy.deepcopy(dtype_for_bin_list)

        for key, value in self.input_dtype_for_bin.items():
            dtype_for_bin_list[key] = value.split(",")
        for key, value in self.output_dtype_for_bin.items():
            dtype_for_bin_list[key + len(self.input_dtype)] = value.split(",")
        for key, value in self.input_fmt_for_bin.items():
            format_for_bin_list[key] = value.split(",")
        for key, value in self.output_fmt_for_bin.items():
            format_for_bin_list[key + len(self.input_dtype)] = value.split(",")

        return ParamInfo(
            dtype_list, format_list, dtype_for_bin_list, format_for_bin_list
        )

    def gen_bin_cprs_list(self: any, param_info: ParamInfo):
        combine_dict = {}
        origin_combine_dict = {}
        for cob_idx in range(0, len(self.input_dtype[0].split(","))):
            origin_combine = ""
            combine = ""
            for param_idx in range(0, len(self.input_dtype) + len(self.output_dtype)):
                if param_info.dtype_for_bin_list[param_idx]:
                    combine += param_info.dtype_for_bin_list[param_idx][cob_idx]
                else:
                    combine += param_info.dtype_list[param_idx][cob_idx]
                origin_combine += param_info.dtype_list[param_idx][cob_idx]
                if param_info.format_for_bin_list[param_idx]:
                    combine += param_info.format_for_bin_list[param_idx][cob_idx]
                else:
                    combine += param_info.format_list[param_idx][cob_idx]
                origin_combine += param_info.format_list[param_idx][cob_idx]
            if combine not in combine_dict:
                combine_dict[combine] = []
            combine_dict[combine].append(cob_idx)
            origin_combine_dict[origin_combine] = cob_idx
        for key, value in combine_dict.items():
            if key not in origin_combine_dict:
                print(f"WARNING: ForBinQuery {key} not in origin combine")
                self.bin_save_list += value
                continue
            if len(value) == 1 and value[0] == origin_combine_dict[key]:
                self.bin_save_list += value
                continue
            self.bin_cprs_head.append(origin_combine_dict[key])
            self.bin_cprs_list.append(value)
        for index, sub_list in enumerate(self.bin_cprs_list):
            if self.bin_cprs_head[index] not in self.bin_save_list:
                continue
            sub_list.append(self.bin_cprs_head[index])
        self.bin_save_list += self.bin_cprs_head

    def gen_for_bin_list(self: any, param_info: ParamInfo):
        combine_size = len(self.input_dtype[0].split(","))
        input_size = len(self.input_dtype)
        output_size = len(self.output_dtype)

        self.input_dtype_for_bin_list = [[] for _ in range(input_size)]
        self.output_dtype_for_bin_list = [[] for _ in range(output_size)]
        for i in range(0, input_size):
            self.input_dtype_for_bin_list[i] = [[] for _ in range(combine_size)]
        for i in range(0, output_size):
            self.output_dtype_for_bin_list[i] = [[] for _ in range(combine_size)]
        self.input_fmt_for_bin_list = copy.deepcopy(self.input_dtype_for_bin_list)
        self.output_fmt_for_bin_list = copy.deepcopy(self.output_dtype_for_bin_list)

        for index, sub_list in enumerate(self.bin_cprs_list):
            head_idx = self.bin_cprs_head[index]
            for cmb_idx in sub_list:
                for i in range(0, input_size):
                    self.input_dtype_for_bin_list[i][head_idx].append(
                        param_info.dtype_list[i][cmb_idx]
                    )
                    self.input_fmt_for_bin_list[i][head_idx].append(
                        param_info.format_list[i][cmb_idx]
                    )
                for i in range(0, output_size):
                    self.output_dtype_for_bin_list[i][head_idx].append(
                        param_info.dtype_list[i + input_size][cmb_idx]
                    )
                    self.output_fmt_for_bin_list[i][head_idx].append(
                        param_info.format_list[i + input_size][cmb_idx]
                    )

    def rm_cprs_cmb(self: any, dtype_list, format_list, input_size, output_size):
        for i in range(0, input_size):
            self.input_dtype_for_bin_list[i] = [
                element
                for index, element in enumerate(self.input_dtype_for_bin_list[i])
                if index in self.bin_save_list
            ]
            self.input_fmt_for_bin_list[i] = [
                element
                for index, element in enumerate(self.input_fmt_for_bin_list[i])
                if index in self.bin_save_list
            ]
            new_dtype_list = [
                element
                for index, element in enumerate(dtype_list[i])
                if index in self.bin_save_list
            ]
            new_dtype_str = ""
            for dtype in new_dtype_list:
                new_dtype_str += f"{dtype},"
            self.input_dtype[i] = new_dtype_str[:-1]
            new_format_list = [
                element
                for index, element in enumerate(format_list[i])
                if index in self.bin_save_list
            ]
            new_format_str = ""
            for fmt in new_format_list:
                new_format_str += f"{fmt},"
            self.input_fmt[i] = new_format_str[:-1]
        for i in range(0, output_size):
            self.output_dtype_for_bin_list[i] = [
                element
                for index, element in enumerate(self.output_dtype_for_bin_list[i])
                if index in self.bin_save_list
            ]
            self.output_fmt_for_bin_list[i] = [
                element
                for index, element in enumerate(self.output_fmt_for_bin_list[i])
                if index in self.bin_save_list
            ]
            new_dtype_list = [
                element
                for index, element in enumerate(dtype_list[i + input_size])
                if index in self.bin_save_list
            ]
            new_dtype_str = ""
            for dtype in new_dtype_list:
                new_dtype_str += f"{dtype},"
            self.output_dtype[i] = new_dtype_str[:-1]
            new_format_list = [
                element
                for index, element in enumerate(format_list[i + input_size])
                if index in self.bin_save_list
            ]
            new_format_str = ""
            for fmt in new_format_list:
                new_format_str += f"{fmt},"
            self.output_fmt[i] = new_format_str[:-1]

    def is_set_for_bin_query(self: any):
        return any(
            [
                self.input_dtype_for_bin,
                self.output_dtype_for_bin,
                self.input_fmt_for_bin,
                self.output_fmt_for_bin,
            ]
        )

    def for_bin_list_match(self: any):
        if not self.is_set_for_bin_query():
            return
        input_size = len(self.input_dtype)
        output_size = len(self.output_dtype)
        param_info = self.get_full_list()
        self.gen_bin_cprs_list(param_info)
        self.gen_for_bin_list(param_info)
        if len(self.bin_save_list) == len(self.input_dtype[0].split(",")):
            print(
                "WARNING: ForBinQuery can not compress number of bin file with this set, please check!!."
            )
            return
        self.rm_cprs_cmb(
            param_info.dtype_list, param_info.format_list, input_size, output_size
        )

    def gen_input_json_with_super_config(self: any, param_file: str):
        with open(param_file, "r") as f:
            data = json.load(f)

        if "op_list" in data and isinstance(data["op_list"], list):
            for op_node in data["op_list"]:
                if "bin_filename" in op_node:
                    op_node["bin_filename"] += "_relocatable"
                else:
                    print(
                        f"[WARNING]The op_node in {param_file} does not have bin_filename field, please check!!!"
                    )
        else:
            print(
                f"[WARNING]The op_list in {param_file} is empty or not list type, please check!!!"
            )

        base_name = os.path.basename(param_file)
        name, ext = os.path.splitext(base_name)
        new_file_name = f"{name}_relocatable.json"
        new_file_path = os.path.join(os.path.dirname(param_file), new_file_name)

        with open(new_file_path, "w") as f:
            json.dump(data, f, indent="  ")
        return new_file_path

    def gen_input_json_based_on_input_json(self: any, ori_json: str, output_dir: str):
        generated_files = split_json_files(ori_json, output_dir)
        index_value = -1
        if generated_files:
            for param_file in generated_files:
                index_value += 1
                file_name_with_ext = os.path.basename(param_file)
                bin_file, _ = os.path.splitext(file_name_with_ext)
                self._write_build_cmd(param_file, bin_file, index_value, output_dir)
                if self.op_super_config:
                    index_value += 1
                    new_param_file = self.gen_input_json_with_super_config(param_file)
                    bin_file += "_relocatable"
                    self._write_build_cmd(
                        new_param_file, bin_file, index_value, output_dir, True
                    )
        else:
            print(
                "[ERROR]the generation of input_json based on specified json is failed"
            )

    def gen_input_json_based_on_specified_json(
        self: any, ori_json: str, output_dir: str
    ):
        generated_files = split_json_files(ori_json, output_dir)
        index_value = -1
        if generated_files:
            for param_file in generated_files:
                index_value += 1
                file_name_with_ext = os.path.basename(param_file)
                bin_file, ext = os.path.splitext(file_name_with_ext)
                self._write_build_cmd(param_file, bin_file, index_value, output_dir)
                if self.op_super_config:
                    index_value += 1
                    new_param_file = self.gen_input_json_with_super_config(param_file)
                    bin_file += "_relocatable"
                    self._write_build_cmd(
                        new_param_file, bin_file, index_value, output_dir, True
                    )
        else:
            print(
                "[ERROR]the generation of input_json based on specified json is failed"
            )

    def _gen_inputs_for_input_json(self: any, index, required_parameter, inputs):
        input_name_len = len(self.input_name)
        for idx in range(0, input_name_len):
            idtypes = self.input_dtype[idx].split(",")
            ifmts = self.input_fmt[idx].split(",")
            itype = self.input_type[idx]
            para = {}
            para["name"] = self.input_name[idx][:-5]
            para["index"] = idx
            para["dtype"] = idtypes[index]
            if (
                self.is_set_for_bin_query()
                and self.input_dtype_for_bin_list[idx][index]
            ):
                para["dtypeForBinQuery"] = self.input_dtype_for_bin_list[idx][index]
            para["format"] = ifmts[index]
            if self.is_set_for_bin_query() and self.input_fmt_for_bin_list[idx][index]:
                para["formatForBinQuery"] = self.input_fmt_for_bin_list[idx][index]
            para["paramType"] = itype
            para["shape"] = [-2]
            para["format_match_mode"] = "FormatAgnostic"

            input_parameter_key = (idtypes[index], ifmts[index])
            if itype == "dynamic":
                inputs.append([para])
                required_parameter.append(input_parameter_key)
            elif itype == "required":
                inputs.append(para)
                required_parameter.append(input_parameter_key)
            else:
                inputs.append(para)

    def _gen_outputs_for_input_json(self: any, index, required_parameter, outputs):
        output_name_len = len(self.output_name)
        for idx in range(0, output_name_len):
            odtypes = self.output_dtype[idx].split(",")
            ofmts = self.output_fmt[idx].split(",")
            otype = self.output_type[idx]
            para = {}
            para["name"] = self.output_name[idx][:-5]
            para["index"] = idx
            para["dtype"] = odtypes[index]
            if (
                self.is_set_for_bin_query()
                and self.output_dtype_for_bin_list[idx][index]
            ):
                para["dtypeForBinQuery"] = self.output_dtype_for_bin_list[idx][index]
            para["format"] = ofmts[index]
            if self.is_set_for_bin_query() and self.output_fmt_for_bin_list[idx][index]:
                para["formatForBinQuery"] = self.output_fmt_for_bin_list[idx][index]
            para["paramType"] = otype
            para["shape"] = [-2]
            para["format_match_mode"] = "FormatAgnostic"
            output_parameter_key = (odtypes[index], ofmts[index])
            if otype == "dynamic":
                outputs.append([para])
                required_parameter.append(output_parameter_key)
            elif otype == "required":
                outputs.append(para)
                required_parameter.append(output_parameter_key)
            else:
                outputs.append(para)

    def _gen_attrs_for_input_json(self: any, attrs):
        for attr in self.attr_list:
            att = {}
            att["name"] = attr
            atype = self.attr_val.get(attr).get("type").lower()
            att["dtype"] = atype
            att["value"] = const_var.ATTR_DEF_VAL.get(atype)
            attrs.append(att)

    def gen_input_json(self: any, auto_gen_path: str):
        key_map = {}
        if len(self.input_dtype) == 0 and len(self.output_dtype) == 0:
            count = 1
        else:
            self.for_bin_list_match()
            if len(self.input_dtype) == 0:
                count = len(self.output_dtype[0].split(","))
            else:
                count = len(self.input_dtype[0].split(","))
            if count == 0:
                raise RuntimeError(
                    f"Op {self.op_type} must have at least one input or output"
                )
        required_parameters = set()
        index_value = -1

        for i in range(0, count):
            inputs = []
            outputs = []
            attrs = []
            required_parameter = []
            op_node = {}

            self._gen_inputs_for_input_json(i, required_parameter, inputs)
            self._gen_outputs_for_input_json(i, required_parameter, outputs)
            self._gen_attrs_for_input_json(attrs)

            required_parameter_tuple = tuple(required_parameter)
            if required_parameter_tuple in required_parameters:
                continue
            else:
                required_parameters.add(required_parameter_tuple)
                index_value += 1

            op_node["bin_filename"] = ""
            op_node["inputs"] = inputs
            op_node["outputs"] = outputs
            if len(attrs) > 0:
                op_node["attrs"] = attrs

            param = {}
            param["op_type"] = self.op_type
            param["op_list"] = [op_node]
            objstr = json.dumps(param, indent="  ")
            md5sum = hashlib.md5(objstr.encode("utf-8")).hexdigest()
            while key_map.get(md5sum) is not None:
                objstr += "1"
                md5sum = hashlib.md5(objstr.encode("utf-8")).hexdigest()
            key_map[md5sum] = md5sum
            bin_file = self.op_type + "_" + md5sum
            op_node["bin_filename"] = bin_file
            param_file = os.path.join(self.out_path, bin_file + "_param.json")
            param_file = os.path.realpath(param_file)

            self._write_build_json(param_file, param)
            self._write_build_cmd(param_file, bin_file, index_value, auto_gen_path)
            if self.op_super_config:
                bin_file += "_relocatable"
                op_node["bin_filename"] = bin_file
                param_file = os.path.join(self.out_path, bin_file + "_param.json")
                param_file = os.path.realpath(param_file)
                self._write_build_json(param_file, param)
                index_value += 1
                self._write_build_cmd(
                    param_file, bin_file, index_value, auto_gen_path, True
                )

    def _write_build_json(self: any, param_file: str, param):
        with os.fdopen(
            os.open(param_file, const_var.WFLAGS, const_var.WMODES), "w"
        ) as fd:
            json.dump(param, fd, indent="  ")

    def _generate_check_result(self: any, enable_tiling_keys: bool, bin_file: str):
        check_result = ""
        if enable_tiling_keys is False:
            check_result += 'echo "${res}"\n'
            check_result += const_var.CHK_CMD.format(res_file=bin_file + ".json")
            check_result += const_var.CHK_CMD.format(res_file=bin_file + ".o")
        else:
            check_result += "if [ $? -eq 1 ]; then\n"
            check_result += '    if echo "${res}" | \
grep -q "None of the given tiling keys are in the supported list"; then\n'
            check_result += '        echo "${res}"\n'
            check_result += "    else\n"
            check_result += '        echo "${res}"\n'
            check_result += "        exit 1\n"
            check_result += "    fi\n"
            check_result += "else\n"
            check_result += 'echo "${res}"\n'
            check_result += const_var.CHK_CMD.format(res_file=bin_file + ".json")
            check_result += const_var.CHK_CMD.format(res_file=bin_file + ".o")
            check_result += "fi\n"
        return check_result

    def _write_build_cmd(
        self: any,
        param_file: str,
        bin_file: str,
        index: int,
        auto_gen_path: str,
        super_mode=False,
    ):
        hard_soc = const_var.conv_soc_ver(self.soc)
        if not hard_soc:
            hard_soc = self.soc.capitalize()
        if os.path.dirname(self.op_file) != "":
            name_com = [self.op_type, os.path.basename(self.op_file), str(index)]
        else:
            name_com = [self.op_type, self.op_file, str(index)]
        compile_file = os.path.join(self.out_path, "-".join(name_com) + ".sh")
        compile_file = os.path.realpath(compile_file)

        bin_cmd_str = "res=$(asc_opc $1 --main_func={fun} --input_param={param} --soc_version={soc} \
                --output=$2 --impl_mode={impl} --simplified_key_mode=0 --op_mode=dynamic "

        build_cmd_var = "#!/bin/bash\n"
        build_cmd_var += f'echo "[{self.soc}] Generating {bin_file} ..."\n'
        plog_level = os.environ.get("ASCEND_GLOBAL_LOG_LEVEL")
        plog_stdout = os.environ.get("ASCEND_SLOG_PRINT_TO_STDOUT")
        if plog_level is None:
            build_cmd_var += const_var.SET_PLOG_LEVEL_ERROR
        if plog_stdout is None:
            build_cmd_var += const_var.SET_PLOG_STDOUT
        build_cmd_var += const_var.SRC_ENV
        if hard_soc == "Ascend610Lite":
            build_cmd_var += f"export ASCEND_CUSTOM_OPP_PATH={auto_gen_path}:$ASCEND_CUSTOM_OPP_PATH \n"
        build_cmd_var += bin_cmd_str.format(
            fun=self.op_intf,
            soc=hard_soc,
            param=param_file,
            impl="high_performance,optional",
        )
        enable_tiling_keys = False
        if self.tiling_keys:
            tiling_keys_list = sorted(list(self.tiling_keys))
            tiling_key_str = ",".join([str(_key) for _key in tiling_keys_list])
            build_cmd_var += f' --tiling_key="{tiling_key_str}"'
            enable_tiling_keys = True

        if self.op_debug_config:
            op_debug_str = ",".join([str(_key) for _key in list(self.op_debug_config)])
            build_cmd_var += f" --op_debug_config={op_debug_str}"

        if super_mode and self.op_super_config:
            op_super_config_str = " ".join(
                [str(_key) for _key in list(self.op_super_config)]
            )
            build_cmd_var += f" {op_super_config_str}"

        if self.kernel_template_input:
            kernel_template_input_str = self.kernel_template_input
            build_cmd_var += f" --kernel-template-input={kernel_template_input_str}"

        build_cmd_var += ")\n"
        build_cmd_var += "\n"

        check_result = self._generate_check_result(enable_tiling_keys, bin_file)
        build_cmd_var += check_result
        build_cmd_var += f'echo "[{self.soc}] Generating {bin_file} Done"\n'

        with os.fdopen(
            os.open(compile_file, const_var.WFLAGS, const_var.WMODES), "w"
        ) as fd:
            fd.write(build_cmd_var)


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
    if not compute_unit:
        return True
    soc_lists = _parse_soc_list(compute_unit)
    return target_soc in soc_lists


def _process_tiling_key_option(op_type, options, tiling_key_info):
    if "--tiling_key" in options:
        format_tiling_keys = get_tiling_keys(options.split("=")[1])
        if format_tiling_keys:
            tiling_key_info[op_type].update(format_tiling_keys)


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


def _process_opc_options(
    op_type,
    opc_configs,
    soc,
    tiling_key_info,
    op_debug_config,
    kernel_json_file,
    input_param_file,
    kernel_template_input_info,
):
    compute_unit = opc_configs[1]
    if not _is_soc_match(compute_unit, soc):
        return

    for options in opc_configs[2:]:
        _process_tiling_key_option(op_type, options, tiling_key_info)
        _process_op_debug_config_option(op_type, options, op_debug_config)
        _process_kernel_json_file_option(op_type, options, kernel_json_file)
        _process_input_param_file_option(op_type, options, input_param_file)
        _process_kernel_template_input_option(
            op_type, options, kernel_template_input_info
        )


def parse_op_debug_config(opc_config_file: str, soc: str) -> Dict:
    tiling_key_info = defaultdict(set)
    op_debug_config = defaultdict(set)
    kernel_json_file = defaultdict(dict)
    input_param_file = defaultdict(dict)
    kernel_template_input_info = defaultdict(dict)

    if not opc_config_file or not os.path.exists(opc_config_file):
        return (
            tiling_key_info,
            op_debug_config,
            kernel_json_file,
            input_param_file,
            kernel_template_input_info,
        )

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
        )

    return (
        tiling_key_info,
        op_debug_config,
        kernel_json_file,
        input_param_file,
        kernel_template_input_info,
    )


def gen_option_config(debug_config, super_config, op_debug_config):
    for _op_type, _op_option in op_debug_config.items():
        for _option in _op_option:
            if _option.startswith("--op_relocatable_kernel_binary") and (
                "false" in _option or "False" in _option
            ):
                continue
            elif _option.startswith(
                "--op_relocatable_kernel_binary"
            ) or _option.startswith("--op_super_kernel_options"):
                super_config[_op_type].add(_option)
            else:
                debug_config[_op_type].add(_option)


def gen_bin_param_file(
    cfgfile: str, out_dir: str, soc: str, opc_config_file: str = "", ops: list = None
):
    if not os.path.exists(cfgfile):
        print(
            f"INFO: {cfgfile} does not exists in this project, skip generating compile commands."
        )
        return

    debug_config = defaultdict(set)
    super_config = defaultdict(set)
    op_descs = opdesc_parser.get_op_desc(cfgfile, [], [], BinParamBuilder, ops)
    (
        tiling_key_info,
        op_debug_config,
        kernel_json_file,
        input_param_file,
        kernel_template_input_info,
    ) = parse_op_debug_config(opc_config_file, soc)
    gen_option_config(debug_config, super_config, op_debug_config)

    auto_gen_path_dir = os.path.dirname(cfgfile)
    all_soc_key = "ALL"
    for op_desc in op_descs:
        op_desc.set_soc_version(soc)
        op_desc.set_out_path(out_dir)
        if op_desc.op_type in debug_config:
            op_desc.set_op_debug_config(debug_config[op_desc.op_type])
        if all_soc_key in debug_config:
            op_desc.set_op_debug_config(debug_config[all_soc_key])
        if op_desc.op_type in super_config:
            op_desc.set_op_super_config(super_config[op_desc.op_type])
        if all_soc_key in super_config:
            op_desc.set_op_super_config(super_config[all_soc_key])
        if op_desc.op_type in tiling_key_info:
            op_desc.set_tiling_key(tiling_key_info[op_desc.op_type])
        if all_soc_key in tiling_key_info:
            op_desc.set_tiling_key(tiling_key_info[all_soc_key])
        if op_desc.op_type in kernel_json_file:
            op_desc.json_file = kernel_json_file[op_desc.op_type]
        if all_soc_key in kernel_json_file:
            op_desc.json_file = kernel_json_file[all_soc_key]
        if all_soc_key in kernel_template_input_info:
            op_desc.set_kernel_template_input(kernel_template_input_info[all_soc_key])
        if op_desc.op_type in kernel_template_input_info:
            op_desc.set_kernel_template_input(
                kernel_template_input_info[op_desc.op_type]
            )

        key_params = ""
        if op_desc.op_type in input_param_file:
            key_params = input_param_file[op_desc.op_type]

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
    return parser.parse_args(argv)


if __name__ == "__main__":
    args = parse_args(sys.argv)
    if len(args.argv) <= 3:
        raise RuntimeError("arguments must greater than 3")
    gen_bin_param_file(
        args.argv[1], args.argv[2], args.argv[3], opc_config_file=args.opc_config_file
    )
