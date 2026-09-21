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

import configparser
import argparse
import os
import shutil
from pathlib import Path
import warnings


def args_parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--auto-gen-path", required=True, help="auto gen path")
    parser.add_argument("-n", "--target-name", required=True, help="kernel target name")
    parser.add_argument("-s", "--kernel-base-dir", required=True, help="kernel base dir")
    parser.add_argument("-d", "--copy-dst-dir", required=True, help="copy kernel file dst dir")
    return parser.parse_args()


def check_file_extension(file_path, supported_extensions):
    """
    Check if file extension is in the supported list
    :param file_path: File path or filename
    :param supported_extensions: List of supported extensions (e.g. ['.txt', '.cpp'])
    """
    _, ext = os.path.splitext(file_path)
    ext = ext.lower()
    if ext not in supported_extensions:
        warnings.warn(
            f"\n[Warning]: Supported file extension: {supported_extensions}, please check : {file_path}.",
            category=UserWarning,
        )
        return False
    return True


def copy_dir(src_dir, dst_dir, exclude_dir=None):
    try:
        src_dir, dst_dir = map(lambda x: os.path.abspath(x), [src_dir, dst_dir])
        if not os.path.isdir(src_dir):
            raise NotADirectoryError(f"kernel src dir not exists: {src_dir}")
        if exclude_dir is not None and is_subdirectory(exclude_dir, src_dir):
            warnings.warn(
                f"\n[Warning]: Copy Warning, src_dir include dst_dir."
                f"\n[Warning]: src: KERNEL_DIR:{src_dir}"
                f"\n[Warning]: dst :{exclude_dir}",
                category=UserWarning,
            )
        if os.path.islink(src_dir):
            src_dir = os.path.abspath(os.readlink(src_dir))
        for dir_path, dirs, items in os.walk(src_dir):
            if exclude_dir is not None and is_subdirectory(dir_path, exclude_dir):
                continue
            for item in items + dirs:
                src_item = os.path.abspath(os.path.join(dir_path, item))
                dst_item = os.path.abspath(os.path.join(dst_dir, dir_path.replace(src_dir, "."), item))
                if os.path.islink(src_item):
                    raise ValueError(f"unsupport soft link file, please check it. {os.path.abspath(src_item)}")
                elif os.path.isdir(src_item):
                    continue
                if not check_file_extension(src_item, ["", ".txt", ".h", ".hpp", ".cpp", ".c"]):
                    warnings.warn(
                        f"\n[Warning]: Unexpected file type found in {os.path.abspath(src_item)}"
                        f"\n[Warning]: Please check dir is required and correct : {os.path.abspath(dir_path)}",
                        category=UserWarning,
                    )
                copy_file(src_item, dst_item)
    except Exception as e:
        raise RuntimeError("copy kernel dir failed: {}".format(e))


def copy_file(src_file, dst_file):
    try:
        if not os.path.exists(src_file):
            raise FileNotFoundError(f"kernel src file {os.path.abspath(src_file)} not found, please check it.")
        os.makedirs(os.path.dirname(dst_file), exist_ok=True)
        if (not src_file.endswith(".txt")) and (not os.path.exists(dst_file)):
            if os.path.islink(src_file):
                raise ValueError(f"unsupport soft link file, please check it. {os.path.abspath(src_file)}")
            elif os.path.isfile(src_file):
                shutil.copy2(src_file, dst_file)
            else:
                raise TypeError(f"undefined type : {os.path.abspath(src_file)}")
    except Exception as e:
        raise RuntimeError("copy kernel file failed: {}".format(e))


def is_subdirectory(child_path, parent_path):
    child = os.path.abspath(os.path.normpath(child_path))
    parent = os.path.abspath(os.path.normpath(parent_path))
    if child == parent:
        return True
    parent_with_sep = parent + os.sep
    return child.startswith(parent_with_sep)


def is_safe_relative_path(path: str) -> bool:
    if not path:
        return False
    path_obj = Path(path)
    if path_obj.is_absolute():
        return False
    stack = []
    for part in path_obj.parts:
        if part == "..":
            if stack:
                stack.pop()
            else:
                return False
        elif part == ".":
            continue
        else:
            stack.append(part)
    return True


def copy_kernel_src_file(args):
    kernel_source_ini_file_path = os.path.join(args.auto_gen_path, args.target_name + "_custom_source_files.ini")
    if os.path.exists(args.copy_dst_dir):
        shutil.rmtree(args.copy_dst_dir)
    suffixes_to_ignore = [".txt"]
    cmake_current_binary_path = os.path.abspath(os.path.join(args.copy_dst_dir, "..", "..", ".."))
    if not os.path.exists(kernel_source_ini_file_path):
        copy_dir(args.kernel_base_dir, args.copy_dst_dir, exclude_dir=cmake_current_binary_path)
    kernel_src_config = configparser.ConfigParser()
    kernel_src_config.read(kernel_source_ini_file_path)
    sections = kernel_src_config.sections()
    for section in sections:
        if kernel_src_config.has_option(section, "kernel_dir"):
            sub_dir = kernel_src_config.get(section, "kernel_dir")
            if not is_safe_relative_path(sub_dir):
                raise ValueError(
                    f"Unsafe relative path: [{sub_dir}], please ensure base-directory-independent path structure.\n"
                )
            copy_dir(
                os.path.join(args.kernel_base_dir, sub_dir),
                os.path.join(args.copy_dst_dir, sub_dir),
                exclude_dir=cmake_current_binary_path,
            )
        if kernel_src_config.has_option(section, "kernel_file"):
            file_name = kernel_src_config.get(section, "kernel_file")
            sub_dir = (
                kernel_src_config.get(section, "kernel_dir")
                if kernel_src_config.has_option(section, "kernel_dir")
                else "."
            )
            sub_dir = os.path.dirname(os.path.join(sub_dir, file_name))
            file_name = os.path.basename(file_name)
            if not is_safe_relative_path(sub_dir):
                raise ValueError(
                    f"Unsafe relative path: [{sub_dir}], please ensure base-directory-independent path structure.\n"
                )
            src_file = os.path.join(args.kernel_base_dir, sub_dir, file_name)
            dst_file = os.path.join(args.copy_dst_dir, sub_dir, file_name)
            copy_file(src_file, dst_file)


if __name__ == "__main__":
    args = args_parse()
    copy_kernel_src_file(args)
