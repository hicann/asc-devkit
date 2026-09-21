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

""""""

import argparse
import os
import io
import subprocess
import sys
import stat
from typing import List


def save_file(filepath: str, content: str):
    """Save file."""
    try:
        with open(filepath, "w", encoding="utf-8") as file:
            os.chmod(filepath, stat.S_IRUSR + stat.S_IWUSR)
            file.write(content)
    except Exception as err:
        print("[ERROR]: write file failed, filename is: {}".format(filepath))
        raise err


def extract_archive(archive_path: str, dst_dir: str):
    """Extract archive into destination directory."""
    subprocess.run(["ar", "-x", archive_path], cwd=dst_dir, check=True)


def get_objects_in_archive(archive_path: str) -> List[str]:
    """Get objects in archive."""
    result = subprocess.run(["ar", "-t", archive_path], check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    display = result.stdout.decode()
    return display.split()


def generate_cmake_config_content(target: str, dst_dir: str, objects: List[str]) -> str:
    all_object_list = [os.path.join(dst_dir, _object) for _object in objects]
    all_objects = ";".join(all_object_list)
    buff = io.StringIO()
    buff.write(f"add_library({target} OBJECT IMPORTED)\n")
    buff.write(f"set_target_properties({target} PROPERTIES\n")
    buff.write(f'    IMPORTED_OBJECTS "{all_objects}"\n')
    buff.write(")\n")
    return buff.getvalue()


def main(argv):
    """Main process."""
    # -a xxx.a -t xxx -d xxx -o xxx.cmake
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--archive", required=True, help="Archive name")
    parser.add_argument("-t", "--target", required=True, help="Output target name.")
    parser.add_argument("-d", "--dst-dir", default=".", help="Destination directory.")
    parser.add_argument("-o", "--cmake", required=True, help="Output cmake config.")
    args = parser.parse_args(argv)

    if not os.path.exists(args.dst_dir):
        os.makedirs(args.dst_dir)

    extract_archive(args.archive, args.dst_dir)

    objects = get_objects_in_archive(args.archive)

    cmake_config_content = generate_cmake_config_content(args.target, args.dst_dir, objects)

    save_file(os.path.join(args.dst_dir, args.cmake), cmake_config_content)


if __name__ == "__main__":
    main(sys.argv[1:])
