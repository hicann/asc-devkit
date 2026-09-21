#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import os
import sys


def get_file_action(path):
    """
    Determine CI action based on file path.
    Returns:
        "SKIP"    - Skip all (no compile and test, no package)
        "PKG"     - Trigger package build
        "COMPILE" - Normal compile
    """
    skip_keywords = [".md", "OWNERS", "LICENSE", "classify_rule.yaml", "docs"]
    pkg_keywords = ["examples"]

    for kw in skip_keywords:
        if kw in path:
            return "SKIP"

    for kw in pkg_keywords:
        if kw in path:
            return "PKG"

    return "COMPILE"


def main():
    """
    Get change info from ci, CI_MODE=True, ci will write 'get diff >/change_file.txt'
    Returns:
        "SKIP"    - Skip all (docs, OWNERS, .md only)
        "PKG"     - Trigger package build (examples only)
        "COMPILE" - Normal compile (source code changes)
    """
    if len(sys.argv) < 2:
        return "COMPILE"

    changed_files_arg = sys.argv[1]
    try:
        if os.path.isfile(changed_files_arg) and changed_files_arg.endswith(".txt"):
            with open(changed_files_arg, "r") as f:
                changed_files = [line.strip() for line in f.readlines() if line.strip()]
        elif "," in changed_files_arg:
            changed_files = [f.strip() for f in changed_files_arg.split(",") if f.strip()]
        else:
            changed_files = [changed_files_arg.strip()]
    except BaseException:
        return "COMPILE"

    if not changed_files:
        return "COMPILE"

    actions = [get_file_action(f) for f in changed_files]

    if all(a == "SKIP" for a in actions):
        return "SKIP"

    if any(a == "PKG" for a in actions):
        if not any(a == "COMPILE" for a in actions):
            return "PKG"
        else:
            return "COMPILE"

    return "COMPILE"


if __name__ == "__main__":
    sys.stdout.write(main())
