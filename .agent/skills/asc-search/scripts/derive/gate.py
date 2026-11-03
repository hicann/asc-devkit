# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""索引筛选条件：CANN 版本检测、架构映射与版本区间判定。

检测链（references/index_derivation_contract.md 第 4 节）：--cann-version 显式参数 > $ASCEND_HOME_PATH
安装目录版本 > 仓 version.cmake。架构标签经 product-map.yaml 映射架构号。
"""

from __future__ import annotations

import os
import re
from pathlib import Path
from typing import Any

import yaml

VERSION_POINT_RE = re.compile(r"^(\d+(?:\.\d+)*)(?:-([0-9A-Za-z.-]+))?$")
VERSION_RANGE_RE = re.compile(r"^>=(?P<floor>\d+(?:\.\d+){0,2}(?:-[0-9A-Za-z.-]+)?)$")
LEGACY_RANGE_RE = re.compile(r"^(>=[0-9.]+),<=[0-9.]+$")


def version_key(value: str) -> tuple:
    """版本号归一化：接受 1 段以上的数字段（如 9.2 / 9.2.0 / 9.2.0.1）与可选预发布后缀。"""
    match = VERSION_POINT_RE.fullmatch(value)
    if not match:
        raise ValueError(f"invalid CANN version: {value!r}")
    numbers, pre = match.groups()
    parts = [int(part) for part in numbers.split(".")]
    major, minor, patch = (parts + [0, 0])[:3]
    extra = tuple(parts[3:])
    pre_parts = tuple((0, int(p)) if p.isdigit() else (1, p) for p in (pre or "").split("."))
    return major, minor, patch, extra, pre is None, pre_parts


def first_support(value: str, *, allow_legacy: bool = False) -> str:
    if not isinstance(value, str):
        raise ValueError("version must be >=first_support_version")
    if allow_legacy:
        legacy = LEGACY_RANGE_RE.fullmatch(value)
        if legacy:
            value = legacy[1]
    if not VERSION_RANGE_RE.fullmatch(value):
        raise ValueError(f"version must contain only >=first_support_version: {value!r}")
    return value


def load_product_map(path: Path) -> dict[str, Any]:
    payload = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict) or "archs" not in payload:
        raise ValueError(f"invalid product map: {path}")
    return payload


def arch_label_to_number(label: str, product_map: dict[str, Any]) -> str:
    if label in product_map["archs"]:
        return label
    for arch, info in product_map["archs"].items():
        if label in info.get("labels", []):
            return str(arch)
    raise ValueError(f"unknown arch label: {label!r}; known architecture numbers: {list(product_map['archs'])}")


def npu_to_arch(marker: str, product_map: dict[str, Any]) -> str | None:
    for arch, info in product_map["archs"].items():
        if marker in info.get("npu_markers", []):
            return str(arch)
    return None


def detect_cann_version(explicit: str | None, repo_root: Path) -> str:
    if explicit:
        version_key(explicit)
        return explicit
    home = os.environ.get("ASCEND_HOME_PATH", "")
    if home:
        match = re.search(r"cann-(\d+\.\d+(?:\.\d+)*(?:-[0-9A-Za-z.-]+)?)(?=/|$)", home)
        if match:
            return match.group(1)
    cmake = repo_root / "version.cmake"
    if cmake.is_file():
        match = re.search(r'VERSION\s+"([^"\n]+)"', cmake.read_text(encoding="utf-8"))
        if match:
            version_key(match.group(1))
            return match.group(1)
    raise ValueError("cannot detect local CANN version; pass --cann-version explicitly")


def version_covers(version: str, cann_version: str) -> bool:
    """First support only. Deprecation never imposes an availability upper bound."""
    try:
        return version_key(cann_version) >= version_key(first_support(version)[2:])
    except ValueError:
        return False


def version_sort_key(version: str) -> tuple:
    return version_key(first_support(version)[2:])
