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

"""
Check if precompiled kernel.cpp.o contains a template __global__ function definition.
Add src file property respectively in template_config.cmake.
"""

import sys
import re
import os
import argparse
from typing import Iterator, List, Tuple
from host_stub_util import ExtractError, ArgumentError
from extract_host_stub import do_save_commands


class ParseSrcFilePathError(ExtractError):
    """Parse src file path from precompile files error."""


def get_template_config_filepath(dst_dir: str) -> str:
    """Get template_config.cmake destination file path."""
    return os.path.join(dst_dir, "template_config.cmake")


def generate_template_config_code(filepaths: List[str]) -> str:
    """Parse precompiled src file, generate template_config.cmake."""
    config_options = ""
    disable_kernel_check_option = "--cce-disable-kernel-global-attr-check"
    src_file_pattern = r'#\s*\d+\s*"([^"]+)"'
    template_kernel_func_pattern = (
        r"template<([^<>]*(?:<[^<>]*>)*[^<>]*)>\s*__attribute__\(\(cce_kernel\)\)"
        r"\s*(?:\[aicore\]|__attribute__\(\(cce_aicore\)\))\s*(.+?)\s*\{"
    )
    for path in filepaths:
        try:
            with open(path, encoding="utf-8") as file:
                data = file.read()
                first_line = data.splitlines()[0]
                src_file_match = re.search(src_file_pattern, first_line)
                if src_file_match:
                    src_file_path = src_file_match.group(1)
                else:
                    raise ParseSrcFilePathError()
                template_match = re.compile(template_kernel_func_pattern, re.DOTALL)
                if not template_match.search(data):
                    config_options += f"set_source_files_properties({src_file_path} \
PROPERTIES COMPILE_OPTIONS {disable_kernel_check_option})\n"

        except Exception as err:
            print("[ERROR]: read file failed, filename is: {}".format(path))
            raise err
    return config_options


def generate_save_template_config_commands(filepaths: List[str], dst_dir: str) -> Iterator[Tuple[str, str]]:
    """Generate save template_config commands."""
    yield (get_template_config_filepath(dst_dir), generate_template_config_code(filepaths))


def main(argv: List[str]):
    parser = argparse.ArgumentParser()
    parser.add_argument("filepaths", nargs="+", help="Preprocessed file paths.")
    parser.add_argument("-d", "--dst-dir", default=".", help="Destination directory.")

    args = parser.parse_args(argv)

    dst_dir = os.path.realpath(args.dst_dir)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)

    do_save_commands(generate_save_template_config_commands(args.filepaths, dst_dir))

    return True


def main_with_except(argv: List[str]):
    """Main process with except exceptions."""
    try:
        return main(argv)
    except ArgumentError as ex:
        print(f"error: check arguments error, {ex}")
        return False


if __name__ == "__main__":
    if not main_with_except(sys.argv[1:]):
        sys.exit(1)
