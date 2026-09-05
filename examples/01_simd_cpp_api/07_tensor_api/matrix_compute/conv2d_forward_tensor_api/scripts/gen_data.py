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

import os
import argparse
import numpy as np


np.random.seed(19)

BATCH = 4
C0 = 16
C1 = 2
CIN = C1 * C0  # 32
H = 9
W = 9
COUT = 16
KH = 3
KW = 3
STRIDE = (2, 2)
DILATION = (2, 2)
PADDING = (2, 2, 2, 2)  # top, bottom, left, right
PAD_VALUE = 1


def conv2d_nhwc(x, weight, stride, dilation, padding, pad_value=0):
    stride_h, stride_w = stride
    dilation_h, dilation_w = dilation
    pad_top, pad_bottom, pad_left, pad_right = padding
    n, h, w, cin = x.shape
    cout, _, kh, kw = weight.shape
    hout = (h + pad_top + pad_bottom - dilation_h * (kh - 1) - 1) // stride_h + 1
    wout = (w + pad_left + pad_right - dilation_w * (kw - 1) - 1) // stride_w + 1
    x_pad = np.pad(
        x,
        ((0, 0), (pad_top, pad_bottom), (pad_left, pad_right), (0, 0)),
        mode="constant",
        constant_values=pad_value,
    )
    y = np.zeros((n, hout, wout, cout), dtype=np.float32)

    for n_idx in range(n):
        for ho in range(hout):
            for wo in range(wout):
                for co in range(cout):
                    acc = 0.0
                    for ci in range(cin):
                        for hk in range(kh):
                            for wk in range(kw):
                                hi = ho * stride_h + hk * dilation_h
                                wi = wo * stride_w + wk * dilation_w
                                acc += float(x_pad[n_idx, hi, wi, ci]) * float(
                                    weight[co, ci, hk, wk]
                                )
                    y[n_idx, ho, wo, co] = acc
    return y


def weight_to_k_cout(weight):
    # weight: [Cout, Cin, KH, KW]，Cin = C1 * C0。K 序 = (C1, KH, KW, C0)，与 img2col 硬件展开顺序一致。
    cout, cin, kh, kw = weight.shape
    k = cin * kh * kw
    out = np.zeros((k, cout), dtype=np.float16)
    for co in range(cout):
        for c1 in range(C1):
            for c0 in range(C0):
                ci = c1 * C0 + c0
                for hk in range(kh):
                    for wk in range(kw):
                        k_idx = ((c1 * kh + hk) * kw + wk) * C0 + c0
                        out[k_idx, co] = weight[co, ci, hk, wk]
    return out.reshape(-1)


def to_nc1hwc0(x_nhwc, c1):
    # x_nhwc: [N, H, W, C]，C = c1 * C0 -> NC1HWC0 排布。
    n, h, w, c = x_nhwc.shape
    assert c == c1 * C0
    x = x_nhwc.reshape(n, h, w, c1, C0)
    # NC1HWC0：N, C1, H, W, C0
    x = np.transpose(x, (0, 3, 1, 2, 4))
    return np.ascontiguousarray(x).reshape(-1)


def to_nchw(x_nhwc):
    # x_nhwc: [N, H, W, C] -> NCHW: [N, C, H, W]。
    return np.ascontiguousarray(np.transpose(x_nhwc, (0, 3, 1, 2))).reshape(-1)


def gen_golden_data(scenario):
    # scenario: 0=NC1HWC0, 1=NCHW, 2=NHWC（与样例 SCENARIO_NUM 对应）。
    fmap = np.random.uniform(-1, 1, (BATCH, H, W, CIN)).astype(np.float16)
    weight = np.random.uniform(-1, 1, (COUT, CIN, KH, KW)).astype(np.float16)

    golden = conv2d_nhwc(fmap, weight, STRIDE, DILATION, PADDING, PAD_VALUE).astype(
        np.float16
    )

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    # fmap GM 按场景摆盘。
    if scenario == 1:
        to_nchw(fmap).tofile("./input/x1_gm.bin")
    elif scenario == 2:
        np.ascontiguousarray(fmap).reshape(-1).tofile("./input/x1_gm.bin")
    else:
        to_nc1hwc0(fmap, C1).tofile("./input/x1_gm.bin")

    # weight GM 始终按 [K, Cout] 行优先（K 序 = (C1, KH, KW, C0)）。
    weight_to_k_cout(weight).tofile("./input/x2_gm.bin")

    # golden GM 按场景摆盘。
    if scenario == 1:
        to_nchw(golden).tofile("./output/golden.bin")
    elif scenario == 2:
        np.ascontiguousarray(golden).reshape(-1).tofile("./output/golden.bin")
    else:
        to_nc1hwc0(golden, COUT // C0).tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--scenario",
        type=int,
        default=0,
        help="0=NC1HWC0, 1=NCHW, 2=NHWC (must match SCENARIO_NUM used to build)",
    )
    args = parser.parse_args()
    gen_golden_data(args.scenario)
