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
ascendc gen tiling struct isolate
"""

import os
import sys
import json
import argparse
from pathlib import Path
from asc_op_compile_base.common.utils.log_utils import LogUtil, AscendCLogLevel
from asc_op_compile_base.asc_op_compiler.get_op_tiling import (
    load_custom_lib,
    load_build_in_lib,
    TilingInfo,
    generate_dynamic_tiling_struct_file,
    generate_static_tiling_struct_file,
)
from asc_op_compile_base.common.platform.platform_info import set_current_compile_soc_info


def load_json(op_type, json_file):
    try:
        if not os.path.exists(json_file):
            LogUtil.print_compile_log(
                op_type, f"[Sub process] isolate tiling json file {json_file} not existed.", AscendCLogLevel.LOG_ERROR
            )
            return None

        with open(json_file, "r", encoding="utf-8") as f:
            return json.load(f)

    except Exception as e:
        LogUtil.print_compile_log(
            op_type,
            f"[Sub process] load isolate tiling json file {json_file} failed, error info: {e}",
            AscendCLogLevel.LOG_ERROR,
        )
        return None


class IsolateTilingInfo:
    def __init__(self, tiling_info_dict, json_file):
        self.json_path = json_file
        self.tiling_info_dict = tiling_info_dict
        self.op_type = tiling_info_dict["optype"]
        self.is_build_in_op = tiling_info_dict["is_build_in_op"]
        self.custom_op_tiling_path = tiling_info_dict["custom_op_tiling_path"]
        self.kernel_meta_path = tiling_info_dict["kernel_meta_path"]
        self.is_static_shape = tiling_info_dict["is_static_shape"]
        self.run_info = {}
        if self.is_static_shape:
            self.run_info = tiling_info_dict["run_info"]
            self.run_info["tiling_data"] = bytes.fromhex(self.run_info["tiling_data"])
        self.tiling_key_list = tiling_info_dict["tiling_key_list"]
        self.tiling_key_group_map = tiling_info_dict["tiling_key_group_map"]
        self.tiling_const_propagation = tiling_info_dict.get("tiling_const_propagation", False)
        self.tiling_info = TilingInfo()

    def dump_json(self):
        self.tiling_info_dict["tiling_info"] = self.tiling_info.dump_to_dict()
        try:
            with open(self.json_path, "w", encoding="utf-8") as f:
                json.dump(self.tiling_info_dict, f, indent=2, ensure_ascii=False)
                return True
        except Exception as e:
            LogUtil.print_compile_log(
                self.op_type,
                f"[Sub process]load isolate tiling json file {self.json_path} failed, error info: {e}",
                AscendCLogLevel.LOG_ERROR,
            )
            return False


def load_op_tiling_lib(isolate_tiling_info):
    try:
        if isolate_tiling_info.is_build_in_op:
            LogUtil.print_compile_log(
                isolate_tiling_info.op_type, "[Sub process]load build-in op tiling lib", AscendCLogLevel.LOG_INFO
            )
            return load_build_in_lib()
        else:
            LogUtil.print_compile_log(
                isolate_tiling_info.op_type,
                f"[Sub process]load custom op tiling lib: {isolate_tiling_info.custom_op_tiling_path}",
                AscendCLogLevel.LOG_INFO,
            )
            return load_custom_lib(isolate_tiling_info.custom_op_tiling_path)
    except Exception as e:
        LogUtil.print_compile_log(
            isolate_tiling_info.op_type,
            f"[Sub process]tiling so load failed, error info : {e}",
            AscendCLogLevel.LOG_ERROR,
        )
        return False


def generate_tiling_info(isolate_tiling_info):
    tiling_info = isolate_tiling_info.tiling_info
    tiling_info.tiling_key_list = isolate_tiling_info.tiling_key_list
    if isolate_tiling_info.is_static_shape:
        generate_static_tiling_struct_file(
            isolate_tiling_info.op_type,
            isolate_tiling_info.run_info,
            tiling_info,
            isolate_tiling_info.tiling_key_list,
            isolate_tiling_info.tiling_key_group_map,
            isolate_tiling_info.tiling_const_propagation,
        )
    else:
        tiling_info.static_shape_flag = False
        generate_dynamic_tiling_struct_file(
            isolate_tiling_info.op_type,
            tiling_info,
            isolate_tiling_info.tiling_key_list,
            isolate_tiling_info.tiling_key_group_map,
        )
    isolate_tiling_info.tiling_info = tiling_info


def parse_input_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("op_type", type=str, help="optype")
    parser.add_argument("isolate_json", type=str, help="json file for storing info")
    parser.add_argument("soc_version", type=str, help="soc version")
    dump_parse_args = parser.parse_args()
    return dump_parse_args


if __name__ == "__main__":
    args = parse_input_args()
    set_current_compile_soc_info(args.soc_version)
    LogUtil.print_compile_log(args.op_type, "[Sub process] begin load isolate tiling info.", AscendCLogLevel.LOG_INFO)
    tiling_info_dict = load_json(args.op_type, args.isolate_json)
    if tiling_info_dict is None:
        LogUtil.print_compile_log(
            args.op_type, "[Sub process] load isolate tiling info failed.", AscendCLogLevel.LOG_WARNING
        )
        sys.exit(1)

    isolate_tiling_info = IsolateTilingInfo(tiling_info_dict, args.isolate_json)
    LogUtil.print_compile_log(args.op_type, "[Sub process] begin load tiling so.", AscendCLogLevel.LOG_INFO)
    res = load_op_tiling_lib(isolate_tiling_info)
    if not res:
        LogUtil.print_compile_log(
            args.op_type, "[Sub process] load isolate tiling so failed.", AscendCLogLevel.LOG_WARNING
        )
        sys.exit(1)

    LogUtil.print_compile_log(args.op_type, "[Sub process] begin generate tiling info.", AscendCLogLevel.LOG_INFO)
    generate_tiling_info(isolate_tiling_info)
    if isolate_tiling_info.tiling_info.tiling_info_completed is False:
        LogUtil.print_compile_log(args.op_type, "[Sub process] generate tiling so failed.", AscendCLogLevel.LOG_WARNING)
        sys.exit(1)
    LogUtil.print_compile_log(args.op_type, "[Sub process] begin dump isolate tiling info.", AscendCLogLevel.LOG_INFO)
    res = isolate_tiling_info.dump_json()
    if not res:
        LogUtil.print_compile_log(
            args.op_type, "[Sub process] dump isolate tiling info failed.", AscendCLogLevel.LOG_WARNING
        )
        sys.exit(1)
    sys.exit(0)
