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

import argparse
from pathlib import Path

import numpy as np


ELEMENT_COUNT = 512


def make_input():
    return ((np.arange(ELEMENT_COUNT, dtype=np.float32) % 31) - 15 + 0.25).astype(
        np.float16
    )


def make_golden(input_data, scenario_num):
    rounded = np.ceil(input_data.astype(np.float32))
    if scenario_num == 1:
        converted = np.clip(rounded, -8, 7).astype(np.int8).astype(np.uint8)
        return (converted[0::2] | (converted[1::2] << 4)).astype(np.uint8)
    return rounded.astype(np.int32)


def main():
    parser = argparse.ArgumentParser(
        description="Generate deterministic cast input and golden data."
    )
    parser.add_argument("-scenario_num", type=int, choices=[1, 2], default=1)
    args = parser.parse_args()

    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(exist_ok=True)
    output_dir.mkdir(exist_ok=True)

    input_data = make_input()
    input_data.tofile(input_dir / "input_x.bin")
    make_golden(input_data, args.scenario_num).tofile(output_dir / "golden.bin")


if __name__ == "__main__":
    main()
