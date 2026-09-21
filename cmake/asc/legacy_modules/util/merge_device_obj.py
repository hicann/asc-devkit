#!/usr/bin/env python3
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
import os
import subprocess


class CompareError(Exception):
    """Files are inconsistent."""


class MergeCMDError(Exception):
    """Merge command failed."""


def run_merge_cmd(script, linker, build_type, input_file, output_file):
    """run cmds"""

    input_str = " ".join(input_file)

    output_dir = os.path.dirname(output_file)
    output_name = os.path.basename(output_file)

    cmd_str = f"bash {script} -l {linker} -o {output_dir} -t {build_type} -m -n {output_name} {input_str}"

    cmds = cmd_str.split()
    result = subprocess.run(cmds, check=False, close_fds=False)
    if result.returncode != 0:
        raise MergeCMDError(f"Merge command failed, return code: {result.returncode}")


def single_obj_merge(script, linker, build_type, all_merge_list):
    for merge_list in all_merge_list:
        output_file = merge_list[0]
        input_file = merge_list[1:]
        run_merge_cmd(script, linker, build_type, input_file, output_file)


def read_file(source_file):
    """read file content."""
    try:
        with open(source_file, encoding="utf-8") as file:
            content = file.readlines()
        return content
    except Exception as err:
        print("[ERROR]: read file failed, filename is {}".format(source_file))
        raise err


def get_obj_name(obj_file):
    """get obj name."""
    obj_base_name = os.path.basename(obj_file)
    obj_split_name = obj_base_name.split(".")
    prefix = obj_split_name[0]
    file_name = prefix.replace("auto_gen_", "")
    suffix = obj_split_name[-1]
    return rf"{file_name}.{suffix}"


def get_normal_obj_info(input_dir, file_name, output_dir):
    obj_list = []
    merge_list = []
    cfg_file = os.path.join(input_dir, file_name)

    if not os.path.exists(cfg_file):
        return obj_list, merge_list

    contents = read_file(cfg_file)
    for content in contents:
        obj_file = content.strip()
        if obj_file:
            obj_name = get_obj_name(obj_file)
            dst_file = os.path.join(output_dir, obj_name)
            merge_list.append((dst_file, obj_file))
            obj_list.append(obj_file)
    return obj_list, merge_list


def get_mix_obj_info(aic_dir, aiv_dir, file_name, output_dir):
    obj_list = []
    merge_list = []
    aic_cfg_file = os.path.join(aic_dir, file_name)
    aiv_cfg_file = os.path.join(aiv_dir, file_name)

    if (not os.path.exists(aic_cfg_file)) or (not os.path.exists(aiv_cfg_file)):
        return obj_list, merge_list

    aic_contents = read_file(aic_cfg_file)
    aiv_contents = read_file(aiv_cfg_file)
    for aic_content, aiv_content in zip(aic_contents, aiv_contents):
        aic_obj = aic_content.strip()
        aiv_obj = aiv_content.strip()
        if (not aic_obj) or (not aiv_obj):
            continue

        aic_basename = os.path.basename(aic_obj)
        aiv_basename = os.path.basename(aiv_obj)
        if aic_basename != aiv_basename:
            raise CompareError(f"aic: {aic_obj}, aiv: {aiv_obj}")

        obj_name = get_obj_name(aic_obj)
        dst_file = os.path.join(output_dir, obj_name)
        merge_list.append((dst_file, aic_obj, aiv_obj))
        # append aic ahead of aiv
        obj_list.append(aic_obj)
        obj_list.append(aiv_obj)

    return obj_list, merge_list


def parse_args():
    """parse parameters"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--linker", required=True, help="ccec linker")

    parser.add_argument("-o", "--output", required=True, help="output directory")

    parser.add_argument("--build-type", required=True, help="build type")

    parser.add_argument("--script", required=True, help="merge script")

    parser.add_argument("--aiv-dir", nargs="?", help="aiv directory")

    parser.add_argument("--aic-dir", nargs="?", help="aic directory")

    parser.add_argument("--normal-dir", nargs="?", help="normal directory")

    parser.add_argument("-n", "--name", nargs="?", help="output name")

    parser.add_argument("--fatbin", action="store_true", help="fatbin mode")

    args = parser.parse_args()

    return args


def main():
    """Main process."""
    args = parse_args()

    all_source_list = []
    all_merge_list = []

    if args.normal_dir:
        normal_objs, merge_normal = get_normal_obj_info(args.normal_dir, "mix_build.flag", args.output)
        all_source_list.extend(normal_objs)
        all_merge_list.extend(merge_normal)
    else:
        aic_objs, merge_aic = get_normal_obj_info(args.aic_dir, "aic_build.flag", args.output)
        all_source_list.extend(aic_objs)
        all_merge_list.extend(merge_aic)

        aiv_objs, merge_aiv = get_normal_obj_info(args.aiv_dir, "aiv_build.flag", args.output)
        all_source_list.extend(aiv_objs)
        all_merge_list.extend(merge_aiv)

        mix_objs, merge_mix = get_mix_obj_info(args.aic_dir, args.aiv_dir, "mix_build.flag", args.output)
        all_source_list.extend(mix_objs)
        all_merge_list.extend(merge_mix)

    if args.fatbin:
        output_file = os.path.join(args.output, args.name)
        run_merge_cmd(args.script, args.linker, args.build_type, all_source_list, output_file)
    else:
        single_obj_merge(args.script, args.linker, args.build_type, all_merge_list)


if __name__ == "__main__":
    main()
