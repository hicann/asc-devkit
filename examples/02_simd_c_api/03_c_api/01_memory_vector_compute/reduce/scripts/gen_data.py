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


ELEMENT_COUNT = 64


def main():
    parser = argparse.ArgumentParser(
        description="Generate deterministic reduce input and golden data."
    )
    parser.add_argument("-scenario_num", type=int, choices=[1, 2], default=1)
    args = parser.parse_args()

    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(exist_ok=True)
    output_dir.mkdir(exist_ok=True)

    input_data = (np.arange(ELEMENT_COUNT, dtype=np.float32) % 16 + 1).astype(
        np.float32
    )
    if args.scenario_num == 1:
        golden = np.array([np.sum(input_data, dtype=np.float32)], dtype=np.float32)
    else:
        golden = np.sum(input_data.reshape(-1, 8), axis=1, dtype=np.float32)
    input_data.tofile(input_dir / "input_x.bin")
    golden.tofile(output_dir / "golden.bin")


if __name__ == "__main__":
    main()
