#!/usr/bin/python3
# coding=utf-8

# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Generate fixed input and golden data for the L2 Cache split sample."""

import argparse
import struct
from pathlib import Path


TOTAL_DATA_ELEMENTS = 201326592
CHUNK_ELEMENTS = 65536
COMPUTE_ROUND_COUNT = 2


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, choices=[1, 2], default=1)
    return parser.parse_args()


def main():
    args = parse_args()
    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(exist_ok=True)
    output_dir.mkdir(exist_ok=True)

    input_path = input_dir / "input.bin"
    golden_path = output_dir / "golden.bin"
    with input_path.open("wb") as input_file, golden_path.open("wb") as golden_file:
        offset = 0
        while offset < TOTAL_DATA_ELEMENTS:
            count = min(CHUNK_ELEMENTS, TOTAL_DATA_ELEMENTS - offset)
            input_values = [
                float(((offset + index) % 32) - 16) for index in range(count)
            ]
            golden_values = [value + COMPUTE_ROUND_COUNT for value in input_values]
            input_file.write(struct.pack(f"<{count}e", *input_values))
            golden_file.write(struct.pack(f"<{count}e", *golden_values))
            offset += count

    bytes_per_half = 2
    total_data_mib = TOTAL_DATA_ELEMENTS * bytes_per_half / 1024 / 1024
    print(
        f"scenarioNum={args.scenarioNum}, total_data_elements={TOTAL_DATA_ELEMENTS}, "
        f"total_data={total_data_mib:.0f} MB"
    )


if __name__ == "__main__":
    main()
