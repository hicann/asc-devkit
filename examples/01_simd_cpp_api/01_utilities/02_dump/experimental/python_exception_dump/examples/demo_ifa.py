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

# ----------------------------------------------------------------------------
# demo_ifa.py - IFA (incre_flash_attention) 异常自动 dump demo
#
# 流程: enable_exception_dump() -> 正常跑一遍算子（dump 输入/输出 tensor）->
#       构造非法 block_table 触发核内越界异常（dump 污染输入）->
#       callback 自动 dump 公共信息 + kernel args -> 校验产物
#
# 产物（均在 NPUOPS_DUMP_DIR 目录）：
#   input_*.bin/.json    输入 tensor 数据与元信息（调用边界，Python 侧 dump）
#   output_*.bin/.json   输出 tensor 数据与元信息（正常返回后，Python 侧 dump）
#   *_args.bin / *_info.txt  异常公共信息与原始 kernel args（C++ 回调 dump）
# ----------------------------------------------------------------------------
import os
import sys
import glob

# dump 配置（环境变量说明见 README "配置（环境变量）"）
DUMP_DIR = os.path.abspath("./exception_dump_ifa")
os.environ["NPUOPS_DUMP_DIR"] = DUMP_DIR
os.environ["NPUOPS_DUMP_LEVEL"] = "1"  # 1 = 公共信息 + args

PKG_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "npuops_exception_dump", "python"))
sys.path.insert(0, PKG_ROOT)

import torch
import torch_npu
import ascend_ops  # noqa: F401  # 注册 ascend_ops torch 算子库
from npuops.debug import enable_exception_dump, run_with_tensor_dump

# ============================== 配置 ==============================
B = 2  # batch
Q_HEAD = 8  # query head num
KV_HEAD = 1  # kv head num
Q_SEQ = 1  # query seq len
BLOCK_SIZE = 128
HEAD_DIM = 128
KV_SEQ = 512  # kv seq len
MAX_BLK_PER_BATCH = KV_SEQ // BLOCK_SIZE + 1
BLOCK_NUM = B * MAX_BLK_PER_BATCH

# IFA kernel args 布局（32 个 GM 指针 * 8B + TilingData），异常时 args 总大小
EXPECTED_ARGS_BYTES = 1264

# 输入 tensor dump 产物大小预期（用于校验）：B*Q_HEAD*Q_SEQ*HEAD_DIM 个 bf16
EXPECTED_QUERY_BYTES = B * Q_HEAD * Q_SEQ * HEAD_DIM * 2

# 算子标签（run_with_tensor_dump 产物命名使用）
OP_TAG = "npu_fused_infer_attention_score"


