# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""Consume first-support versions and scoped lifecycle events without producer imports."""

from __future__ import annotations

from typing import Any
from .gate import version_key


def event_applies(event: dict[str, Any], identifier: str, arch: str) -> bool:
    scope = event.get("scope", "")
    if not scope.startswith("overload:"):
        return False
    target, _, event_arch = scope[len("overload:") :].partition("@")
    return target == identifier and (not event_arch or event_arch == arch)


def page_available(events: list[dict[str, Any]], target: str) -> bool:
    # A fallback page selector does not establish that the entire API was removed.
    return not any(
        e.get("kind") == "removed"
        and e.get("scope") == "page"
        and e.get("scope_basis") != "fallback"
        and version_key(e["since"]) <= version_key(target)
        for e in events
    )


def signature_available(signature: dict[str, Any], events: list[dict[str, Any]], target: str, arch: str) -> bool:
    relevant = [e for e in events if event_applies(e, signature["override_id"], arch)]
    added = [e["since"] for e in relevant if e["kind"] in {"introduced", "overload_added"}]
    if added and version_key(target) < min(map(version_key, added)):
        return False
    if any(e["kind"] == "removed" and version_key(e["since"]) <= version_key(target) for e in relevant):
        return False
    return True


def deprecation(signature: dict[str, Any], events: list[dict[str, Any]], target: str, arch: str) -> Any:
    value = signature.get("deprecated")
    if value:
        declared = value.get("version")
        if not declared:
            return value
        try:
            in_effect = version_key(declared) <= version_key(target)
        except ValueError:
            # 畸形声明版本（如 ASC_DEPRECATED(9..2, ...)）视为该声明证据不可用，退回事件推导，
            # 避免单个头文件错误中断整次索引构建。
            in_effect = False
        if in_effect:
            return value
    candidates = []
    for event in events:
        if event["kind"] != "deprecated" or version_key(event["since"]) > version_key(target):
            continue
        if event_applies(event, signature["override_id"], arch):
            specificity = 0 if "@" in event["scope"] else 1
        elif event["scope"] == "page" and event.get("scope_basis") != "fallback":
            specificity = 2
        else:
            continue
        candidates.append((specificity, version_key(event["since"]), event["since"]))
    if not candidates:
        return None
    # 事件给出生效的废弃版本，但声明自身已标注的替代目标仍要保留（见推导契约）。
    replacement = (value or {}).get("replacement", "")
    return {"version": min(candidates)[2], "replacement": replacement}
