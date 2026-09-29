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

"""NPU 异常 dump 与输入/输出 tensor dump 对外接口。

能力分两类：
1. 异常自动 dump（依赖外挂库 so）：kernel 异常时 C++ 回调自动落盘公共信息（_info.txt）
   与原始 kernel args（_args.bin，含全部 GM 指针与 tiling）；
2. 输入/输出 tensor dump（纯 Python 实现，不依赖 so、无需重新编译）：
   在算子调用边界 dump 输入/输出 tensor 的完整数据（.bin）与元信息（.json），
   与异常 dump 产物交叉验证（.json 中的 data_ptr 与 _args.bin 中的 GM 指针对应）。

tensor dump 产物命名（与 C++ 侧异常产物风格对齐）：
    {stage}_{tag}_{name}_dev{idx}_{timestamp}.bin   tensor 原始数据
    {stage}_{tag}_{name}_dev{idx}_{timestamp}.json  元信息（shape/dtype/device/data_ptr 等）
其中 stage 为 input/output（dump_tensor 单独调用时可省略），name 中的非法字符替换为 '_'。
dump 目录决策链与 C++ 侧 GetDumpDir 一致：入参 dump_dir > 环境变量 NPUOPS_DUMP_DIR > ./exception_dump/
"""

import json
import os
import re
import time

from ._binding import init as _init

__all__ = ["enable_exception_dump", "dump_tensor", "dump_tensors", "run_with_tensor_dump"]

# tensor dump 默认输出目录（与 C++ 侧 GetDumpDir 默认值保持一致）
_DEFAULT_DUMP_DIR = "./exception_dump/"

# 文件名安全字符白名单（ASCII 字母/数字/下划线，与 C++ 侧 safeName 逻辑对齐）
_UNSAFE_CHARS = re.compile(r"[^0-9A-Za-z_]")


def enable_exception_dump(kernels=None):
    """开启 NPU kernel 异常自动 dump。

    Args:
        kernels: kernel name 前缀列表，用于过滤需要 dump 的算子。
                 - None 或 []: 对所有 kernel 生效
                 - ["kernel_A", "kernel_B"]: 仅对指定前缀的 kernel 进行 dump

    Example:
        # 对所有 kernel 生效
        enable_exception_dump()

        # 仅开启指定前缀的算子
        enable_exception_dump(["incre_flash_attention"])
    """
    ret = _init(kernels)
    if ret != 0:
        raise RuntimeError(f"npuopsExceptionDumpInit failed, ret={ret}")


# ============================== 内部工具 ==============================


def _get_dump_dir(dump_dir=None):
    """dump 目录决策链：入参 > NPUOPS_DUMP_DIR 环境变量 > 默认目录（与 C++ 侧一致）。"""
    if dump_dir:
        return dump_dir
    env = os.environ.get("NPUOPS_DUMP_DIR", "")
    if env:
        return env
    return _DEFAULT_DUMP_DIR


def _timestamp():
    """时间戳字符串，风格与 C++ 侧 GetTimestamp 一致：20260924_103307_467。"""
    now = time.time()
    ms = int((now % 1) * 1000)
    return time.strftime("%Y%m%d_%H%M%S", time.localtime(now)) + "_%03d" % ms


def _safe_name(name):
    """文件名安全化：非 [0-9A-Za-z_] 字符替换为 '_'，空名兜底 'tensor'。"""
    s = _UNSAFE_CHARS.sub("_", str(name))
    return s if s else "tensor"


def _device_tag(tensor):
    """device 段命名：npu:0 -> dev0；cpu -> cpu。"""
    dev = tensor.device
    if dev.type == "cpu" or dev.index is None:
        return "cpu" if dev.type == "cpu" else "dev" + str(dev.index or 0)
    return "dev" + str(dev.index)


def _iter_named_tensors(obj, prefix=""):
    """递归展开 obj 中的 tensor，产出 (name, tensor)。

    支持结构：Tensor 直接产出；list/tuple 展开为 {prefix}[{i}]；
    dict 展开为 {prefix}.{key}（无 prefix 时直接用 key）。标量与其他类型忽略。
    """
    import torch

    if isinstance(obj, torch.Tensor):
        yield prefix if prefix else "tensor", obj
    elif isinstance(obj, (list, tuple)):
        for i, v in enumerate(obj):
            yield from _iter_named_tensors(v, ("%s[%d]" % (prefix, i)) if prefix else ("arg[%d]" % i))
    elif isinstance(obj, dict):
        for k, v in obj.items():
            yield from _iter_named_tensors(v, ("%s.%s" % (prefix, k)) if prefix else str(k))


def _tensor_to_bytes(tensor):
    """tensor 数据转原始字节（host 侧）。

    实现要点（保证服务器环境理论可运行）：
    - NPU tensor 走 .cpu() 标准 D2H（torch_npu 提供，同步拷贝）；
    - bf16/fp16 等 numpy 不直接支持的 dtype，通过 reshape(-1).view(torch.uint8)
      以字节视角导出（读回方式见 README），规避 .numpy() 对 bf16 的限制；
    - 0-dim tensor 先 reshape(-1) 变 1-D 再 view；空 tensor 直接返回 b""。
    """
    import torch

    t = tensor.detach()
    if t.device.type != "cpu":
        t = t.to("cpu")  # NPU -> CPU 标准 D2H，同步拷贝
    t = t.contiguous()
    if t.numel() == 0:
        return b""
    return t.reshape(-1).view(torch.uint8).numpy().tobytes()


# ============================== tensor dump API ==============================


