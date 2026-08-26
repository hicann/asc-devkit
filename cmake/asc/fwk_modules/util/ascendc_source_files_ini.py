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

import configparser
import argparse
import os
import stat
import hashlib

from opdesc_parser import _trans_short_soc_to_soc_version


def args_parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--auto-gen-path", required=True, help="auto gen path")
    parser.add_argument("-n", "--target-name", required=True, help="kernel target name")
    parser.add_argument("-t", "--op-type", required=True, help="op type")
    parser.add_argument("-f", "--op-kernel-file", help="op kernel file")
    soc_group = parser.add_mutually_exclusive_group()
    soc_group.add_argument("-c", "--compute-unit", nargs="*", help="compute unit")
    soc_group.add_argument("-s", "--soc-series", nargs="*", help="soc series")
    parser.add_argument("-d", "--op-kernel-dir", help="op kernel dir")
    args = parser.parse_args()
    if args.soc_series:
        args.compute_unit = [
            " ".join(
                _trans_short_soc_to_soc_version(soc_ver)
                for soc_ver in args.soc_series[0].strip().split()
            )
        ]
    return args


def is_only_filename(s):
    """判断字符串是否仅为文件名（不包含任何目录路径）"""
    # 检查是否包含路径分隔符
    return os.sep not in s and os.altsep not in s if os.altsep else os.sep not in s


def is_relative_path(path):
    """判断路径是否为相对路径"""
    return not os.path.isabs(path)


def gen_custom_source_files(args):
    gen_file_path = os.path.join(
        args.auto_gen_path, args.target_name + "_custom_source_files.ini"
    )
    src_ini_config = None
    if os.path.exists(gen_file_path):
        src_ini_config = configparser.ConfigParser()
        src_ini_config.read(gen_file_path)
    flags = os.O_WRONLY | os.O_CREAT
    modes = stat.S_IWUSR | stat.S_IRUSR
    try:
        with os.fdopen(os.open(gen_file_path, flags, modes), "a") as fd:
            hash_value = hashlib.sha256(
                (
                    args.op_type
                    + str(args.op_kernel_file)
                    + str(args.op_kernel_dir)
                    + str(args.compute_unit)
                ).encode("utf-8")
            ).hexdigest()
            if (
                src_ini_config is not None
                and hash_value + "_" + args.op_type in src_ini_config.sections()
            ):
                return True
            fd.write("[" + hash_value + "_" + args.op_type + "]" + "\n")
            if args.op_kernel_dir is not None:
                flag = is_relative_path(args.op_kernel_dir)
                if not flag:
                    print(
                        "[ERROR] op kernel dir should be relative path to kernel base dir"
                    )
                    return flag
                fd.write("kernel_dir=" + args.op_kernel_dir + "\n")
            if args.op_kernel_file is not None:
                fd.write("kernel_file=" + args.op_kernel_file + "\n")
            if args.compute_unit is not None:
                tmp_compute_unit = ",".join(
                    x.strip() for x in args.compute_unit[0].strip().split()
                )
                fd.write("compute_unit=" + tmp_compute_unit + "\n")
    except Exception:
        print("write custom source files ini failed")
        return False
    return True


if __name__ == "__main__":
    args = args_parse()
    ret = gen_custom_source_files(args)
    if not ret:
        exit(1)
