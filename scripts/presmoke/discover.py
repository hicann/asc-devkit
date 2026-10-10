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

from __future__ import annotations

from pathlib import Path
from typing import Iterable, List


def discover_examples(examples_root: Path) -> List[Path]:
    examples: List[Path] = []
    for readme in examples_root.rglob("README.md"):
        text = readme.read_text(encoding="utf-8", errors="ignore")
        if "## 编译运行" in text or "## 编译与运行" in text or "## 运行方式" in text:
            examples.append(readme.parent)
    return sorted(examples, key=lambda p: p.relative_to(examples_root).as_posix())


def filter_examples(
    paths: Iterable[Path], includes: Iterable[str], excludes: Iterable[str], exact_includes: Iterable[str] | None = None
) -> List[Path]:
    include_terms = [x for x in includes if x]
    exact_include_terms = {x for x in (exact_includes or []) if x}
    exclude_terms = [x for x in excludes if x]
    result: List[Path] = []
    for path in paths:
        text = path.as_posix()
        exact_match = any(text == term or text.endswith(f"/{term}") for term in exact_include_terms)
        if exact_include_terms and not exact_match:
            continue
        if include_terms and not any(term in text for term in include_terms):
            continue
        if exclude_terms and any(term in text for term in exclude_terms):
            continue
        result.append(path)
    return result