def dump_tensor(tensor, name, stage=None, dump_dir=None):
    """dump 单个 tensor：数据落 .bin、元信息落 .json，返回 .bin 路径。

    Args:
        tensor:    待 dump 的 tensor（NPU 或 CPU 均可）
        name:      tensor 名称（用于产物命名，非法字符自动替换为 '_'）
        stage:     阶段标签，文件名前缀，常用 "input" / "output"；None 时无前缀
        dump_dir:  输出目录；缺省走 NPUOPS_DUMP_DIR 环境变量 -> ./exception_dump/

    元信息 (.json) 记录：name/stage/shape/dtype/itemsize/numel/nbytes/
    device/data_ptr/bin_file/timestamp，其中 data_ptr 为 dump 时刻 device 端地址，
    可与异常 dump 的 _args.bin 中的 GM 指针交叉验证。
    """
    import torch

    if not isinstance(tensor, torch.Tensor):
        raise TypeError("dump_tensor expects a torch.Tensor, got %s" % type(tensor).__name__)

    data_ptr = tensor.data_ptr()  # 在 D2H 前取 device 端地址，用于与 _args.bin 交叉验证
    device = str(tensor.device)
    dev_tag = _device_tag(tensor)
    ts = _timestamp()
    safe = _safe_name(name)
    prefix = ("%s_%s" % (_safe_name(stage), safe)) if stage else safe

    out_dir = _get_dump_dir(dump_dir)
    os.makedirs(out_dir, exist_ok=True)  # 与 C++ 侧 EnsureDir 对应
    bin_path = os.path.join(out_dir, "%s_%s_%s.bin" % (prefix, dev_tag, ts))

    data = _tensor_to_bytes(tensor)
    with open(bin_path, "wb") as fp:
        fp.write(data)

    meta = {
        "name": str(name),
        "stage": stage if stage else "",
        "shape": list(tensor.shape),
        "dtype": str(tensor.dtype),
        "itemsize": tensor.element_size(),
        "numel": tensor.numel(),
        "nbytes": tensor.numel() * tensor.element_size(),
        "device": device,
        "data_ptr": "0x%x" % data_ptr if data_ptr else "0x0",
        "bin_file": os.path.basename(bin_path),
        "timestamp": ts,
    }
    with open(os.path.join(out_dir, "%s_%s_%s.json" % (prefix, dev_tag, ts)), "w", encoding="utf-8") as fp:
        json.dump(meta, fp, indent=2, ensure_ascii=False)

    print(
        "[NPUOPS][INFO] dumped %s tensor '%s' (%d bytes) -> %s"
        % (stage if stage else "tensor", name, len(data), bin_path)
    )
    return bin_path


def dump_tensors(named_tensors, stage=None, dump_dir=None):
    """批量 dump 一组命名 tensor。

    Args:
        named_tensors: dict[str, Tensor]，如 {"query": q, "block_table": bt}
        stage:         阶段标签（"input" / "output"），仅影响产物文件名前缀
        dump_dir:      输出目录，决策链同 dump_tensor

    Returns:
        list[str]: 各 .bin 产物路径
    """
    paths = []
    for name, tensor in named_tensors.items():
        paths.append(dump_tensor(tensor, name, stage=stage, dump_dir=dump_dir))
    return paths


def run_with_tensor_dump(func, *args, tag=None, dump_inputs=True, dump_outputs=True, dump_dir=None, **kwargs):
    """在算子调用边界自动 dump 输入/输出 tensor，并原样返回调用结果。

    时机与语义：
    - 调用前：递归扫描 args/kwargs 中的全部 tensor（含 dict/tuple 嵌套），
      dump 为 input_ 前缀产物 —— 异常场景下输入现场在故障注入后、下发前已落盘；
    - 正常返回后：dump 返回值中的全部 tensor 为 output_ 前缀产物；
    - 调用抛异常：输入已落盘，输出不产出（kernel 未正常写回，输出数据不可得，
      指针现场由异常回调的 _args.bin 提供），异常原样上抛。

    单个 tensor dump 失败仅打印告警、不中断业务调用（保证故障复现流程不受影响）。

    Args:
        func:         被包装的算子/函数（如 torch.ops.ascend_ops.xxx）
        tag:          产物命名中的算子标签；缺省尝试取 func.__name__，取不到用 "op"
        dump_inputs:  是否 dump 输入 tensor（默认 True）
        dump_outputs: 是否 dump 输出 tensor（默认 True）
        dump_dir:     输出目录，决策链同 dump_tensor

    Example:
        out, lse = run_with_tensor_dump(
            torch.ops.ascend_ops.npu_fused_infer_attention_score,
            tag="npu_fused_infer_attention_score", **params)
    """
    tag = _safe_name(tag or getattr(func, "__name__", "") or "op")

    def _dump_all(named, stage):
        for name, tensor in named:
            try:
                dump_tensor(tensor, "%s_%s" % (tag, name), stage=stage, dump_dir=dump_dir)
            except Exception as e:  # 单个 tensor dump 失败不影响业务调用
                print("[NPUOPS][WARN] dump %s tensor '%s' failed: %s" % (stage, name, e))

    if dump_inputs:
        named = {}
        for i, a in enumerate(args):
            for k, v in _iter_named_tensors(a, "arg%d" % i):
                named[k] = v
        for key, v in kwargs.items():
            for k, t in _iter_named_tensors(v, key):
                named[k] = t
        _dump_all(named.items(), "input")

    result = func(*args, **kwargs)

    if dump_outputs:
        outputs = {}
        for k, v in _iter_named_tensors(result, "ret"):
            outputs[k] = v
        _dump_all(outputs.items(), "output")

    return result
