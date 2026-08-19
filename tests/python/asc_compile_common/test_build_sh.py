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
import subprocess
from pathlib import Path

import pytest


BUILD_SCRIPT = Path(__file__).resolve().parents[3] / "build.sh"


@pytest.mark.parametrize("trailing_args", [[], ["--build-type", "Debug"]])
def test_sign_script_requires_value(trailing_args):
    result = subprocess.run(
        ["bash", str(BUILD_SCRIPT), "--pkg", "--sign-script", *trailing_args],
        capture_output=True,
        text=True,
        check=False,
    )

    assert result.returncode == 1
    assert "[ERROR] --sign-script requires a value." in result.stdout
