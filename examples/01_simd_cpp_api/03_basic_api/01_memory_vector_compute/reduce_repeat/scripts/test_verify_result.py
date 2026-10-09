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


import struct
import subprocess
import sys
from pathlib import Path

import pytest


SCRIPT = Path(__file__).with_name("verify_result.py")


def record(value, index):
    return struct.pack("=eH", value, index)


@pytest.mark.parametrize(
    "scenario,output,golden,expected",
    [
        pytest.param(1, struct.pack("=e", 1), struct.pack("=e", 1), 0, id="scenario-1"),
        pytest.param(3, struct.pack("=f", 1), struct.pack("=f", 1), 0, id="scenario-3"),
        pytest.param(4, struct.pack("=ff", 1, 2), struct.pack("=ff", 1, 2), 0, id="scenario-4"),
        pytest.param(2, record(1, 0), record(1, 0), 0, id="matching-record"),
        pytest.param(2, record(1, 65535), record(1, 65535), 0, id="unsigned-index"),
        pytest.param(2, record(1.0009765625, 0), record(1, 0), 0, id="within-tolerance"),
        pytest.param(2, record(1, 1), record(1, 0), 1, id="index-zero-to-one"),
        pytest.param(2, record(1, 0) * 1999 + record(1, 1), record(1, 0) * 2000, 1, id="single-index-error"),
        pytest.param(2, record(2, 0), record(1, 0), 1, id="wrong-value"),
        pytest.param(
            2, record(1, 0) * 599 + record(2, 0), record(1, 0) * 600, 1, id="value-error-ratio-excludes-indices"
        ),
        pytest.param(2, record(1, 0), record(1, 0) * 2, 1, id="truncated-output"),
        pytest.param(2, record(1, 0) * 2, record(1, 0), 1, id="extra-record"),
        pytest.param(2, record(1, 0) + b"x", record(1, 0), 1, id="trailing-byte"),
        pytest.param(2, record(1, 0)[:2], record(1, 0)[:2], 1, id="missing-index"),
        pytest.param(2, record(1, 0) + b"x", record(1, 0) + b"x", 1, id="both-incomplete"),
        pytest.param(2, b"", record(1, 0), 1, id="empty-output"),
        pytest.param(2, b"", b"", 1, id="both-empty"),
    ],
)
def test_verifier_exit_status(tmp_path, scenario, output, golden, expected):
    output_path = tmp_path / "output.bin"
    golden_path = tmp_path / "golden.bin"
    output_path.write_bytes(output)
    golden_path.write_bytes(golden)
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "-scenarioNum", str(scenario), str(output_path), str(golden_path)],
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == expected, result.stdout + result.stderr
    assert ("test pass!" in result.stdout) == (expected == 0)
