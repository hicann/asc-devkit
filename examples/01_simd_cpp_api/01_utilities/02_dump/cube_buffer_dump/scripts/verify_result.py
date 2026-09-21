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

import re
import sys
from pathlib import Path

import numpy as np


OUTPUT_ELEMENT_COUNT = 16 * 16
A_ELEMENT_COUNT = 16 * 32
A_L1_DUMP_ELEMENT_COUNT = 32
BIAS_ELEMENT_COUNT = 16
QUANT_ELEMENT_COUNT = 16
HIFLOAT8_ONE_BITS = 0x08
HIFLOAT8_ONE_VALUE = 1.0
FIXPIPE_QUANT_VISIBLE_MASK = 0x00007FE0FFFFE0FF


def bfloat16_to_float(values):
    return (values.astype(np.uint32) << np.uint32(16)).view(np.float32)


def verify_result(output_path, golden_path):
    output = np.fromfile(output_path, dtype=np.uint16)
    golden = np.fromfile(golden_path, dtype=np.uint16)
    if output.size != OUTPUT_ELEMENT_COUNT or golden.size != OUTPUT_ELEMENT_COUNT:
        print(f"unexpected element count: output={output.size}, golden={golden.size}, expected={OUTPUT_ELEMENT_COUNT}")
        print("Output result check: FAIL")
        return False

    mismatch_indexes = np.flatnonzero(output != golden)
    output_float = bfloat16_to_float(output)
    golden_float = bfloat16_to_float(golden)
    for index in mismatch_indexes[:100]:
        print(
            f"data index: {index:06d}, expected: {golden_float[index]:-.9f} "
            f"(0x{golden[index]:04x}), actual: {output_float[index]:-.9f} (0x{output[index]:04x})"
        )
    passed = mismatch_indexes.size == 0
    print(f"Output result check: {'PASS' if passed else 'FAIL'}")
    return passed


def parse_dump(text, desc, expected_count, value_type=int, expected_fields=()):
    matches = re.findall(rf"DumpTensor: desc={desc},([^\n]*)\n\[(.*?)\]", text, re.S)
    if len(matches) != 1:
        raise RuntimeError(f"expected one DumpTensor desc={desc}, found {len(matches)}")
    header, body = matches[0]
    for field in expected_fields:
        if field not in header:
            raise RuntimeError(f"DumpTensor desc={desc} missing header field: {field}")
    values = [value_type(item.strip()) for item in body.replace("\n", " ").split(",") if item.strip()]
    if len(values) != expected_count:
        raise RuntimeError(
            f"unexpected DumpTensor desc={desc} element count: {len(values)}, expected: {expected_count}"
        )
    return values


def read_dump_input(a_path, bias_path, scale_path):
    a = np.fromfile(a_path, dtype=np.uint8)
    bias = np.fromfile(bias_path, dtype=np.float32)
    quant = np.fromfile(scale_path, dtype=np.uint64)
    if a.size != A_ELEMENT_COUNT:
        raise RuntimeError(f"unexpected matrix A element count: {a.size}")
    if bias.size != BIAS_ELEMENT_COUNT:
        raise RuntimeError(f"unexpected bias element count: {bias.size}")
    if quant.size != QUANT_ELEMENT_COUNT:
        raise RuntimeError(f"unexpected scale element count: {quant.size}")
    if np.any(a[:A_L1_DUMP_ELEMENT_COUNT] != HIFLOAT8_ONE_BITS):
        raise RuntimeError("unexpected matrix A data: expected HiFloat8 1.0 (0x08)")
    a_expected = [HIFLOAT8_ONE_VALUE] * A_L1_DUMP_ELEMENT_COUNT
    return a_expected, bias.tolist(), [int(value) for value in quant]


def check_equal(name, actual, expected):
    if actual == expected:
        print(f"{name}: PASS")
        return True
    print(f"{name}: FAIL")
    if len(actual) != len(expected):
        print(f"  length mismatch: actual={len(actual)}, expected={len(expected)}")
    for index, (actual_value, expected_value) in enumerate(zip(actual, expected)):
        if actual_value != expected_value:
            if isinstance(actual_value, int) and isinstance(expected_value, int):
                print(f"  [{index:02d}] actual=0x{actual_value:016x}, expected=0x{expected_value:016x}")
            else:
                print(f"  [{index:02d}] actual={actual_value}, expected={expected_value}")
    return False


def main():
    if len(sys.argv) != 7:
        print(f"Usage: {sys.argv[0]} <output.bin> <golden.bin> <run.log> <a.bin> <bias.bin> <scale.bin>")
        return 1

    text = Path(sys.argv[3]).read_text(encoding="utf-8")
    a_input, bias_input, fixpipe_input = read_dump_input(Path(sys.argv[4]), Path(sys.argv[5]), Path(sys.argv[6]))
    fixpipe_expected = [value & FIXPIPE_QUANT_VISIBLE_MASK for value in fixpipe_input]
    checks = [
        verify_result(sys.argv[1], sys.argv[2]),
        check_equal(
            "HiFloat8 A L1 data check",
            parse_dump(text, 100, A_L1_DUMP_ELEMENT_COUNT, float, ("data_type=hifloat8", "position=L1")),
            a_input,
        ),
        check_equal(
            "Bias L1 raw input check",
            parse_dump(text, 101, BIAS_ELEMENT_COUNT, float, ("data_type=float32", "position=L1")),
            bias_input,
        ),
        check_equal(
            "Bias Table raw data check",
            parse_dump(text, 201, BIAS_ELEMENT_COUNT, float, ("data_type=float32", "position=BIAS")),
            bias_input,
        ),
        check_equal(
            "Fixpipe L1 raw input check",
            parse_dump(text, 102, QUANT_ELEMENT_COUNT, expected_fields=("data_type=uint64", "position=L1")),
            fixpipe_input,
        ),
        check_equal(
            "Fixpipe quant visible-mask check",
            parse_dump(text, 301, QUANT_ELEMENT_COUNT, expected_fields=("data_type=uint64", "position=FIXBUF")),
            fixpipe_expected,
        ),
    ]
    if all(checks):
        print("test pass!")
        return 0
    return 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError, ValueError) as error:
        print(error)
        sys.exit(1)
