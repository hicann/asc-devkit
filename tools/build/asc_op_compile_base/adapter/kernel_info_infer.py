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
compile operator
"""

import os
import stat
import copy
import re
from tbe.common.buildcfg import get_current_build_config
from tbe.common.buildcfg.buildcfg_mapping import enable_vector_core
from tbe.tvm.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from .get_op_tiling import OpInfo
from .template_tiling import extract_template_tiling_info, decode_tiling
from .log_utils import AscendCLogLevel, CompileStage
from .global_storage import global_var_storage
from .ascendc_constants import (
    InferChannelParamsFromIFile,
    KernelMetaType,
    STR_TO_KERNEL_TYPE_V220,
    STR_TO_KERNEL_TYPE_V200,
    CompileOptionTuple,
    CORE_TYPE_MIX,
    TILING_KEY_MACRO,
)
from .ascendc_common_utility import CommonUtility
from .ascendc_compile_v220 import gen_compile_cmd_v220
from .super_kernel_utility import check_exist_instrinsic_when_super_kernel
from .ascendc_compile_gen_code import _gen_compile_cmd
from .ascendc_compile_utils import (
    tpl_tilingkey_kernel_type_check,
    tpl_tilingkey_deterministic_extract,
    tpl_tilingkey_native_extract,
)

DEFAULT_TILING_KEY = "0"


class KernelInfoInfer:
    """
    This class is used for get tiling key list and some kernel info
    i.g. code channel, kernel type for v200 and v220
    """

    @staticmethod
    def get_hard_sync_instr_from_i_file(content):
        """
        find whether used SyncAll instr in kernel func
        if so, return true
        otherwise, return false
        """
        pattern = re.compile(r"SyncAll\s*<\w+>\s*\(\s*\)\s*;")
        result = pattern.findall(content)
        if len(result) != 0:
            return True
        pattern = r"SyncAll\s*\(\s*\)\s*\;"
        match = re.findall(pattern, content)
        if len(match) > 2:
            return True
        return False

    @staticmethod
    def get_sync_task_start_end_instr_from_i_file(content):
        """
        find whether used SetNextTaskStart WaitPreTaskEnd instr in kernel func
        if so, return true
        otherwise, return false
        """
        set_pattern = r"SetNextTaskStart\s*(?:<[^;{]*>)?\s*\(\s*\)\s*\;"
        wait_pattern = r"WaitPreTaskEnd\s*(?:<[^;{]*>)?\s*\(\s*\)\s*\;"
        set_match = re.findall(set_pattern, content)
        wait_match = re.findall(wait_pattern, content)
        return len(set_match) > 2, len(wait_match) > 2

    @staticmethod
    def get_enable_deterministic_var_from_i_file(content):
        """
        find whether enable_deterministic in kernel func
        if so, return true
        otherwise, return false
        """
        pattern = r"__enable_feature_for_compile_deterministic\s*=\s*(-?\d+)\s*;"
        match = re.search(pattern, content)
        if match and match.group(1) == "1":
            return True
        return False

    @staticmethod
    def find_kernel_type(s):
        match = re.search(
            r"__enable_feature_for_compile_default\s*=\s*([0-9a-zA-Z_]{1,})\s*;", s
        )
        if match:
            return None, match.group(1)
        else:
            match = re.search(
                r"__enable_feature_for_compile_(-?\d+)([a-zA-Z]*)\s*=\s*([0-9a-zA-Z_]{1,})\s*;",
                s,
            )
            if match:
                return match.group(1), match.group(3)
            return None, None

    @staticmethod
    def find_tilingkey(s):
        if re.search(r"g_tilingKey == \(", s):
            matches = re.findall(r"g_tilingKey == \((-?\d+)", s)
            if matches:
                if "TILING_KEY_LIST" in s:
                    return matches, True
                else:
                    return matches, False
            else:
                matches = re.findall(r"g_tilingKey == \((.*?)\)", s)
                if matches:
                    CommonUtility.print_compile_log(
                        "",
                        f"Var: {matches[0]} in TILING_KEY_IS({matches[0]}) can not be \
processed as numeric variables in the precompilation phase. please use numeric constants or macros.",
                        AscendCLogLevel.LOG_ERROR,
                    )
                return None, False
        return None, False

    @staticmethod
    def find_tiling_struct_and_expression(s):
        # find tiling keywords used in REGISTER_TILING_DEFAULT and REGISTER_TILING_FOR_TILINGKEY from input string
        # If find __enable_custom_tiling, return its tiling struct and expression
        if "__enable_custom_tiling" not in s:
            return None, None
        match = re.search(
            r"__enable_custom_tiling\s*([0-9a-zA-Z_:<>]{1,})\s*=\s*default;", s
        )
        if match:
            return match.group(1), None
        else:
            match = re.search(
                r"__enable_custom_tiling\s*([0-9a-zA-Z_:<>]{1,})\s*=\s*\"(.*)\";", s
            )
            if match:
                return match.group(1), match.group(2)
            else:
                raise_tbe_python_err(
                    TBE_DEFAULT_PYTHON_ERROR_CODE,
                    ("tiling struct match expression is wrong. lines: " + s),
                )
                return None, None

    @staticmethod
    def find_tiling_struct_no_register_flag(s: str) -> bool:
        return "__enable_no_register_custom_tiling" in s

    @staticmethod
    def get_kernel_type_enum(kernel_type, compile_log_path):
        if CommonUtility.is_v220() or CommonUtility.is_c310():
            if kernel_type in STR_TO_KERNEL_TYPE_V220.keys():
                return STR_TO_KERNEL_TYPE_V220[kernel_type]
            else:
                if kernel_type not in STR_TO_KERNEL_TYPE_V200.keys():
                    CommonUtility.print_compile_log(
                        "",
                        "current kernel type: {} is not support in current core version".format(
                            kernel_type
                        ),
                        AscendCLogLevel.LOG_WARNING,
                    )
                return None
        elif CommonUtility.is_v200():
            if kernel_type in STR_TO_KERNEL_TYPE_V200.keys():
                return STR_TO_KERNEL_TYPE_V200[kernel_type]
            else:
                if kernel_type not in STR_TO_KERNEL_TYPE_V220.keys():
                    CommonUtility.print_compile_log(
                        "",
                        "current kernel type: {} is not support in current core version".format(
                            kernel_type
                        ),
                        AscendCLogLevel.LOG_WARNING,
                    )
                return None
        else:
            raise Exception(
                "current kernel type: {} is not support in current core version".format(
                    kernel_type
                )
            )
        return None

    @staticmethod
    def get_kernel_type_enum_for_group(kernel_type):
        if CommonUtility.is_v220() or CommonUtility.is_c310():
            if kernel_type in STR_TO_KERNEL_TYPE_V220.keys():
                return STR_TO_KERNEL_TYPE_V220[kernel_type]
            elif kernel_type in STR_TO_KERNEL_TYPE_V200.keys():
                return STR_TO_KERNEL_TYPE_V200[kernel_type]
            else:
                raise Exception(
                    "current kernel type: {} is not support in current core version".format(
                        kernel_type
                    )
                )
        return None

    @staticmethod
    def is_valid_kernel_type_in_group(kernel_type, tiling_key=None):
        if kernel_type is None or kernel_type == "":
            return False
        tiling_key_info = "" if tiling_key is None else f"of tiling key {tiling_key} "
        kernel_type_info = str(kernel_type).split(".")[-1]
        if kernel_type not in STR_TO_KERNEL_TYPE_V220.values():
            raise Exception(
                f"kernel type: {kernel_type_info} {tiling_key_info}is not support in current \
core version"
            )
        elif kernel_type not in [
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
        ]:
            raise Exception(
                f"kernel type: {kernel_type_info} {tiling_key_info}is not support in \
TILING_KEY_LIST, please set to KERNEL_TYPE_MIX_AIC_1_1 or KERNEL_TYPE_MIX_AIC_1_2"
            )
        else:
            return True

    @staticmethod
    def gen_tiling_struct_macro_src_file(
        tiling_key_list, tiling_struct_expr_map, src_file
    ):
        file_contents = ""
        for key, value in tiling_struct_expr_map.items():
            for expression in value:
                for tiling_key in tiling_key_list:
                    new_expression = expression.replace(
                        TILING_KEY_MACRO, TILING_KEY_MACRO + "_" + tiling_key
                    )
                    file_contents += f"#if defined({TILING_KEY_MACRO}_{tiling_key}) && {new_expression}\n"
                    file_contents += f"    auto __ascendc_custom_tiling_struct = ({tiling_key}, {key});\n"
                    file_contents += "#endif\n"
        try:
            with open(src_file, "w") as f:
                f.writelines(file_contents)
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                ("write tiling struct tmp file failed, reason is :", err),
            )

    @staticmethod
    def get_tiling_key_corresponding_struct(
        tiling_key_list,
        default_tiling_struct,
        src_tiling_file,
        dst_tiling_file,
        compile_log_path,
    ):
        tiling_key_struct_map = {}
        tiling_compile_cmd = [
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
            "-std=c++17",
            "-E",
            src_tiling_file,
            "-o",
            dst_tiling_file,
        ]
        for tiling_key in tiling_key_list:
            tiling_compile_cmd.append(
                f"-D{TILING_KEY_MACRO}_{tiling_key}={tiling_key}UL"
            )
        CommonUtility.run_cmd_inner(
            tiling_compile_cmd, CompileStage.PRECOMPILE, compile_log_path
        )
        match_tiling_struct = ""
        try:
            with open(dst_tiling_file, "r") as f:
                lines = f.readlines()
                for line in lines:
                    if line.startswith("#"):
                        continue
                    match = re.search(
                        r"auto __ascendc_custom_tiling_struct\s*=\s*\((-?\d+),\s([0-9a-zA-Z_:]{1,})\);",
                        line,
                    )
                    if match:
                        if match.group(
                            1
                        ) in tiling_key_struct_map and tiling_key_struct_map[
                            match.group(1)
                        ] != match.group(2):
                            raise_tbe_python_err(
                                TBE_DEFAULT_PYTHON_ERROR_CODE,
                                (
                                    f"tiling key {match.group(1)} should have 1 unique \
                                                  corresponding tiling struct, but \
found following structs::{tiling_key_struct_map[match.group(1)]}, {match.group(2)}"
                                ),
                            )
                        else:
                            tiling_key_struct_map[match.group(1)] = match.group(2)
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                ("read tiling struct dump file failed, reason is :", err),
            )
        for tiling_key in tiling_key_list:
            if tiling_key not in tiling_key_struct_map:
                tiling_key_struct_map[tiling_key] = default_tiling_struct
        return tiling_key_struct_map

    @staticmethod
    def check_func_name_exist(pattern: str, text: str):
        match = re.search(r"\b{}\b\s*\(".format(pattern), text)
        if match:
            return True
        else:
            return False

    @staticmethod
    def dfx_for_func_name(cce_file: str, origin_func_name: str, func_name_exist: bool):
        if not func_name_exist:
            cce_file = cce_file.split("/")[-1]
            CommonUtility.print_compile_log(
                "",
                f"kernel entry `{origin_func_name}' not implement in `{cce_file}', \
please check whether the function name is correct in the kernel file.",
                AscendCLogLevel.LOG_ERROR,
            )
            raise Exception("An error occurred during stage of infer compile info")

    @staticmethod
    def search_any_in_line(line, keywords):
        pattern = re.compile(
            r"\b(" + "|".join(re.escape(keyword) for keyword in keywords) + r")\b"
        )
        matches = pattern.findall(line)
        return matches

    @staticmethod
    def _gen_tiling_key_struct_map(
        default_tiling_struct,
        declare_param_str,
        select_param_str,
        decode_tiling_result,
        dst_i_file,
        tiling_key_list,
        tiling_struct_expr_map,
        compile_log_path,
        tiling_key_group_map,
    ):
        tiling_key_struct_map = {}
        if default_tiling_struct != "":
            if declare_param_str and select_param_str:
                for tiling_key, information in decode_tiling_result.items():
                    tiling_key_struct_map[str(tiling_key)] = information.get(
                        "tilingStruct", default_tiling_struct
                    )
            else:
                src_tiling_file = dst_i_file[:-2] + "_tiling_key_tiling_struct.cpp"
                dis_tiling_i_file = (
                    dst_i_file[:-2] + "_tiling_key_tiling_struct" + dst_i_file[-2:]
                )
                entire_tiling_key_list = tiling_key_list.copy()
                if tiling_key_group_map is not None:
                    for tiling_key_slaves in tiling_key_group_map.values():
                        entire_tiling_key_list.extend(tiling_key_slaves)
                KernelInfoInfer.gen_tiling_struct_macro_src_file(
                    entire_tiling_key_list, tiling_struct_expr_map, src_tiling_file
                )
                tiling_key_struct_map = (
                    KernelInfoInfer.get_tiling_key_corresponding_struct(
                        entire_tiling_key_list,
                        default_tiling_struct,
                        src_tiling_file,
                        dis_tiling_i_file,
                        compile_log_path,
                    )
                )
        else:
            if len(tiling_struct_expr_map) != 0:
                raise Exception(
                    "if use user-defined tiling structure, must provide default tiling struct, use macro \
REGISTER_TILING_DEFAULT"
                )
            for _, information in decode_tiling_result.items():
                if "tilingStruct" in information:
                    raise Exception(
                        "if use user-defined tiling structure, must provide default tiling struct,\
 use macro REGISTER_TILING_DEFAULT"
                    )
        return tiling_key_struct_map

    @staticmethod
    def infer_info_from_ifile(
        op_info: OpInfo,
        dst_i_file: str,
        compile_log_path,
        cce_file: str,
        origin_func_name: str,
    ):
        tiling_key_list = []
        tiling_key_group_map = {}
        declare_param_str = ""
        select_param_str = ""
        decode_tiling_result = {}
        code_channel: int = -1
        no_kfc_server_flag = False
        find_kfc_server = False
        default_tiling_struct = ""
        tiling_struct_expr_map = {}
        register_tiling_struct = set()
        tpl_tiling_struct = set()
        if not (CommonUtility.is_v220() or CommonUtility.is_c310()):
            code_channel = CORE_TYPE_MIX
        if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
            check_exist_instrinsic_when_super_kernel(dst_i_file)
        try:
            with open(dst_i_file, "r") as fd:
                content = fd.read()
                fd.close()
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                ("read dst_i_file failed, reason is:", err),
            )
        hard_sync = KernelInfoInfer.get_hard_sync_instr_from_i_file(content)
        global_var_storage.set_variable("ascendc_op_with_syncall", hard_sync)
        if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
            set_task_bar, wait_task_bar = (
                KernelInfoInfer.get_sync_task_start_end_instr_from_i_file(content)
            )
        else:
            set_task_bar = False
            wait_task_bar = False
        enable_deterministic = KernelInfoInfer.get_enable_deterministic_var_from_i_file(
            content
        )
        tiling_key_kernel_type = {}
        tiling_key_kernel_type_full = {}
        tiling_key_deterministic = {}
        default_kernel_type = KernelMetaType.KERNEL_TYPE_MAX
        default_kernel_type_for_group = KernelMetaType.KERNEL_TYPE_MAX
        dump_info = {"dump_type": ""}
        func_name_exist = False

        need_find_kernel_type = (
            not CommonUtility.is_m510()
            and not CommonUtility.is_l300()
            and not CommonUtility.is_l311()
            and not CommonUtility.is_l510()
            and not CommonUtility.is_l516()
        )
        tiling_no_register_flag = False
        try:
            with open(dst_i_file, "r") as fd:
                lines = fd.readlines()
                fd.close()
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                ("read dst_i_file failed, reason is:", err),
            )
        keywords = [
            "bisheng_compiler",
            "ccec_compiler",
            "tikcpp/tikcfw",
            "gnu/bits",
            "include/c++",
            "include/kernel_tiling/kernel_tiling.h",
        ]
        is_op_block: bool = True
        for line in lines:
            if line.startswith("#"):
                found_built_in = KernelInfoInfer.search_any_in_line(line, keywords)
                is_op_block = not found_built_in
                continue
            if not is_op_block:
                continue
            func_name_exist = func_name_exist or KernelInfoInfer.check_func_name_exist(
                origin_func_name, line
            )
            if declare_param_str == "" and "@@ASCENDC_TPL_ARGS_DECL" in line:
                declare_param_str = line
            if select_param_str == "" and "@@ASCENDC_TPL_LISTS" in line:
                select_param_str = line
            if (not find_kfc_server) and "AscendC::KfcServer" in line:
                code_channel = CORE_TYPE_MIX
                find_kfc_server = True
            # process register tiling strcut and expression
            tiling_struct, tiling_expression = (
                KernelInfoInfer.find_tiling_struct_and_expression(line)
            )
            if tiling_struct is not None:
                if tiling_expression is None:
                    if (
                        default_tiling_struct != ""
                        and default_tiling_struct != tiling_struct
                    ):
                        raise_tbe_python_err(
                            TBE_DEFAULT_PYTHON_ERROR_CODE,
                            ("Only one default tiling structure can be configured."),
                        )
                    else:
                        default_tiling_struct = tiling_struct
                else:
                    if tiling_struct in tiling_struct_expr_map:
                        tiling_struct_expr_map[tiling_struct].add(
                            "(" + tiling_expression + ")"
                        )
                    else:
                        tiling_struct_expr_map[tiling_struct] = set(
                            [str("(" + tiling_expression + ")")]
                        )
            tiling_key, kernel_type = KernelInfoInfer.find_kernel_type(line)
            if need_find_kernel_type:
                if tiling_key is None and kernel_type is not None:
                    cur_kernel_type = KernelInfoInfer.get_kernel_type_enum(
                        kernel_type, compile_log_path
                    )
                    if cur_kernel_type is not None:
                        default_kernel_type = cur_kernel_type
                    default_kernel_type_for_group = (
                        KernelInfoInfer.get_kernel_type_enum_for_group(kernel_type)
                    )
                if tiling_key is not None and kernel_type is not None:
                    cur_kernel_type = KernelInfoInfer.get_kernel_type_enum(
                        kernel_type, compile_log_path
                    )
                    if cur_kernel_type is not None:
                        tiling_key_kernel_type[str(int(tiling_key))] = cur_kernel_type
                    tiling_key_kernel_type_full[str(int(tiling_key))] = (
                        KernelInfoInfer.get_kernel_type_enum_for_group(kernel_type)
                    )
            tiling_no_register_flag |= (
                KernelInfoInfer.find_tiling_struct_no_register_flag(line)
            )
            numbers, is_tiling_key_list = KernelInfoInfer.find_tilingkey(line)
            if numbers is None:
                continue
            if not is_tiling_key_list:
                for number in numbers:
                    if str(int(number)) not in tiling_key_list:
                        tiling_key_list.append(str(int(number)))
            else:
                if str(int(numbers[0])) not in tiling_key_list:
                    if len(numbers) != 2:
                        raise Exception(
                            f"number of tiling key is len{numbers}, just support two \
                            tilingkeys"
                        )
                    for number_slave in numbers[1:]:
                        if str(int(number_slave)) in tiling_key_list:
                            raise Exception(
                                f"tiling_key {number_slave} is exists in tiling_key_list."
                            )
                    tiling_key_list.append(str(int(numbers[0])))
                    tiling_key_group_map[str(int(numbers[0]))] = numbers[1:]

        if tiling_key_group_map is not None and len(tiling_key_group_map) > 0:
            KernelInfoInfer.get_tiling_key_kernel_type_full(
                tiling_key_group_map,
                tiling_key_kernel_type_full,
                default_kernel_type_for_group,
            )
        KernelInfoInfer.get_tiling_key_kernel_type_in_group(
            tiling_key_kernel_type, tiling_key_kernel_type_full
        )

        for tiling_struct in tiling_struct_expr_map.keys():
            register_tiling_struct.add(tiling_struct)

        if declare_param_str and select_param_str:
            # TPL
            extract_template_tiling_info(declare_param_str, select_param_str)
            decode_tiling_result = decode_tiling()
            tiling_key_list = [str(k) for k in decode_tiling_result.keys()]
            if len(tiling_key_list) == 0:
                raise Exception("no match kernel template input!")
            # ==================== All SEL checks
            tiling_key_list, decode_tiling_result = tpl_tilingkey_kernel_type_check(
                tiling_key_list, decode_tiling_result, tiling_key_kernel_type
            )

            # ==================== Filter SEL : ALL to Group
            tiling_key_list, decode_tiling_result = tpl_tilingkey_native_extract(
                tiling_key_list, decode_tiling_result, op_info
            )
            # ==================== Group SEL checks
            tiling_key_list, decode_tiling_result = tpl_tilingkey_deterministic_extract(
                tiling_key_list,
                decode_tiling_result,
                tiling_key_deterministic,
            )
            # ==================== check done
            group_to_key = {}
            group_id_to_all_keys = {}
            for key, value in decode_tiling_result.items():
                group_id = value.get("groupId", "")
                if group_id != "":
                    if group_id not in group_to_key:
                        group_to_key[group_id] = key
                    if group_id not in group_id_to_all_keys:
                        group_id_to_all_keys[group_id] = []
                    group_id_to_all_keys[group_id].append(key)

            tiling_key_list_tmp = list(group_to_key.values())
            tiling_key_list_tmp = [str(key) for key in tiling_key_list_tmp]

            key_to_same_group_keys = {}
            for key in tiling_key_list_tmp:
                if int(key) in decode_tiling_result.keys():
                    group_id = decode_tiling_result[int(key)].get("groupId")
                    same_group = group_id_to_all_keys.get(group_id, [])
                    others = [str(k) for k in same_group if str(k) != str(key)]
                    if len(others) > 0:
                        key_to_same_group_keys[key] = others
            tiling_key_group_map = key_to_same_group_keys
            if len(tiling_key_list_tmp) > 0:
                tiling_key_list = tiling_key_list_tmp

        KernelInfoInfer.dfx_for_func_name(cce_file, origin_func_name, func_name_exist)

        if tiling_no_register_flag:
            CommonUtility.dump_log("Found Using REGISTER_NONE_TILING", compile_log_path)
            global_var_storage.set_variable("ascendc_tiling_no_register", True)

        if len(tiling_key_list) == 0:
            tiling_key_list = [DEFAULT_TILING_KEY]
        if not find_kfc_server:
            no_kfc_server_flag = True

        no_set_kernel_type = False
        if (
            default_kernel_type == KernelMetaType.KERNEL_TYPE_MAX
            and not tiling_key_kernel_type
        ):
            # TPL ORIGIN
            if get_current_build_config(enable_vector_core):
                CommonUtility.dump_log(
                    "Information Library Configuration Takes Effect", compile_log_path
                )
                for tiling_key in tiling_key_list:
                    tiling_key_kernel_type[tiling_key] = (
                        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE
                    )
            else:
                no_set_kernel_type = True
        else:
            tilingkey_without_kernel_type_set = set(tiling_key_list) - set(
                tiling_key_kernel_type.keys()
            )
            if (
                len(tilingkey_without_kernel_type_set) > 0
                and default_kernel_type == KernelMetaType.KERNEL_TYPE_MAX
            ):
                raise Exception("must provide default kernel type")
            for tiling_key in tilingkey_without_kernel_type_set:
                tiling_key_kernel_type[tiling_key] = default_kernel_type
            if get_current_build_config(enable_vector_core):
                CommonUtility.dump_log(
                    "Information Library Configuration Does Not Take Effect After the Macro Is Enabled",
                    compile_log_path,
                    "[WARNING] : ",
                )
        if not global_var_storage.get_variable("ascendc_compile_debug_config"):
            CommonUtility.remove_temp_file(dst_i_file)

        tiling_key_struct_map = KernelInfoInfer._gen_tiling_key_struct_map(
            default_tiling_struct,
            declare_param_str,
            select_param_str,
            decode_tiling_result,
            dst_i_file,
            tiling_key_list,
            tiling_struct_expr_map,
            compile_log_path,
            tiling_key_group_map,
        )

        for tiling_struct in tiling_key_struct_map.values():
            tpl_tiling_struct.add(tiling_struct)

        return InferChannelParamsFromIFile(
            tiling_key_list,
            code_channel,
            hard_sync,
            no_kfc_server_flag,
            enable_deterministic,
            tiling_key_kernel_type,
            no_set_kernel_type,
            default_kernel_type,
            dump_info,
            decode_tiling_result,
            default_tiling_struct,
            tiling_struct_expr_map,
            tiling_key_struct_map,
            register_tiling_struct,
            tpl_tiling_struct,
            set_task_bar,
            wait_task_bar,
            tiling_key_deterministic,
            tiling_key_group_map,
        )

    @staticmethod
    def get_tiling_key_kernel_type_in_group(
        tiling_key_kernel_type, tiling_key_kernel_type_origin
    ):
        if tiling_key_kernel_type_origin is not None:
            for tiling_key in tiling_key_kernel_type_origin.keys():
                if tiling_key not in tiling_key_kernel_type.keys():
                    tiling_key_kernel_type[str(int(tiling_key))] = (
                        tiling_key_kernel_type_origin[tiling_key]
                    )

    @staticmethod
    def get_tiling_key_kernel_type_full(
        tiling_key_group_map, tiling_key_kernel_type_full, default_kernel_type_for_group
    ):
        for master_key, slave_keys in tiling_key_group_map.items():
            if master_key in tiling_key_kernel_type_full.keys():
                master_kernel_type = tiling_key_kernel_type_full[master_key]
                KernelInfoInfer.is_valid_kernel_type_in_group(
                    master_kernel_type, master_key
                )
            else:
                KernelInfoInfer.set_default_kernel_type_for_group(
                    tiling_key_kernel_type_full,
                    master_key,
                    default_kernel_type_for_group,
                )

            for slave_key in slave_keys:
                if slave_key in tiling_key_kernel_type_full.keys():
                    slave_kernel_type = tiling_key_kernel_type_full[slave_key]
                    KernelInfoInfer.is_valid_kernel_type_in_group(
                        slave_kernel_type, slave_key
                    )
                else:
                    KernelInfoInfer.set_default_kernel_type_for_group(
                        tiling_key_kernel_type_full,
                        slave_key,
                        default_kernel_type_for_group,
                    )

    @staticmethod
    def set_default_kernel_type_for_group(
        tiling_key_kernel_type_full, tiling_key, default_kernel_type_for_group
    ):
        if KernelInfoInfer.is_valid_kernel_type_in_group(default_kernel_type_for_group):
            tiling_key_kernel_type_full[tiling_key] = default_kernel_type_for_group
        else:
            raise Exception(
                f"must set kernel type for tiling_key {tiling_key} in group"
            )

    @staticmethod
    def get_tiling_key_list_and_simple_infer_code_channel(
        op_info: OpInfo,
        cce_file: str,
        dst_i_file: str,
        compile_option_tuple: CompileOptionTuple,
        compile_log_path,
        origin_func_name,
    ):
        """
        get tiling key list and simple infer code channel
        :param cce_file:
        :return:InferedInfo
        """
        compile_option_tuple_pre = copy.deepcopy(compile_option_tuple)
        compile_option_tuple_pre.compile_options = (
            compile_option_tuple_pre.compile_options
            + ["-E"]
            + ["-D__CHECK_FEATURE_AT_PRECOMPILE"]
            + ["-includestdio.h"]
        )
        compile_option_tuple_pre.compile_options = (
            compile_option_tuple_pre.compile_options + ["-DASCENDC_TPL_PRE"]
        )
        chip_version = CommonUtility.get_chip_version()
        # generate .i file
        if CommonUtility.is_v220() or CommonUtility.is_c310():
            arch = f"dav-{chip_version}-cube"
            dis_i_file_cube = dst_i_file[:-2] + "_cube" + dst_i_file[-2:]
            pre_compile_cmd = gen_compile_cmd_v220(
                cce_file, dis_i_file_cube, compile_option_tuple_pre, arch, "", False
            )
            CommonUtility.run_cmd_inner(
                pre_compile_cmd, CompileStage.PRECOMPILE, compile_log_path
            )
            arch = f"dav-{chip_version}-vec"
            dis_i_file_vec = dst_i_file[:-2] + "_vec" + dst_i_file[-2:]
            pre_compile_cmd = gen_compile_cmd_v220(
                cce_file, dis_i_file_vec, compile_option_tuple_pre, arch, "", False
            )
            CommonUtility.run_cmd_inner(
                pre_compile_cmd, CompileStage.PRECOMPILE, compile_log_path
            )
            with open(dis_i_file_cube, "r") as f_cube, open(
                dis_i_file_vec, "r"
            ) as f_vec:
                cube_content = f_cube.read()
                vec_content = f_vec.read()
            # merge sub core .i file in dst_i_file
            with open(dst_i_file, "w") as f:
                f.write(cube_content + vec_content)
            # chmod sub core .i file permission
            os.chmod(dis_i_file_cube, stat.S_IRUSR + stat.S_IWUSR)
            os.chmod(dis_i_file_vec, stat.S_IRUSR + stat.S_IWUSR)
        elif CommonUtility.is_m510():
            pre_compile_cmd = gen_compile_cmd_v220(
                cce_file, dst_i_file, compile_option_tuple_pre, None, "", False
            )
            CommonUtility.run_cmd_inner(
                pre_compile_cmd, CompileStage.PRECOMPILE, compile_log_path
            )
        else:
            pre_compile_cmd = _gen_compile_cmd(
                cce_file, dst_i_file, compile_option_tuple_pre, "", False
            )
            CommonUtility.run_cmd_inner(
                pre_compile_cmd, CompileStage.PRECOMPILE, compile_log_path
            )
        if not os.path.exists(dst_i_file):
            raise Exception(
                f"Geneate file {dst_i_file} failed, probably due to error in compile"
            )
        os.chmod(dst_i_file, stat.S_IRUSR + stat.S_IWUSR)
        # get tiling key list and simpel infer code channel
        return KernelInfoInfer.infer_info_from_ifile(
            op_info, dst_i_file, compile_log_path, cce_file, origin_func_name
        )
