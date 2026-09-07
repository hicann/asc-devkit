#!/usr/bin/python3
# coding=utf-8

# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Verify output data for the L2 Cache split sample."""

import argparse
import struct
import sys
from pathlib import Path


BYTES_PER_HALF = 2
CHUNK_BYTES = 1024 * 1024


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("output")
    parser.add_argument("golden")
    parser.add_argument("-scenarioNum", type=int, choices=[1, 2], default=1)
    return parser.parse_args()


def half_to_float(data):
    return struct.unpack("<e", data)[0]


def verify_result(output_path, golden_path):
    output_file_path = Path(output_path)
    golden_file_path = Path(golden_path)
    output_size = output_file_path.stat().st_size
    golden_size = golden_file_path.stat().st_size
    if output_size != golden_size:
        print(
            f"[ERROR] file size mismatch: output={output_size} bytes, "
            f"golden={golden_size} bytes"
        )
        return False
    if output_size % BYTES_PER_HALF != 0:
        print(f"[ERROR] invalid output file size: {output_size} bytes")
        return False

    with output_file_path.open("rb") as output_file, golden_file_path.open(
        "rb"
    ) as golden_file:
        base_index = 0
        while True:
            output_data = output_file.read(CHUNK_BYTES)
            golden_data = golden_file.read(CHUNK_BYTES)
            if not output_data and not golden_data:
                return True
            if output_data == golden_data:
                base_index += len(output_data) // BYTES_PER_HALF
                continue

            for offset in range(0, len(output_data), BYTES_PER_HALF):
                output_half = output_data[offset : offset + BYTES_PER_HALF]
                golden_half = golden_data[offset : offset + BYTES_PER_HALF]
                if output_half == golden_half:
                    continue
                index = base_index + offset // BYTES_PER_HALF
                print(
                    f"[ERROR] result mismatch at index {index}: "
                    f"actual={half_to_float(output_half)}, "
                    f"expected={half_to_float(golden_half)}"
                )
                return False


def main():
    args = parse_args()
    if not verify_result(args.output, args.golden):
        print("test failed!")
        return 1
    print("test pass!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
