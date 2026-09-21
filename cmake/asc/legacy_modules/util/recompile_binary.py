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
import glob
import os
import subprocess


class TooFewLinkCmd(Exception):
    """Link cmd is too few."""


class TooMuchLinkCmd(Exception):
    """Link cmd  is too much."""


class TooFewObj(Exception):
    """Obj is too few."""


class CompileCMDError(Exception):
    """Compiler cmd error."""


def get_link_file(root_dir, target_name):
    link_file = f"{root_dir}/CMakeFiles/{target_name}.dir/link.txt"

    if os.path.exists(link_file):
        return link_file
    else:
        raise FileNotFoundError(f"link.txt file does not exist: {link_file}")


def read_file(source_file):
    """read file content."""
    try:
        with open(source_file, encoding="utf-8") as file:
            content = file.readlines()
        return content
    except Exception as err:
        print("[ERROR]: read file failed, filename is {}".format(source_file))
        raise err


def get_link_cmd(link_file):
    contents = read_file(link_file)
    if not contents:
        raise TooFewLinkCmd(f"There are too few compilation commands in the file: {link_file}")
    return contents


def get_add_obj(add_dir):
    objs = glob.glob(f"{add_dir}/**/*.o", recursive=True)
    if not objs:
        raise TooFewObj(f"There is no obj file in this directory: {add_dir}")
    else:
        return " ".join(sorted(objs))


def get_recompile_cmd(origin_cmd, insert_cmd):
    key_words = "host_stub.cpp.o"
    index = origin_cmd.find(key_words)
    if index != -1:
        index += len(key_words)
        return origin_cmd[:index] + " " + insert_cmd + " " + origin_cmd[index:]
    else:
        return origin_cmd


def run_recompile_cmd(root_dir, compile_cmd):
    """run cmds"""
    cmds = compile_cmd.split()
    print(f"recompile: {compile_cmd}", flush=True)
    result = subprocess.run(cmds, check=True, cwd=root_dir)
    if result.returncode != 0:
        raise CompileCMDError(f"recompile command failed, return code: {result.returncode}")


def parse_args():
    """parse parameters"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--root-dir", required=True, help="target root directory")
    parser.add_argument("--target-name", required=True, help="target name")
    parser.add_argument("--add-dir", required=True, help="the directory where the added obj is located")
    args = parser.parse_args()
    return args


def main():
    """Main process."""
    args = parse_args()
    link_file = get_link_file(args.root_dir, args.target_name)
    link_cmds = get_link_cmd(link_file)
    add_objs = get_add_obj(args.add_dir)
    for link_cmd in link_cmds:
        recompile_cmd = get_recompile_cmd(link_cmd.strip(), add_objs)
        run_recompile_cmd(args.root_dir, recompile_cmd)


if __name__ == "__main__":
    main()
