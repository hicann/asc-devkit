#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from pathlib import Path

import numpy as np

QUERY_HEADS = 32
KV_HEADS = 4
KV_LENGTH = 4160
HEAD_DIM = 128
SCALE = 0.08838834764831845
SEED = 2026


def to_bfloat16_bits(value):
    bits = value.astype(np.float32).view(np.uint32)
    rounded = bits + 0x7FFF + ((bits >> 16) & 1)
    return ((rounded + 0x8000) >> 16).astype(np.uint16)


def from_bfloat16_bits(bits):
    return (bits.astype(np.uint32) << 16).view(np.float32)


def main():
    rng = np.random.default_rng(SEED)
    query = rng.uniform(-0.5, 0.5, size=(QUERY_HEADS, HEAD_DIM)).astype(np.float32)
    key = rng.uniform(-0.5, 0.5, size=(KV_HEADS, KV_LENGTH, HEAD_DIM)).astype(np.float32)
    value = rng.uniform(-0.5, 0.5, size=(KV_HEADS, KV_LENGTH, HEAD_DIM)).astype(np.float32)

    q_bits = to_bfloat16_bits(query)
    k_bits = to_bfloat16_bits(key)
    v_bits = to_bfloat16_bits(value)
    q = from_bfloat16_bits(q_bits)
    k = from_bfloat16_bits(k_bits)
    v = from_bfloat16_bits(v_bits)

    output = np.empty((QUERY_HEADS, HEAD_DIM), dtype=np.float32)
    for head in range(QUERY_HEADS):
        kv_head = head // (QUERY_HEADS // KV_HEADS)
        scores = np.dot(q[head], k[kv_head].T) * SCALE
        probabilities = np.exp(scores - scores.max())
        output[head] = np.dot(probabilities, v[kv_head]) / probabilities.sum()
    output_bits = to_bfloat16_bits(output)

    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)
    q_bits.tofile(input_dir / "query.bin")
    k_bits.tofile(input_dir / "key.bin")
    v_bits.tofile(input_dir / "value.bin")
    output_bits.tofile(output_dir / "golden.bin")


if __name__ == "__main__":
    main()
