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

from __future__ import annotations

import os
import re
import shutil
import subprocess
from typing import Optional


def detect_arch(explicit: Optional[str] = None) -> Optional[str]:
    if explicit:
        return explicit
    env_arch = os.environ.get("PRESMOKE_ARCH")
    if env_arch:
        return env_arch
    npu_smi = shutil.which("npu-smi")
    if not npu_smi:
        return None
    try:
        proc = subprocess.run([npu_smi, "info"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=5)
    except (OSError, subprocess.TimeoutExpired):
        return None
    text = proc.stdout.lower()
    if "910b" in text or "910" in text or "a2" in text:
        return "dav-2201"
    if "950" in text or "a3" in text:
        return "dav-3510"
    found = re.search(r"dav-\d+", text)
    if found:
        return found.group(0)
    return None
