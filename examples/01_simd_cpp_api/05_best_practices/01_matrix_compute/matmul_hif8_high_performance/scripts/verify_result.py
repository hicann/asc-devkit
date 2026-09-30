#!/usr/bin/env python3

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

import numpy as np

from hif8 import decode


def parse_args():
    parser = argparse.ArgumentParser(description="Verify output from the HIF8 Matmul sample.")
    parser.add_argument("--scenario", type=int, choices=(0, 1, 2), required=True)
    parser.add_argument("--output", default="output/output.bin")
    parser.add_argument("--golden", default="output/golden.bin")
    return parser.parse_args()


def main():
    args = parse_args()
    if args.scenario == 2:
        output_bits = np.fromfile(args.output, dtype=np.uint8)
        golden_bits = np.fromfile(args.golden, dtype=np.uint8)
        if output_bits.shape != golden_bits.shape:
            raise ValueError(f"shape mismatch: {output_bits.shape} != {golden_bits.shape}")
        equal = output_bits == golden_bits
        output = decode(output_bits)
        golden = decode(golden_bits)
    else:
        output = np.fromfile(args.output, dtype=np.float32)
        golden = np.fromfile(args.golden, dtype=np.float32)
        if output.shape != golden.shape:
            raise ValueError(f"shape mismatch: {output.shape} != {golden.shape}")
        equal = np.isclose(output, golden, rtol=1e-3, atol=1e-3, equal_nan=True)

    error_ratio = 1.0 - float(np.count_nonzero(equal)) / equal.size
    max_abs_error = float(np.nanmax(np.abs(output - golden)))
    print(f"error ratio: {error_ratio:.6f}")
    print(f"max absolute error: {max_abs_error:.6f}")
    if error_ratio > 1e-3:
        indices = np.flatnonzero(~equal)[:10]
        for index in indices:
            print(f"index={index}, expected={golden[index]}, actual={output[index]}")
        raise ValueError("result verification failed")
    print("test pass!")


if __name__ == "__main__":
    main()
