# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""slim 头（6 字段）解析与冻结词表加载（asc-search 自含工具）。

不依赖其他 skill：词表为本 skill 配置 `config/tag-taxonomy.json`（随包分发的冻结词表）；
推导契约见 references/index_derivation_contract.md。
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

import yaml

SKILL_ROOT = Path(__file__).resolve().parents[2]
TAXONOMY_PATH = SKILL_ROOT / "config" / "tag-taxonomy.json"
DEPENDENCY_KINDS = {"requires", "configures", "constrained_by", "related", "replaced_by"}


class ValidationError(ValueError):
    pass


def split_front_matter(text: str) -> tuple[dict[str, Any], str, str]:
    if not text.startswith("---\n"):
        return {}, "", text
    end = text.find("\n---\n", 4)
    if end < 0:
        raise ValidationError("front matter is not terminated")
    raw = text[4:end]
    data = yaml.safe_load(raw) or {}
    if not isinstance(data, dict):
        raise ValidationError("front matter must be a mapping")
    return data, raw, text[end + len("\n---\n") :]


def taxonomy(path: Path = TAXONOMY_PATH) -> tuple[tuple[str, ...], dict[str, set[str]]]:
    """冻结词表：返回全部 namespace 及其合法值（不止 required）。"""
    payload = json.loads(path.read_text(encoding="utf-8"))
    if payload.get("status") != "frozen":
        raise ValidationError("taxonomy is not frozen")
    defs = payload.get("namespaces")
    if not isinstance(defs, dict) or not defs:
        raise ValidationError("taxonomy namespaces are invalid")
    values: dict[str, set[str]] = {}
    for namespace, definition in defs.items():
        options = definition.get("values") if isinstance(definition, dict) else None
        if not isinstance(options, dict):
            raise ValidationError(f"taxonomy values missing: {namespace}")
        values[namespace] = set(options)
    order = tuple(defs)
    return order, values
