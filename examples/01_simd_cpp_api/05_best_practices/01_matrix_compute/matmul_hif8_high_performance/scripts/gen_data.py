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
import os

import numpy as np

from hif8 import decode, encode


M = 1024
N = 1024
K = 1024


def parse_args():
    parser = argparse.ArgumentParser(description="Generate input and golden data for the HIF8 Matmul sample.")
    parser.add_argument("--scenario", type=int, choices=(0, 1, 2), required=True)
    return parser.parse_args()


def main():
    args = parse_args()
    rng = np.random.default_rng(2026)
    candidates = np.array([-1.0, -0.5, 0.0, 0.5, 1.0], dtype=np.float32)
    a_float = rng.choice(candidates, size=(M, K))
    b_float = rng.choice(candidates, size=(K, N))

    if args.scenario == 0:
        a_input = a_float.astype(np.float16)
        b_input = b_float.astype(np.float16)
        golden = np.matmul(a_input.astype(np.float32), b_input.astype(np.float32)).astype(np.float32)
    else:
        a_input = encode(a_float)
        b_input = encode(b_float)
        golden_float = np.matmul(decode(a_input), decode(b_input)).astype(np.float32)
        golden = encode(golden_float) if args.scenario == 2 else golden_float

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    a_input.tofile("input/a.bin")
    b_input.tofile("input/b.bin")
    golden.tofile("output/golden.bin")
    print(f"generated data for scenario {args.scenario}")


if __name__ == "__main__":
    main()