def gen_inputs(poison_block_table=False):
    q = torch.randn(B, Q_HEAD, Q_SEQ, HEAD_DIM).to(dtype=torch.bfloat16).npu()
    key_cache = torch.randint(0, 100, (BLOCK_NUM, KV_HEAD, HEAD_DIM // 32, BLOCK_SIZE, 32)).to(dtype=torch.int8).npu()
    value_cache = torch.randint(0, 100, (BLOCK_NUM, KV_HEAD, HEAD_DIM // 32, BLOCK_SIZE, 32)).to(dtype=torch.int8).npu()
    if poison_block_table:
        # 故意构造非法 block table（块索引巨大 -> 核内 GM 越界 -> Runtime 异常）
        block_table = torch.full((B, MAX_BLK_PER_BATCH), 0x0FFFFFF0, dtype=torch.int32).npu()
    else:
        block_table = torch.arange(B * MAX_BLK_PER_BATCH, dtype=torch.int32).view(B, MAX_BLK_PER_BATCH).npu()
    actual_seq_kvlen = torch.tensor([KV_SEQ] * B, dtype=torch.int64).npu()
    dequant_scale_key = torch.randn(KV_HEAD, 1, HEAD_DIM).to(dtype=torch.bfloat16).npu()
    dequant_scale_value = torch.randn(KV_HEAD, 1, HEAD_DIM).to(dtype=torch.bfloat16).npu()

    meta_param = {
        "batch_size": B,
        "query_seq_size": Q_SEQ,
        "query_head_num": Q_HEAD,
        "key_head_num": KV_HEAD,
        "head_dim": HEAD_DIM,
        "block_size": BLOCK_SIZE,
        "max_block_num_per_batch": MAX_BLK_PER_BATCH,
        "actual_seq_lengths_kv": actual_seq_kvlen,
        "layout_query": "BNSD",
    }
    metadata = torch.ops.ascend_ops.npu_fused_infer_attention_score_metadata(**meta_param)

    fa_param = {
        "query": q,
        "key": key_cache,
        "value": value_cache,
        "actual_seq_kvlen": actual_seq_kvlen,
        "block_table": block_table,
        "dequant_scale_key": dequant_scale_key,
        "dequant_scale_value": dequant_scale_value,
        "num_query_heads": Q_HEAD,
        "num_key_value_heads": KV_HEAD,
        "softmax_scale": 1.0 / (HEAD_DIM**0.5),
        "block_size": BLOCK_SIZE,
        "input_layout": "BNSD",
        "sparse_mode": 0,
        "inner_precise": 1,
        "key_quant_mode": 0,
        "value_quant_mode": 0,
        "metadata": metadata,
    }
    return fa_param


def check_dump_products():
    files = sorted(glob.glob(os.path.join(DUMP_DIR, "*")))
    if not files:
        print("[FAIL] no dump files found")
        return False
    print("\n===== dump 产物 =====")
    ok = True
    for f in files:
        print(f"  {os.path.basename(f)}  ({os.path.getsize(f)} bytes)")
    names = " ".join(os.path.basename(f) for f in files)
    # 1) 异常 dump 产物：公共信息 + 原始 args
    for need in ["_args.bin", "_info.txt"]:
        if need not in names:
            print(f"[FAIL] missing dump file: *{need}")
            ok = False
    # args.bin 大小与 kernel args 布局一致
    args_files = [f for f in files if f.endswith("_args.bin")]
    if args_files:
        size = os.path.getsize(args_files[0])
        if size == EXPECTED_ARGS_BYTES:
            print(f"[SIZE-OK] args.bin: {size} bytes == expected {EXPECTED_ARGS_BYTES}")
        else:
            print(f"[FAIL] args.bin: {size} bytes != expected {EXPECTED_ARGS_BYTES}")
            ok = False
    # info.txt 公共信息校验（kernel name / error code）
    info_files = [f for f in files if f.endswith("_info.txt")]
    if info_files:
        with open(info_files[0]) as fp:
            content = fp.read()
        for expect in ["kernel_name : incre_flash_attention", "error_code  : 0x7bc87", "args_dumped : yes"]:
            if expect in content:
                print(f"[INFO-OK] info.txt contains '{expect}'")
            else:
                print(f"[FAIL] info.txt missing '{expect}'")
                ok = False
    # 2) 输入/输出 tensor dump 产物（Python 侧，调用边界产出）
    input_bins = [f for f in files if os.path.basename(f).startswith("input_") and f.endswith(".bin")]
    output_bins = [f for f in files if os.path.basename(f).startswith("output_") and f.endswith(".bin")]
    if not input_bins:
        print("[FAIL] missing input tensor dump files: input_*.bin")
        ok = False
    else:
        print(f"[INPUT-OK] input tensor dump: {len(input_bins)} files")
    if not output_bins:
        print("[FAIL] missing output tensor dump files: output_*.bin")
        ok = False
    else:
        print(f"[OUTPUT-OK] output tensor dump: {len(output_bins)} files")
    # 每个 input_*.bin 均有同名 .json 元信息
    jsons = {os.path.basename(f) for f in files if f.endswith(".json")}
    for f in input_bins:
        json_name = os.path.basename(f)[: -len(".bin")] + ".json"
        if json_name not in jsons:
            print(f"[FAIL] missing json meta for {os.path.basename(f)}")
            ok = False
    # query 输入数据大小校验（正常与异常两次调用均会 dump query）
    query_bins = [f for f in input_bins if "_query_" in os.path.basename(f)]
    if query_bins:
        size = os.path.getsize(query_bins[0])
        if size == EXPECTED_QUERY_BYTES:
            print(f"[SIZE-OK] input query tensor: {size} bytes == expected {EXPECTED_QUERY_BYTES}")
        else:
            print(f"[FAIL] input query tensor: {size} bytes != expected {EXPECTED_QUERY_BYTES}")
            ok = False
    else:
        print("[FAIL] missing input query tensor dump")
        ok = False
    # 污染 block_table 读回校验：任一 block_table 输入首个 int32 == 0x0FFFFFF0
    bt_bins = [f for f in input_bins if "_block_table_" in os.path.basename(f)]
    bt_poisoned = False
    for f in bt_bins:
        with open(f, "rb") as fp:
            head = fp.read(4)
        if len(head) == 4 and int.from_bytes(head, byteorder="little") == 0x0FFFFFF0:
            bt_poisoned = True
            print(f"[DATA-OK] poisoned block_table dump detected: {os.path.basename(f)} (first int32 == 0x0FFFFFF0)")
            break
    if not bt_poisoned:
        print("[FAIL] no poisoned block_table dump found (first int32 should be 0x0FFFFFF0)")
        ok = False
    print("[PASS] IFA exception dump OK" if ok else "[FAIL] check failed")
    return ok


def main():
    torch.npu.set_device(0)

    # 1. 开启异常 dump（仅 IFA kernel；重复调用幂等）
    enable_exception_dump(["incre_flash_attention"])

    # 2. 正常跑通算子（验证环境与输入构造正确），调用边界 dump 输入/输出 tensor
    print("\n===== step1: normal run (dump input & output tensors) =====")
    out, lse = run_with_tensor_dump(torch.ops.ascend_ops.npu_fused_infer_attention_score, tag=OP_TAG, **gen_inputs())
    torch.npu.synchronize()
    print(f"normal run OK, output shape: {tuple(out.shape)}")

    # 3. 构造非法 block_table 触发核内越界异常（污染输入在调用前自动落盘，输出不可得）
    print("\n===== step2: trigger exception (poisoned block_table) =====")
    fa_param = gen_inputs(poison_block_table=True)
    try:
        out, lse = run_with_tensor_dump(torch.ops.ascend_ops.npu_fused_infer_attention_score, tag=OP_TAG, **fa_param)
        torch.npu.synchronize()
        print("[WARN] no exception raised?!")
    except RuntimeError as e:
        print(f"caught expected RuntimeError: {str(e)[:300]}")

    # 4. 校验 dump 产物（异常产物 + tensor 产物的存在性、大小、内容）
    print("\n===== step3: check dump products =====")
    return check_dump_products()


if __name__ == "__main__":
    sys.exit(0 if main() else 1)
