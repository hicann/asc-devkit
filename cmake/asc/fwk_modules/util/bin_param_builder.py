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

"""Generate binary parameter JSON and compiler shell scripts for an operator."""

import copy
import hashlib
import json
import os
from pathlib import Path
import shlex
from typing import NamedTuple, Set

import const_var
import opdesc_parser
from split_opc_json_with_op_list import split_json_files


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
        self.simplified_key_mode = 0
        self.kernel_spec_options = ""
        self._kernel_spec_skip_logged = False

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

        dtype_for_bin_list = [[] for _ in range(len(self.input_dtype) + len(self.output_dtype))]
        format_for_bin_list = copy.deepcopy(dtype_for_bin_list)

        for key, value in self.input_dtype_for_bin.items():
            dtype_for_bin_list[key] = value.split(",")
        for key, value in self.output_dtype_for_bin.items():
            dtype_for_bin_list[key + len(self.input_dtype)] = value.split(",")
        for key, value in self.input_fmt_for_bin.items():
            format_for_bin_list[key] = value.split(",")
        for key, value in self.output_fmt_for_bin.items():
            format_for_bin_list[key + len(self.input_dtype)] = value.split(",")

        return ParamInfo(dtype_list, format_list, dtype_for_bin_list, format_for_bin_list)

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
                    self.input_dtype_for_bin_list[i][head_idx].append(param_info.dtype_list[i][cmb_idx])
                    self.input_fmt_for_bin_list[i][head_idx].append(param_info.format_list[i][cmb_idx])
                for i in range(0, output_size):
                    self.output_dtype_for_bin_list[i][head_idx].append(param_info.dtype_list[i + input_size][cmb_idx])
                    self.output_fmt_for_bin_list[i][head_idx].append(param_info.format_list[i + input_size][cmb_idx])

    def rm_cprs_cmb(self: any, dtype_list, format_list, input_size, output_size):
        for i in range(0, input_size):
            self.input_dtype_for_bin_list[i] = [
                element for index, element in enumerate(self.input_dtype_for_bin_list[i]) if index in self.bin_save_list
            ]
            self.input_fmt_for_bin_list[i] = [
                element for index, element in enumerate(self.input_fmt_for_bin_list[i]) if index in self.bin_save_list
            ]
            new_dtype_list = [element for index, element in enumerate(dtype_list[i]) if index in self.bin_save_list]
            new_dtype_str = ""
            for dtype in new_dtype_list:
                new_dtype_str += f"{dtype},"
            self.input_dtype[i] = new_dtype_str[:-1]
            new_format_list = [element for index, element in enumerate(format_list[i]) if index in self.bin_save_list]
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
                element for index, element in enumerate(self.output_fmt_for_bin_list[i]) if index in self.bin_save_list
            ]
            new_dtype_list = [
                element for index, element in enumerate(dtype_list[i + input_size]) if index in self.bin_save_list
            ]
            new_dtype_str = ""
            for dtype in new_dtype_list:
                new_dtype_str += f"{dtype},"
            self.output_dtype[i] = new_dtype_str[:-1]
            new_format_list = [
                element for index, element in enumerate(format_list[i + input_size]) if index in self.bin_save_list
            ]
            new_format_str = ""
            for fmt in new_format_list:
                new_format_str += f"{fmt},"
            self.output_fmt[i] = new_format_str[:-1]

    def is_set_for_bin_query(self: any):
        return any(
            [self.input_dtype_for_bin, self.output_dtype_for_bin, self.input_fmt_for_bin, self.output_fmt_for_bin]
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
            print("WARNING: ForBinQuery can not compress number of bin file with this set, please check!!.")
            return
        self.rm_cprs_cmb(param_info.dtype_list, param_info.format_list, input_size, output_size)

    def gen_input_json_with_super_config(self: any, param_file: str):
        with open(param_file, "r") as f:
            data = json.load(f)

        if "op_list" in data and isinstance(data["op_list"], list):
            for op_node in data["op_list"]:
                if "bin_filename" in op_node:
                    op_node["bin_filename"] += "_relocatable"
                else:
                    print(f"[WARNING]The op_node in {param_file} does not have bin_filename field, please check!!!")
        else:
            print(f"[WARNING]The op_list in {param_file} is empty or not list type, please check!!!")

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
                    self._write_build_cmd(new_param_file, bin_file, index_value, output_dir, True)
        else:
            print("[ERROR]the generation of input_json based on specified json is failed")

    def gen_input_json_based_on_specified_json(self: any, ori_json: str, output_dir: str):
        generated_files = split_json_files(ori_json, output_dir, expected_op_type=self.op_type)
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
                    self._write_build_cmd(new_param_file, bin_file, index_value, output_dir, True)
        else:
            print("[ERROR]the generation of input_json based on specified json is failed")

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
            if self.is_set_for_bin_query() and self.input_dtype_for_bin_list[idx][index]:
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
            if self.is_set_for_bin_query() and self.output_dtype_for_bin_list[idx][index]:
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
                raise RuntimeError(f"Op {self.op_type} must have at least one input or output")
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
                self._write_build_cmd(param_file, bin_file, index_value, auto_gen_path, True)

    def _write_build_json(self: any, param_file: str, param):
        with os.fdopen(os.open(param_file, const_var.WFLAGS, const_var.WMODES), "w") as fd:
            json.dump(param, fd, indent="  ")

    def _generate_check_result(self: any, enable_tiling_keys: bool, bin_file: str, kernel_spec_enabled=False):
        check_result = ""
        if kernel_spec_enabled:
            # Capture OPC status before echo or stale output checks can overwrite it.
            check_result += "opc_status=$?\n"
            check_result += 'echo "${res}"\n'
            check_result += 'if [ "${opc_status}" -ne 0 ]; then\n'
            if enable_tiling_keys:
                check_result += '    if [ "${opc_status}" -eq 1 ] && echo "${res}" | '
                check_result += 'grep -q "None of the given tiling keys are in the supported list"; then\n'
                check_result += f'        echo "{const_var.KERNEL_SPEC_RESULT_PREFIX}filtered"\n'
                check_result += "        exit 0\n"
                check_result += "    fi\n"
            check_result += '    exit "${opc_status}"\n'
            check_result += "fi\n"
            check_result += const_var.CHK_CMD.format(res_file=bin_file + ".json")
            check_result += const_var.CHK_CMD.format(res_file=bin_file + ".o")
            directory = getattr(self, "kernel_spec_directory", None)
            if directory is not None:
                skipped = shlex.quote(str(directory / bin_file / ".manifest-skipped"))
                check_result += f"if [ -f {skipped} ]; then\n"
                check_result += f'    echo "{const_var.KERNEL_SPEC_RESULT_PREFIX}filtered"\n    exit 0\nfi\n'
            check_result += f'echo "{const_var.KERNEL_SPEC_RESULT_PREFIX}completed"\n'
            return check_result
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

    def _get_compile_file(self: any, index: int):
        file_name = self.op_file if os.path.dirname(self.op_file) == "" else os.path.basename(self.op_file)
        name = "-".join([self.op_type, file_name, str(index)]) + ".sh"
        return os.path.realpath(os.path.join(self.out_path, name))

    def _append_optional_compile_options(self: any, command: str, super_mode: bool):
        if self.tiling_keys:
            tiling_key_str = ",".join(str(key) for key in sorted(self.tiling_keys))
            command += f' --tiling_key="{tiling_key_str}"'
            enable_tiling_keys = True
        else:
            enable_tiling_keys = False
        if self.op_debug_config:
            command += " --op_debug_config=" + ",".join(str(key) for key in self.op_debug_config)
        if super_mode and self.op_super_config:
            command += " " + " ".join(str(key) for key in self.op_super_config)
        if self.kernel_template_input:
            command += f" --kernel-template-input={self.kernel_template_input}"
        if super_mode and self.kernel_spec_options:
            if not self._kernel_spec_skip_logged:
                print(f"INFO: [{self.soc}] Skip Kernel Spec options for {self.op_type} relocatable binary.")
                self._kernel_spec_skip_logged = True
        else:
            command += self.kernel_spec_options
        return command, enable_tiling_keys

    def _build_opc_command(self: any, param_file: str, auto_gen_path: str, hard_soc: str, super_mode: bool):
        command = (
            'res=$(asc_opc "$1" --main_func={fun} --input_param={param} --soc_version={soc}                 '
            '--output="$2" --impl_mode={impl} --op_mode=dynamic '
        ).format(fun=self.op_intf, soc=hard_soc, param=param_file, impl="high_performance,optional")
        command += f" --simplified_key_mode={self.simplified_key_mode}"
        if hard_soc == "Ascend610Lite":
            command = f"export ASCEND_CUSTOM_OPP_PATH={auto_gen_path}:$ASCEND_CUSTOM_OPP_PATH \n" + command
        return self._append_optional_compile_options(command, super_mode)

    def _write_build_cmd(self: any, param_file: str, bin_file: str, index: int, auto_gen_path: str, super_mode=False):
        hard_soc = const_var.conv_soc_ver(self.soc)
        if not hard_soc:
            hard_soc = self.soc.capitalize()
        compile_file = self._get_compile_file(index)

        build_cmd_var = "#!/bin/bash\n"
        build_cmd_var += f'echo "[{self.soc}] Generating {bin_file} ..."\n'
        plog_level = os.environ.get("ASCEND_GLOBAL_LOG_LEVEL")
        plog_stdout = os.environ.get("ASCEND_SLOG_PRINT_TO_STDOUT")
        if plog_level is None:
            build_cmd_var += const_var.SET_PLOG_LEVEL_ERROR
        if plog_stdout is None:
            build_cmd_var += const_var.SET_PLOG_STDOUT
        build_cmd_var += const_var.SRC_ENV
        if self.kernel_spec_options and not super_mode:
            directory = getattr(self, "kernel_spec_directory", None)
            if directory is not None:
                build_cmd_var += "rm -rf -- " + shlex.quote(str(directory / bin_file)) + " || exit $?\n"
        command, enable_tiling_keys = self._build_opc_command(param_file, auto_gen_path, hard_soc, super_mode)
        build_cmd_var += command

        build_cmd_var += ")\n"
        build_cmd_var += "\n"

        check_result = self._generate_check_result(
            enable_tiling_keys, bin_file, kernel_spec_enabled=bool(self.kernel_spec_options) and not super_mode
        )
        build_cmd_var += check_result
        build_cmd_var += f'echo "[{self.soc}] Generating {bin_file} Done"\n'

        if not os.path.isfile(compile_file) or Path(compile_file).read_text() != build_cmd_var:
            with os.fdopen(os.open(compile_file, const_var.WFLAGS, const_var.WMODES), "w") as fd:
                fd.write(build_cmd_var)
