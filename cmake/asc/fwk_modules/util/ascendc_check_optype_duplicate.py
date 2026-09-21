#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import configparser
import argparse
import os
import glob
import json
import sys
from asc_op_compile_base.asc_op_compiler.op_tiling import _ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT, op_impl_path
from asc_op_compile_base.common.utils.log_utils import LogUtil, AscendCLogLevel


def check_optype_duplicate(args, ini_optypes):
    opp_path = os.environ.get(_ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT)
    json_path_base = os.path.join(opp_path, op_impl_path, os.path.join("ai_core", "tbe", "config", args.soc_version))
    LogUtil.print_compile_log(
        "check_optype_duplicate",
        f"json path base is {json_path_base}",
        AscendCLogLevel.LOG_DEBUG,
        LogUtil.Option.NON_SOC,
    )
    json_pattern = json_path_base + "/*.json"
    json_files = glob.glob(json_pattern, recursive=True)

    if len(json_files) == 0:
        LogUtil.print_compile_log(
            "check_optype_duplicate", "no json files found", AscendCLogLevel.LOG_DEBUG, LogUtil.Option.NON_SOC
        )
        return 0

    optypes = []
    for json_file in json_files:
        try:
            with open(json_file, "r") as fd:
                ops_info = json.load(fd)
                optypes += ops_info.keys()
        except Exception as err:
            sys.stderr.write(f"json file {json_file} open failed, err msg {err}")
            return 1

    intersection_ops = list(set(ini_optypes) & set(optypes))
    LogUtil.print_compile_log(
        "check_optype_duplicate", f"custom optypes {ini_optypes}", AscendCLogLevel.LOG_DEBUG, LogUtil.Option.NON_SOC
    )
    LogUtil.print_compile_log(
        "check_optype_duplicate",
        f"duplicate optypes {intersection_ops}",
        AscendCLogLevel.LOG_DEBUG,
        LogUtil.Option.NON_SOC,
    )
    if len(intersection_ops) != 0:
        sys.stderr.write(" ".join(intersection_ops))
        return 2

    return 0


def get_optypes(args):
    op_config = configparser.ConfigParser()
    LogUtil.print_compile_log(
        "check_optype_duplicate", f"ini file: {args.ini_file}", AscendCLogLevel.LOG_DEBUG, LogUtil.Option.NON_SOC
    )

    op_config.read(args.ini_file)
    return op_config.sections()


def args_parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--ini-file", help="op info ini.")
    parser.add_argument("-v", "--soc-version", help="soc version.")
    return parser.parse_args()


if __name__ == "__main__":
    args = args_parse()
    ini_optypes = get_optypes(args)
    res = check_optype_duplicate(args, ini_optypes)
    sys.exit(res)
